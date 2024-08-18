// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include <Include.h>

namespace {
    int bootstrap() {
        int exit_code = 1;

        if (source2_gen::Dump()) {
            std::cout << std::format("Successfully dumped Source 2 SDK, now you can safely close this console.") << std::endl;
            std::cout << kPoweredByMessage << std::endl;
            exit_code = 0;
        }

        /// Errors would be logged in the `source2_gen::Dump` itself
        /// We don't want to call getch on linux as the program would be started within a terminal anyway.
#if TARGET_OS == WINDOWS
        (void)std::getchar();
#endif
        return exit_code;
    }
} // namespace

#if TARGET_OS == WINDOWS
BOOL __stdcall DllMain(void*, int call_reason, void*) {
    if (call_reason != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    return static_cast<BOOL>(bootstrap());
}
#elif TARGET_OS == LINUX
void __attribute__((constructor)) DllMain(void) {
    bootstrap();
}
#endif

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
