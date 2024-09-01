// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <concepts>
#include <format>
#include <ranges>
#include <string>
#include <string_view>
#include <tools/loader/loader.h>

namespace util {
    inline std::string PrettifyNum(int num) {
        static const auto fn =
            loader::find_module_symbol<const char* (*)(int)>(loader::find_module_handle(loader::get_module_file_name("tier0")), "V_PrettifyNum");

        if (fn.has_value()) {
            std::string_view res = (*fn)(num);
            if (!res.empty()) {
                return res.data();
            }
        }

        return std::to_string(num);
    }

    /// Useful for optional integers, e.g.
    /// ```cpp
    /// const std::optional<int> offset = try_get_offset();
    /// std::cout << std::format("offset: {}\n", offset.transform(to_hex_string).value_or("unknown"));
    /// ```
    inline std::string to_hex_string(int i) {
        return std::format("{:#x}", i);
    }

    [[nodiscard]] inline std::string EscapePath(std::string_view path) {
        std::string result(path);
        std::ranges::replace(result, ':', '_');
        return result;
    }
} // namespace util

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
