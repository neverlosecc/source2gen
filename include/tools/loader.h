// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.

#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <tools/platform.h>

#pragma warning(disable:4715)

#if TARGET_OS == WINDOWS
    #include "loader_windows.h"
#elif TARGET_OS == LINUX
    #include "loader_linux.h"
#endif

/// Prefer using loader::get_module_file_name(). Only use these macros if you
/// cannot use C++ allocations, e.g. before tier0 has been loaded.
#if TARGET_OS == WINDOWS
    #define LOADER_GET_MODULE_FILE_NAME(expr) LOADER_WINDOWS_GET_MODULE_FILE_NAME(expr)
#elif TARGET_OS == LINUX
    #define LOADER_GET_MODULE_FILE_NAME(expr) LOADER_LINUX_GET_MODULE_FILE_NAME(expr)
#endif

/// Wrapping the functions in the `Platform` namespace causes compiler errors
/// even when they're unused in the code base (Fail fast). It also provides a
/// uniform interface for the caller.
namespace loader {
#if TARGET_OS == WINDOWS
    namespace platform = windows;
#elif TARGET_OS == LINUX
    namespace platform = linux;
#endif

    using module_handle_t = platform::module_handle_t;
    /// Guaranteed not to use allocating C++ functions
    using LoadModuleError = platform::LoadModuleError;

    [[nodiscard]] inline auto get_module_file_name(std::string name) -> std::string {
        return platform::get_module_file_name(std::move(name));
    }

    [[nodiscard]] inline auto find_module_handle(std::string_view name) -> module_handle_t {
        return platform::find_module_handle(name);
    }

    [[nodiscard]] inline auto load_module(std::string_view name) -> std::expected<module_handle_t, LoadModuleError> {
        return platform::load_module(name);
    }

    template <typename Ty = void*>
    [[nodiscard]] inline auto find_module_symbol(module_handle_t handle, std::string_view name) -> std::expected<Ty, LoadModuleError> {
#if TARGET_OS == WINDOWS
        auto sym = platform::find_module_symbol(handle, name);
        if (sym.has_value()) {
            return reinterpret_cast<Ty>(sym.value());
        }

        return std::unexpected(sym.error());
#elif TARGET_OS == LINUX
        auto result = platform::find_module_symbol(handle, name);
        if (result.has_value()) {
            return reinterpret_cast<Ty>(result.value());
        }

        return std::unexpected(result.error());
#else
    #error unsupported operating system
#endif
    }
} // namespace loader

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
