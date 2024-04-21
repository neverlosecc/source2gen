// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include <Include.h>
#include <sdk/sdk.h>

#include <array>
#include <fstream>
#include <string>
#include <string_view>
#include <tools/loader.h>
#include <tools/platform.h>

namespace {
    [[nodiscard]] auto get_required_modules() {
        // clang-format off
      return std::to_array<std::string>({
        // @note: @es3n1n: modules that we'll use in our code
        Loader::get_module_file_name("client"),
        Loader::get_module_file_name("engine2"),
        Loader::get_module_file_name("schemasystem"),
        Loader::get_module_file_name("tier0"),

        #if defined(DOTA2)
        // @note: @soufiw: latest modules that gets loaded in the main menu
        Loader::get_module_file_name("navsystem"),
        #elif defined(CS2)
        Loader::get_module_file_name("matchmaking"),
        #endif

        // modules that we'll dump (minus the ones listed above)
        Loader::get_module_file_name("animationsystem"),
        Loader::get_module_file_name("host"),
        Loader::get_module_file_name("materialsystem2"),
        Loader::get_module_file_name("meshsystem"),
        Loader::get_module_file_name("networksystem"),
        Loader::get_module_file_name("panorama"),
        Loader::get_module_file_name("particles"),
        Loader::get_module_file_name("pulse_system"),
        IF_WINDOWS(Loader::get_module_file_name("rendersystemdx11"),)
        Loader::get_module_file_name("resourcesystem"),
        Loader::get_module_file_name("scenefilecache"),
        Loader::get_module_file_name("scenesystem"),
        Loader::get_module_file_name("server"),
        Loader::get_module_file_name("soundsystem"),
        Loader::get_module_file_name("vphysics2"),
        Loader::get_module_file_name("worldrenderer")
    });
        // clang-format on
    }
} // namespace

namespace source2_gen {
    void Dump() try {
        // set up the allocator before anything else. we can't use any
        // allocating functions without it.
        if (Loader::load_module(LOADER_GET_MODULE_FILE_NAME("tier0")) == nullptr) {
            // don't use any allocating functions in here. That might include
            // dlerror() and FormatMessage(). Is there a friendly way to report
            // this to the user?
            std::fputs("could not load tier0. is " IF_LINUX("LD_LIBRARY_PATH") IF_WINDOWS("PATH") " set?", stderr);
            std::abort();
        }
        static_cast<void>(GetMemAlloc());

        const auto modules = get_required_modules();

        for (const auto& name : modules) {
            std::cout << "loading " << name << std::endl;
            if (Loader::load_module(name) == nullptr) {
                // cannot use any functions that use `new` because we've
                // overridden `new` in IMemAlloc.cpp and it relies on
                // libraries being loaded.
                std::cerr << std::format("Unable to load module {}, is {} set?", name, IF_WINDOWS("PATH") IF_LINUX("LD_LIBRARY_PATH")) << std::endl;
                // TODO: return instead of terminating
                std::exit(1);
            }
        }

        std::cout << std::format("{}: Starting", __FUNCTION__) << std::endl;

        // @note: @es3n1n: Capture interfaces
        //
        sdk::g_schema = CSchemaSystem::GetInstance();
        if (!sdk::g_schema)
            throw std::runtime_error(std::format("Unable to obtain Schema interface"));

        for (const auto& name : modules) {
            auto* handle = Loader::find_module_handle(name.data());
            assert(handle != nullptr && "we loaded modules at startup, where did they go?");

            if (const auto* pInstallSchemaBindings = Loader::find_module_symbol(handle, "InstallSchemaBindings")) {
                auto const InstallSchemaBindings = (std::uint8_t(*)(const char*, CSchemaSystem*))(pInstallSchemaBindings);
                if (!InstallSchemaBindings("SchemaSystem_001", sdk::g_schema)) {
                    std::cerr << std::format("Unable to install schema bindings in {}", name) << std::endl;
                    // TODO: return instead of terminating
                    std::exit(1);
                }
            } else {
                std::cout << std::format("No schemas in {}", name) << std::endl;
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
