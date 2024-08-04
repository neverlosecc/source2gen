// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <cstddef>
#include <format>
#include <sdk/interfaces/client/game/datamap_t.h>
#include <string>
#include <string_view>
#include <vector>

namespace field_parser {
    class field_info_t {
    public:
        std::string m_type; // var type
        fieldtype_t m_field_type = fieldtype_t::FIELD_UNUSED; // var type
        std::string m_name; // var name

        // array sizes, for example {13, 37} for multi demensional array "[13][37]"
        std::vector<std::size_t> m_array_sizes = {};

        std::size_t m_bitfield_size = 0ull; // bitfield size, set to 0 if var isn't a bitfield

    public:
        [[nodiscard]] bool is_bitfield() const noexcept {
            return static_cast<bool>(m_bitfield_size);
        }

        [[nodiscard]] bool is_array() const noexcept {
            return !m_array_sizes.empty();
        }

    public:
        [[nodiscard]] std::size_t total_array_size() const {
            std::size_t result = 0ull;

            for (auto size : m_array_sizes) {
                if (!result) {
                    result = size;
                    continue;
                }

                result *= size;
            }

            return result;
        }

    public:
        [[nodiscard]] std::string formatted_array_sizes() const {
            std::string result;

            for (std::size_t size : m_array_sizes)
                result += std::format("[{}]", size);

            return result;
        }

        [[nodiscard]] std::string formatted_name() const {
            if (is_bitfield())
                return std::format("{}: {}", m_name, m_bitfield_size);

            if (is_array())
                return std::format("{}{}", m_name, formatted_array_sizes());

            return m_name;
        }
    };

    /// @return @ref std::nullopt if type_name is not a built-in type
    [[nodiscard]]
    std::optional<std::string_view> type_name_to_cpp(std::string_view type_name);

    field_info_t parse(const std::string& type_name, const std::string& name, const std::vector<std::size_t>& array_sizes);
    field_info_t parse(const fieldtype_t& type_name, const std::string& name, const std::size_t& array_sizes = 1);
} // namespace field_parser

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
