// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#include <Include.h>
#include <sdk/sdk.h>

#include "tools/console/console.h"

namespace {
    using namespace std::string_view_literals;

    // clang-format off
    constexpr std::initializer_list<std::string_view> kRequiredGameModules = {
        // @note: @es3n1n: modules that we'll use in our code
        "client.dll"sv,
        "engine2.dll"sv,
        "schemasystem.dll"sv,
        "tier0.dll"sv,
        
        // @note: @soufiw: latest modules that gets loaded in the main menu
        "navsystem.dll"sv,
        #if defined(CS2)
        "matchmaking.dll"sv,
        #endif
    };
    // clang-format on

    std::atomic_bool is_finished = false;
} // namespace

namespace source2_gen {
    void Setup() try {
        // @note: @es3n1n: Waiting for game init
        //
        const auto required_modules_present = []() [[msvc::forceinline]] -> bool {
            bool result = true;

            for (auto& name : kRequiredGameModules)
                result &= static_cast<bool>(GetModuleHandleA(name.data()));

            return result;
        };

        while (!required_modules_present())
            sleep_for(std::chrono::seconds(5));

        std::cout << std::format("{}: Starting", __FUNCTION__) << std::endl;

        // @note: @es3n1n: Capture interfaces
        //
        sdk::g_schema = CSchemaSystem::GetInstance();
        if (!sdk::g_schema)
            throw std::runtime_error(std::format("Unable to obtain Schema interface"));

        while(!sdk::g_schema->SchemaSystemIsReady())
            sleep_for(std::chrono::seconds(5));

        // @note: @es3n1n: Obtaining type scopes and generating sdk
        //
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

        // @note: @es3n1n: We are done here
        //
        is_finished = true;
    } catch (std::runtime_error& err) {
        std::cout << std::format("{} :: ERROR :: {}", __FUNCTION__, err.what()) << std::endl;
        is_finished = true;
    }

    void WINAPI main(const HMODULE module) {
        auto console = std::make_unique<DebugConsole>();
        console->start(kConsoleTitleMessage.data());

        std::jthread setup_thread(&Setup);

        while (!is_finished) {
            console->update();
            sleep_for(std::chrono::milliseconds(1));
        }

        std::cout << std::format("Successfuly dumped Source 2 SDK, now you can safely close this console.") << std::endl;
        std::cout << kPoweredByMessage << std::endl;

        console->stop();
        console.reset();

        FreeLibraryAndExitThread(module, EXIT_SUCCESS);
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

