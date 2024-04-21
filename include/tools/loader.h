// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.

#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <tools/platform.h>

#if TARGET_OS == WINDOWS
    #include "loader_windows.h"
#elif TARGET_OS == LINUX
    #include "loader_linux.h"
#endif

/// Prefer using Loader::get_module_file_name(). Only use these macros if you
/// cannot use C++ allocations, e.g. before tier0 has been loaded.
#if TARGET_OS == WINDOWS
    #define LOADER_GET_MODULE_FILE_NAME(expr) LOADER_WINDOWS_GET_MODULE_FILE_NAME(expr)
#elif TARGET_OS == LINUX
    #define LOADER_GET_MODULE_FILE_NAME(expr) LOADER_LINUX_GET_MODULE_FILE_NAME(expr)
#endif

/// Wrapping the functions in the `Platform` namespace causes compiler errors
/// even when they're unused in the code base (Fail fast). It also provides a
/// uniform interface for the caller.
namespace Loader {
#if TARGET_OS == WINDOWS
    namespace Platform = Windows;
#elif TARGET_OS == LINUX
    namespace Platform = Linux;
#endif

    using module_handle_t = Platform::module_handle_t;
    /// Guaranteed not to use allocating C++ functions
    using LoadModuleError = Platform::LoadModuleError;

    [[nodiscard]] inline auto get_module_file_name(std::string name) -> std::string {
        return Platform::get_module_file_name(std::move(name));
    }

    [[nodiscard]] inline auto find_module_handle(std::string_view name) -> module_handle_t {
        return Platform::find_module_handle(name);
    }

    [[nodiscard]] inline auto load_module(std::string_view name) -> std::expected<module_handle_t, LoadModuleError> {
        return Platform::load_module(name);
    }

    [[nodiscard]] inline auto find_module_symbol(module_handle_t handle, std::string_view name) -> void* {
        return Platform::find_module_symbol(handle, name);
    }
} // namespace Loader

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
