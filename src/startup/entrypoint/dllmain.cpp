// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#include <Include.h>

namespace {
    void OnProcessAttach(const HMODULE h_module) {
        std::thread main_thread([h_module] { source2_gen::main(h_module); });

        if (main_thread.joinable())
            main_thread.detach();
    }

    void OnProcessDetach() {
        // Add any necessary clean-up code here
    }
} // namespace

BOOL APIENTRY DllMain(const HMODULE module, const DWORD reason, LPVOID reserved [[maybe_unused]]) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        OnProcessAttach(module);
        break;
    case DLL_PROCESS_DETACH:
        OnProcessDetach();
        break;
    default:
        break;
    }

    return TRUE;
}


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