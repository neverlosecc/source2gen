// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include <cassert>
#include <string>
#include <string_view>
#include <windows.h>

// keep in sync with get_module_file_name()
#define LOADER_WINDOWS_GET_MODULE_FILE_NAME(name) name ".dll"

namespace Loader::Windows {
    using module_handle_t = HMODULE;

    // keep in sync with LOADER_WINDOWS_GET_MODULE_FILE_NAME
    [[nodiscard]] inline auto get_module_file_name(std::string name) -> std::string {
        return name.append(".dll");
    }

    [[nodiscard]] inline auto find_module_handle(std::string_view name) -> void* {
        return GetModuleHandleA(name.data());
    }

    [[nodiscard]] inline auto load_module(std::string_view name) -> void* {
        return LoadLibraryA(name.data());
    }

    [[nodiscard]] inline auto find_module_symbol(module_handle_t handle, std::string_view name) -> void* {
        assert(handle != nullptr);
        return GetProcAddress(handle, name.data());
    }
} // namespace Loader::Windows

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
