// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include "options.hpp"
#include "tools/util.h"
#include <array>
#include <filesystem>
#include <fstream>
#include <Include.h>
#include <iostream>
#include <iterator>
#include <sdk/sdk.h>
#include <span>
#include <string>
#include <tools/loader/loader.h>
#include <tools/platform.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace {
    [[nodiscard]] auto GetRequiredModules() {
        // clang-format off
        return std::to_array<std::string>({
            loader::get_module_file_name("client"),
            loader::get_module_file_name("engine2"),
            loader::get_module_file_name("schemasystem"),
            loader::get_module_file_name("tier0"),

#if defined(DOTA2)
            loader::get_module_file_name("navsystem"),
#elif defined(CS2)
            loader::get_module_file_name("matchmaking"),
#endif

            loader::get_module_file_name("animationsystem"),
            loader::get_module_file_name("host"),
            loader::get_module_file_name("materialsystem2"),
            loader::get_module_file_name("meshsystem"),
            loader::get_module_file_name("networksystem"),
            loader::get_module_file_name("panorama"),
            loader::get_module_file_name("particles"),
            loader::get_module_file_name("pulse_system"),
            IF_WINDOWS(loader::get_module_file_name("rendersystemdx11"),)
            loader::get_module_file_name("resourcesystem"),
            loader::get_module_file_name("scenefilecache"),
            loader::get_module_file_name("scenesystem"),
            loader::get_module_file_name("server"),
            loader::get_module_file_name("soundsystem"),
            loader::get_module_file_name("vphysics2"),
            loader::get_module_file_name("worldrenderer"),
            IF_WINDOWS(loader::get_module_file_name("assetpreview"),)
        });
        // clang-format on
    }
} // namespace

namespace source2_gen {
    // TODO: this duplicate of the constant in sdk.h. We should let the user specify the sdk path via Options.
    constexpr std::string_view kOutDirName = "sdk";
    using sdk::GenerateTypeScopeSdk;

    struct module_dump {
        std::unordered_set<const CSchemaEnumBinding*> enums{};
        std::unordered_set<const CSchemaClassBinding*> classes{};
    };

    /// @return Key is the module name
    std::unordered_map<std::string, module_dump> CollectModules(std::span<CSchemaSystemTypeScope*> type_scopes) {
        struct unique_module_dump {
            /// Key is the enum name. Used for de-duplication.
            std::unordered_map<std::string, CSchemaEnumBinding*> enums{};
            /// Key is the class name. Used for de-duplication.
            std::unordered_map<std::string, CSchemaClassBinding*> classes{};
        };

        // Key is the module name, e.g. SchemaEnumInfoData_t::m_pszModule.
        std::unordered_map<std::string, unique_module_dump> dumped_modules{};

        const auto append_scope = [&dumped_modules](const CSchemaSystemTypeScope* scope) {
            if (scope == nullptr) {
                return;
            }

            auto current_enums = scope->GetEnumBindings();
            for (auto el : current_enums.GetElements()) {
                auto& dump{dumped_modules.emplace(el->m_pszModule, unique_module_dump{}).first->second};
                dump.enums.emplace(el->m_pszName, el);
            }

            auto current_classes = scope->GetClassBindings();
            for (auto el : current_classes.GetElements()) {
                auto& dump{dumped_modules.emplace(el->m_pszModule, unique_module_dump{}).first->second};
                dump.classes.emplace(el->m_pszName, el);
            }
        };

        append_scope(sdk::g_schema->GlobalTypeScope());

        for (const auto* current_scope : type_scopes) {
            append_scope(current_scope);
        }

        std::unordered_map<std::string, module_dump> result{};

        for (const auto& [module_name, unique_dump] : dumped_modules) {
            constexpr auto to_set = [](const auto& pair) {
                return pair.second;
            };

            module_dump dump{};

            std::ranges::transform(unique_dump.enums, std::inserter(dump.enums, dump.enums.end()), to_set);

            std::ranges::transform(unique_dump.classes, std::inserter(dump.classes, dump.classes.end()), to_set);

            result.emplace(module_name, dump);
        }

        return result;
    }

    /// A very basic C preprocessor.
    /// Writes contents of @p path to @p out while expanding `#include` directives
    void ExpandIncludesRecursive(std::ofstream& out, std::unordered_set<std::filesystem::path>& seen_files, const std::filesystem::path& path) {
        std::ifstream f(path);
        if (!f.good()) {
            std::cerr << std::format("Could not read from {}: {}", path.string(), std::strerror(errno)) << std::endl;
            std::exit(1);
        }

        std::string line{};
        while (std::getline(f, line)) {
            if (line.starts_with('#')) {
                constexpr std::string_view prefix = "#include \"source2sdk/";

                if (line.starts_with(prefix)) {
                    const std::filesystem::path include_path{std::string{kOutDirName} + "/include/source2sdk/" +
                                                             line.substr(prefix.length(), line.length() - (prefix.length() + 1))};
                    if (!seen_files.contains(include_path)) {
                        seen_files.emplace(include_path);
                        ExpandIncludesRecursive(out, seen_files, include_path);
                    }
                }
            } else {
                out << line << '\n';
            }
        }
    }

