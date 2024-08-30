// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include <Include.h>
#include <sdk/sdk.h>

#include "options.hpp"
#include <array>
#include <fstream>
#include <iterator>
#include <string>
#include <tools/loader/loader.h>
#include <tools/platform.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace {
    [[nodiscard]] auto get_required_modules() {
        // clang-format off
        return std::to_array<std::string>({
            // @note: @es3n1n: modules that we'll use in our code
            loader::get_module_file_name("client"),
            loader::get_module_file_name("engine2"),
            loader::get_module_file_name("schemasystem"),
            loader::get_module_file_name("tier0"),

#if defined(DOTA2)
            // @note: @soufiw: latest modules that gets loaded in the main menu
            loader::get_module_file_name("navsystem"),
#elif defined(CS2)
            loader::get_module_file_name("matchmaking"),
#endif

            // modules that we'll dump (minus the ones listed above)
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
    struct module_dump {
        std::unordered_set<const CSchemaEnumBinding*> enums{};
        std::unordered_set<const CSchemaClassBinding*> classes{};
    };

    /// @return Key is the module name
    std::unordered_map<std::string, module_dump> collect_modules(std::span<CSchemaSystemTypeScope*> type_scopes) {
        struct unique_module_dump {
            /// Key is the enum name. Used for de-duplication.
            std::unordered_map<std::string, CSchemaEnumBinding*> enums{};
            /// Key is the class name. Used for de-duplication.
            std::unordered_map<std::string, CSchemaClassBinding*> classes{};
        };

        // Key is the module name, e.g. SchemaEnumInfoData_t::m_pszModule.
        std::unordered_map<std::string, unique_module_dump> dumped_modules{};

        for (const auto* current_scope : type_scopes) {
            auto current_enums = current_scope->GetEnumBindings();
            for (auto el : current_enums.GetElements()) {
                auto& dump{dumped_modules.emplace(el->m_pszModule, unique_module_dump{}).first->second};
                dump.enums.emplace(el->m_pszName, el);
            }

            auto current_classes = current_scope->GetClassBindings();
            for (auto el : current_classes.GetElements()) {
                auto& dump{dumped_modules.emplace(el->m_pszModule, unique_module_dump{}).first->second};
                dump.classes.emplace(el->m_pszName, el);
            }
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

    bool Dump(const Options options) try {
        // set up the allocator before anything else. we can't use allocating
        // C++ functions without it.
        const auto loaded = loader::load_module(LOADER_GET_MODULE_FILE_NAME("tier0"));
        if (!loaded.has_value()) {
            // don't use any allocating C++ functions in here.
            std::fputs("Could not load tier0. Is " IF_LINUX("LD_LIBRARY_PATH") IF_WINDOWS("PATH") " set?\n", stderr);
            std::fputs(loaded.error().as_string().data(), stderr);
            std::fputc('\n', stderr);
            std::abort();
        }
        static_cast<void>(GetMemAlloc());

        std::locale::global(std::locale(""));
        std::cout.imbue(std::locale());

        const auto modules = get_required_modules();

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

        const std::unordered_map all_modules = collect_modules(std::span{type_scopes.m_pElements, static_cast<std::size_t>(type_scopes.m_Size)});

        sdk::GeneratorCache cache{};

        for (const auto& [module_name, dump] : all_modules) {
            sdk::GenerateTypeScopeSdk(options, cache, module_name, dump.enums, dump.classes);
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
