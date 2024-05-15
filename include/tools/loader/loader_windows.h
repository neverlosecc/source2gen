// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>
#include <windows.h>

#include "loader_shared.h"

// keep in sync with get_module_file_name()
#define LOADER_WINDOWS_GET_MODULE_FILE_NAME(name) name ".dll"

namespace loader::windows {
    using module_handle_t = HMODULE;

    namespace detail {
        inline LoadModuleError win32_error(DWORD error = GetLastError()) {
            LPSTR pBuffer = nullptr;

            const auto size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error,
                                             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&pBuffer), 0, nullptr);

            const auto result = LoadModuleError::from_string(pBuffer);

            LocalFree(pBuffer);
            return result;
        }
    } // namespace detail

    // keep in sync with LOADER_WINDOWS_GET_MODULE_FILE_NAME
    [[nodiscard]] inline auto get_module_file_name(std::string name) -> std::string {
        return name.append(".dll");
    }

    [[nodiscard]] inline auto find_module_handle(std::string_view name) -> module_handle_t {
        return GetModuleHandleA(name.data());
    }

    [[nodiscard]] inline auto load_module(std::string_view name) -> std::expected<module_handle_t, LoadModuleError> {
        auto result = LoadLibraryA(name.data());
        if (result == reinterpret_cast<HINSTANCE>(0)) {
            return std::unexpected(detail::win32_error());
        }

        return result;
    }

    template <typename Ty>
    [[nodiscard]] inline auto find_module_symbol(module_handle_t handle, std::string_view name) -> std::expected<Ty, LoadModuleError> {
        assert(handle != nullptr);
        if (auto const h_module = GetProcAddress(handle, name.data())) {
            return reinterpret_cast<Ty>(h_module);
        }

        return std::unexpected(detail::win32_error());
    }
} // namespace loader::windows

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
