// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#include <Include.h>
#include <sdk/sdk.h>

#include <array>
#include <fstream>
#include <proc.h>
#include <string>
#include <string_view>

namespace {
    using namespace std::string_view_literals;

    // clang-format off
    constexpr std::array kRequiredGameModules = std::to_array<std::string_view>({
        // @note: @es3n1n: modules that we'll use in our code
        LIBRARY("client"),
        LIBRARY("engine2"),
        LIBRARY("schemasystem"),
        LIBRARY("tier0"),

        #if defined(DOTA2)
        // @note: @soufiw: latest modules that gets loaded in the main menu
        LIBRARY("navsystem"),
        #elif defined(CS2)
        LIBRARY("matchmaking"),
        #endif

        // modules that we'll dump (minus the ones listed above)
        LIBRARY("animationsystem"),
        LIBRARY("host"),
        LIBRARY("materialsystem2"),
        LIBRARY("meshsystem"),
        LIBRARY("networksystem"),
        LIBRARY("panorama"),
        LIBRARY("particles"),
        LIBRARY("pulse_system"),
        // TODO: cfg_windows
        // LIBRARY("rendersystemdx11"),
        LIBRARY("resourcesystem"),
        LIBRARY("scenefilecache"),
        LIBRARY("scenesystem"),
        LIBRARY("server"),
        LIBRARY("soundsystem"),
        LIBRARY("vphysics2"),
        LIBRARY("worldrenderer")
    });
    // clang-format on
} // namespace

namespace source2_gen {
    // TODO: remove
    [[nodiscard]] auto find_module_base(const std::string_view module_name) -> std::optional<std::uintptr_t> {
        auto file = std::ifstream{"/proc/self/maps"};

        if (!file.is_open()) {
            throw std::runtime_error{"cannot open /proc/self/maps for reading"};
        }

        std::string line{};
        while (std::getline(file, line)) {
            if (line.contains("lib" + std::string{module_name} + ".so")) {
                if (line.contains("xp ")) {
                    auto stream = std::istringstream{line};
                    auto base = std::uint64_t{};
                    stream >> std::hex >> base;
                    return base;
                }
            }
        }

        return std::nullopt;
    }

    void Dump() try {
        // @note: @es3n1n: Waiting for game init
        //
        for (const auto& name : kRequiredGameModules) {
            std::cout << "loading " << name << std::endl;
            if (LoadLibraryA(name.data()) == nullptr) {
                // cannot use any functions that use `new` because we've
                // overridden `new` in IMemAlloc.cpp and it relies on
                // libraries being loaded.
                std::fprintf(stderr, "Unable to load library %s, is LD_LIBRARY_PATH set?\n", name.data());
                std::terminate();
            }
        }

        std::cout << std::format("{}: Starting", __FUNCTION__) << std::endl;

        // @note: @es3n1n: Capture interfaces
        //
        sdk::g_schema = CSchemaSystem::GetInstance();
        if (!sdk::g_schema)
            throw std::runtime_error(std::format("Unable to obtain Schema interface"));

        for (const auto& name : kRequiredGameModules) {
            auto* handle = GetModuleHandleA(name.data());
            assert(handle != nullptr && "we loaded kRequiredGameModules at startup, where did they go?");

            if (const auto* pInstallSchemaBindings = GetProcAddress(handle, "InstallSchemaBindings")) {
                auto const InstallSchemaBindings = (std::uint8_t(*)(const char*, CSchemaSystem*))(pInstallSchemaBindings);
                if (!InstallSchemaBindings("SchemaSystem_001", sdk::g_schema)) {
                    std::cerr << std::format("Unable to install schema bindings in {}", name) << std::endl;
                    std::terminate();
                }
            } else {
                std::cout << std::format("No InstallSchemaBindings() in {}", name) << std::endl;
            }
        }

        // @note: @es3n1n: Obtaining type scopes and generating sdk

        const auto type_scopes = sdk::g_schema->GetTypeScopes();
        for (auto i = 0; i < type_scopes.Count(); ++i)
            sdk::GenerateTypeScopeSdk(type_scopes.m_pElements[i]);

        // @note: @es3n1n: Generating sdk for global type scope
        //
        sdk::GenerateTypeScopeSdk(sdk::g_schema->GlobalTypeScope());

        std::cout << std::format("Schema stats: {} registrations; {} were redundant; {} were ignored ({} bytes of ignored data)",
                                 util::PrettifyNum(sdk::g_schema->GetRegistration()), util::PrettifyNum(sdk::g_schema->GetRedundant()),
                                 util::PrettifyNum(sdk::g_schema->GetIgnored()), util::PrettifyNum(sdk::g_schema->GetIgnoredBytes()))
                  << std::endl;
    } catch (std::runtime_error& err) {
        std::cout << std::format("{} :: ERROR :: {}", __FUNCTION__, err.what()) << std::endl;
    }

    void main() {
        Dump();

        std::cout << std::format("Successfully dumped Source 2 SDK, now you can safely close this console.") << std::endl;
        std::cout << kPoweredByMessage << std::endl;
    }
} // namespace source2_gen

// source2gen - Source2 games SDK generator
// Copyright 2023 neverlosecc
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
