// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include <cstdint>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>

#include "tools/fnv.h"

namespace codegen {
    constexpr char kTabSym = '\t';
    constexpr std::size_t kTabsPerBlock = 1; // @note: @es3n1n: how many \t characters shall we place per each block
    constexpr std::array kBlacklistedCharacters = {':', ';', '\\', '/'};

    // @note: @es3n1n: a list of possible integral types for bitfields (would be used in `guess_bitfield_type`)
    //
    // clang-format off
    constexpr auto kBitfieldIntegralTypes = std::to_array<std::pair<std::size_t, std::string_view>>({
        {8, "uint8_t"},
        {16, "uint16_t"},
        {32, "uint32_t"},
        {64, "uint64_t"},

        // @todo: @es3n1n: define uint128_t/uint256_t/... as custom structs in the very beginning of the file
        {128, "uint128_t"},
        {256, "uint256_t"},
        {512, "uint512_t"},
    });
    // clang-format on

    inline std::string guess_bitfield_type(const std::size_t bits_count) {
        for (auto p : kBitfieldIntegralTypes) {
            if (bits_count > p.first)
                continue;

            return p.second.data();
        }

        throw std::runtime_error(std::format("{} : Unable to guess bitfield type with size {}", __FUNCTION__, bits_count));
    }

    struct IGenerator {
        using self_ref = IGenerator&;

        virtual self_ref pragma(const std::string& val) = 0;

        virtual self_ref include(const std::string& item) = 0;

        virtual self_ref next_line() = 0;

        virtual self_ref access_modifier(const std::string& modifier) = 0;

        virtual self_ref begin_class(const std::string& class_name, const std::string& access_modifier = "public") = 0;

        virtual self_ref begin_class_with_base_type(const std::string& class_name, const std::string& base_type,
                                                    const std::string& access_modifier = "public") = 0;

        virtual self_ref end_class() = 0;

        virtual self_ref begin_struct(const std::string& name, const std::string& access_modifier = "public") = 0;

        virtual self_ref begin_struct_with_base_type(const std::string& name, const std::string& base_type,
                                                     const std::string& access_modifier = "public") = 0;

        virtual self_ref end_struct() = 0;

        virtual self_ref begin_namespace(const std::string& namespace_name) = 0;

        virtual self_ref end_namespace() = 0;

        virtual self_ref begin_enum_class(const std::string& enum_name, const std::string& base_typename = "") = 0;

        virtual self_ref end_enum_class() = 0;

        virtual self_ref enum_item(const std::string& name, std::int64_t value) = 0;

        virtual self_ref begin_function(const std::string& prefix, const std::string& type_name, const std::string& func_name,
                                        bool increment_tabs_count = true, bool move_cursor_to_next_line = true) = 0;

        virtual self_ref end_function(bool decrement_tabs_count, bool move_cursor_to_next_line = true) = 0;

        virtual self_ref return_value(const std::string& value, bool move_cursor_to_next_line = true) = 0;

        virtual self_ref static_field_getter(const std::string& type_name, const std::string& prop_name, const std::string& mod_name,
                                             const std::string& decl_class, const std::size_t index) = 0;

        virtual self_ref comment(const std::string& text, bool move_cursor_to_next_line = true) = 0;

        virtual self_ref prop(const std::string& type_name, const std::string& name, bool move_cursor_to_next_line = true) = 0;

        virtual self_ref forward_declaration(const std::string& text) = 0;

        // TOOD: add an "options" struct and document optionality
        virtual self_ref struct_padding(const std::optional<std::ptrdiff_t> pad_offset, const std::size_t padding_size,
                                        const bool move_cursor_to_next_line = true, const bool is_private_field = false,
                                        const std::size_t bitfield_size = 0ull) = 0;

        virtual self_ref begin_bitfield_block() = 0;

        virtual self_ref end_bitfield_block(bool move_cursor_to_next_line) = 0;

        virtual self_ref restore_tabs_count() = 0;

        virtual self_ref reset_tabs_count() = 0;
    };

} // namespace codegen

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
