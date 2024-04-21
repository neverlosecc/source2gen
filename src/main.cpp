// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include <Include.h>

int main(int argc, char* argv[]) {
    if (source2_gen::Dump()) {
        std::cout << std::format("Successfully dumped Source 2 SDK, now you can safely close this console.") << std::endl;
        std::cout << kPoweredByMessage << std::endl;
    } else {
        // error was logged by Dump()
        return 1;
    }
}

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
