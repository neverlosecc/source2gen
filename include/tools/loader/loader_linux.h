// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include <algorithm>
#include <cassert>
#include <cstring>
#include <dlfcn.h>
#include <expected>
#include <string>
#include <string_view>

#include "loader_shared.h"

// keep in sync with get_module_file_name()
#define LOADER_LINUX_GET_MODULE_FILE_NAME(name) "lib" name ".so"

namespace loader::linux {
    using module_handle_t = void*;

    // keep in sync with LOADER_LINUX_GET_MODULE_FILE_NAME
    [[nodiscard]] inline auto get_module_file_name(std::string name) -> std::string {
        return name.insert(0, "lib").append(".so");
    }

    [[nodiscard]] inline auto find_module_handle(std::string_view name) -> module_handle_t {
        return dlopen(name.data(), RTLD_LAZY | RTLD_NOLOAD);
    }

    [[nodiscard]] inline auto load_module(std::string_view name) -> std::expected<module_handle_t, ModuleLookupError> {
        if (auto* const handle = dlopen(name.data(), RTLD_LAZY)) {
            return handle;
        }
        return std::unexpected(ModuleLookupError::from_string(dlerror()));
    }

    template <typename Ty>
    [[nodiscard]] inline auto find_module_symbol(module_handle_t handle, std::string_view name) -> std::expected<Ty, ModuleLookupError> {
        assert(handle != nullptr && "If you need RTLD_DEFAULT, write a new function to avoid magic values. Most of the time when handle=nullptr, a "
                                    "developer made a mistake and we want to catch that.");
        auto* const result = dlsym(handle, name.data());
        if (result == nullptr) {
            return std::unexpected(ModuleLookupError::from_string(dlerror()));
        }
        return reinterpret_cast<Ty>(result);
    }
} // namespace loader::linux

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