    // Post-processes an already-generated C SDK so it can be parsed by IDA.
    // - merges all files into a single file by resolving `#include`s
    void PostProcessCIDA(const std::unordered_set<std::filesystem::path>& generated_files) {
        const auto out_file_path = std::string{kOutDirName} + "/ida.h";
        std::ofstream out(out_file_path, std::ios::out);

        std::unordered_set<std::filesystem::path> seen_files{};

        for (const auto& file : generated_files) {
            ExpandIncludesRecursive(out, seen_files, file);
        }
    }

    [[nodiscard]]
    constexpr std::string_view GetStaticSdkName(Language language) {
        using enum Language;
        switch (language) {
        case cpp:
            return "cpp";
        case c:
        case c_ida:
            return "c";
        }

        assert(false && "unhandled enumerator");
    }

    [[nodiscard]]
    std::filesystem::path FindSdkStatic(const Options& options) {
        const auto directories = std::format("sdk-static/{}", GetStaticSdkName(options.emit_language));

        /// Try from the current cwd first
        if (auto path = std::filesystem::path(directories); is_directory(path)) {
            return path;
        }

        /// On windows, cwd will be `source2gen\build\bin\Release`.
        /// Let's walk back until we find our directory.
        constexpr std::size_t kDepth = 4;
        auto cwd = std::filesystem::current_path();
        for (std::size_t i = 0; i < kDepth; ++i) {
            cwd = cwd.parent_path();

            if (auto path = cwd / directories; is_directory(path)) {
                return path;
            }
        }

        throw std::runtime_error(std::format("Unable to find sdk-static: {}", directories));
    }

    bool Dump(Options options) try {
        std::locale::global(std::locale(""));
        std::cout.imbue(std::locale());

        const auto modules = GetRequiredModules();

        for (const auto& name : modules) {
            std::cout << std::format("{}: Loading {}", __FUNCTION__, name) << std::endl;

            if (loader::load_module(name).has_value()) {
                continue;
            }

            // cannot use any functions that use `new` because we've
            // overridden `new` in IMemAlloc.cpp and it relies on
            // libraries being loaded.
            std::cerr << std::format("{}: Unable to load module {}, is {} set?", __FUNCTION__, name, IF_WINDOWS("PATH") IF_LINUX("LD_LIBRARY_PATH"))
                      << std::endl;
            return false;
        }

        std::cout << std::format("{}: Starting", __FUNCTION__) << std::endl;

        // @note: @es3n1n: Capture interfaces
        //
        sdk::g_schema = CSchemaSystem::GetInstance();
        if (!sdk::g_schema) {
            std::cerr << std::format("{}: Unable to obtain Schema interface", __FUNCTION__) << std::endl;
            return false;
        }

        for (const auto& name : modules) {
            auto* handle = loader::find_module_handle(name);
            assert(handle != nullptr && "we loaded modules at startup, where did they go?");

            using InstallSchemaBindingsTy = std::uint8_t (*)(const char*, CSchemaSystem*);
            if (auto InstallSchemaBindings = loader::find_module_symbol<InstallSchemaBindingsTy>(handle, "InstallSchemaBindings");
                InstallSchemaBindings.has_value()) {
                if ((*InstallSchemaBindings)("SchemaSystem_001", sdk::g_schema)) {
                    continue;
                }

                std::cerr << std::format("{}: Unable to install schema bindings in {}", __FUNCTION__, name) << std::endl;
                return false;
            }

            std::cout << std::format("{}: No schemas in {}", __FUNCTION__, name) << std::endl;
        }

        // @note: @es3n1n: Obtaining type scopes and generating sdk
        const auto type_scopes = sdk::g_schema->GetTypeScopes();
        assert(type_scopes.Count() > 0 && "sdk is outdated");

        const std::unordered_map all_modules = CollectModules(std::span{type_scopes.m_pElements, static_cast<std::size_t>(type_scopes.m_Size)});

        sdk::GeneratorCache cache{};
        std::unordered_set<std::filesystem::path> generated_files{};

        for (const auto& [module_name, dump] : all_modules) {
            const auto result = GenerateTypeScopeSdk(options, cache, module_name, dump.enums, dump.classes);
            std::ranges::move(result.generated_files, std::inserter(generated_files, generated_files.end()));
        }

        // Throws an exception with descriptive message. No need for explicit error handling.
        // Need to do this before PostProcessCIDA() because sdk-static contains types that are
        // missing in the generated sdk.
        std::filesystem::copy(FindSdkStatic(options), kOutDirName,
                              std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);

        if (options.emit_language == Language::c_ida) {
            PostProcessCIDA(generated_files);
        }

        std::cout << std::format("Schema stats: {} registrations; {} were redundant; {} were ignored ({} bytes of ignored data)",
                                 util::PrettifyNum(sdk::g_schema->GetRegistration()), util::PrettifyNum(sdk::g_schema->GetRedundant()),
                                 util::PrettifyNum(sdk::g_schema->GetIgnored()), util::PrettifyNum(sdk::g_schema->GetIgnoredBytes()))
                  << std::endl;

        return true;
    } catch (const std::runtime_error& err) {
        std::cout << std::format("{} :: ERROR :: {}", __FUNCTION__, err.what()) << std::endl;
        return false;
    }
} // namespace source2_gen

// source2gen - Source2 games SDK generator
// Copyright 2024 neverlosecc
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
