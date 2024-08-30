// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>

namespace codegen {
    constexpr char kTabSym = '\t';
    constexpr std::size_t kTabsPerBlock = 1; // @note: @es3n1n: how many \t characters shall we place per each block

    struct Prop {
        std::string type_name{};
        std::string name{};
        std::optional<int> bitfield_size{};
    };

    struct Padding {
        struct Bytes {
            std::size_t value{};
        };
        struct Bits {
            std::size_t value{};
        };

        /**
         * May be @ref std::nullopt in unions
         */
        std::optional<std::ptrdiff_t> pad_offset{};
        std::variant<Bytes, Bits> size;
        // TOOD: remove?
        bool is_private_field{false};
        // TOOD: inconsistent. some functions have this as a separate parameter
        bool move_cursor_to_next_line{true};
    };

    struct IncludeOptions {
        /**
         * Treat the included file as a local file, e.g. in C++ use quoatation marks for includes.
         */
        bool local{false};

        /**
         * Treat the included file as a system file, e.g. in C++ don't add a file extension.
         */
        bool system{false};
    };

    struct IGenerator {
        using self_ref = IGenerator&;

        /**
         * The fully qualified type name of the (unsigned) integer type with the given width.
         */
        virtual std::string get_uint(std::size_t bits_count) const = 0;

        /**
         * Maps the following Source2 type names to the fully qualified type name in the target language.
         *
         * - float32
         * - float64
         * - int8
         * - int16
         * - int32
         * - int64
         * - uint8
         * - uint16
         * - uint32
         * - uint64
         *
         * @return @ref std::nullopt if @p source_name is not listed above
         */
        virtual std::optional<std::string> find_built_in(std::string_view source_name) const = 0;

        /**
         * @return Extension to be used for generated files. Only 1 extension is
         * supported per language.
         */
        [[nodiscard]]
        virtual std::string get_file_extension() const = 0;

        /**
         * Can be used by implementations to generate language-specific
         * preambles, such as header guards in C.
         */
        virtual self_ref preamble() = 0;

        virtual self_ref include(std::string_view module_or_file_name, IncludeOptions options) = 0;

        virtual self_ref next_line() = 0;

        virtual self_ref access_modifier(const std::string& modifier) = 0;

        virtual self_ref begin_class(const std::string& class_name, const std::string& access_modifier = "public") = 0;

        virtual self_ref begin_class_with_base_type(const std::string& class_name, const std::string& base_type,
                                                    const std::string& access_modifier = "public") = 0;

        virtual self_ref end_class() = 0;

        virtual self_ref begin_struct(std::string_view name, const std::string& access_modifier = "public") = 0;

        virtual self_ref begin_struct_with_base_type(const std::string& name, const std::string& base_type,
                                                     const std::string& access_modifier = "public") = 0;

        virtual self_ref end_struct() = 0;

        virtual self_ref begin_namespace(const std::string& namespace_name) = 0;

        virtual self_ref end_namespace() = 0;

        virtual self_ref begin_enum_class(const std::string& enum_name, const std::string& base_typename = "") = 0;

        virtual self_ref end_enum_class() = 0;

        virtual self_ref enum_item(const std::string& name, std::uint64_t value) = 0;

        virtual self_ref begin_function(const std::string& prefix, const std::string& type_name, const std::string& func_name,
                                        bool increment_tabs_count = true, bool move_cursor_to_next_line = true) = 0;

        virtual self_ref end_function(bool decrement_tabs_count, bool move_cursor_to_next_line = true) = 0;

        virtual self_ref return_value(const std::string& value, bool move_cursor_to_next_line = true) = 0;

        virtual self_ref static_field_getter(const std::string& type_name, const std::string& prop_name, const std::string& mod_name,
                                             const std::string& decl_class, const std::size_t index) = 0;

        virtual self_ref static_assert_size(std::string_view type_name, int expected_size, const bool move_cursor_to_next_line = true) = 0;

        virtual self_ref static_assert_offset(std::string_view class_name, std::string_view prop_name, int expected_offset,
                                              const bool move_cursor_to_next_line = true) = 0;

        virtual self_ref comment(const std::string& text, bool move_cursor_to_next_line = true) = 0;

        /// Not to be used for inline comments
        virtual self_ref begin_multi_line_comment(const bool move_cursor_to_next_line = true) = 0;

        /// Not to be used for inline comments
        virtual self_ref end_multi_line_comment(const bool move_cursor_to_next_line = true) = 0;

        virtual self_ref prop(Prop prop, bool move_cursor_to_next_line = true) = 0;

        virtual self_ref forward_declaration(const std::string& text) = 0;

        virtual self_ref struct_padding(Padding options) = 0;

        virtual self_ref begin_bitfield_block() = 0;

        virtual self_ref end_bitfield_block(bool move_cursor_to_next_line) = 0;

        virtual self_ref restore_tabs_count() = 0;

        virtual self_ref reset_tabs_count() = 0;

        virtual std::string str() const = 0;

        virtual ~IGenerator() = default;
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
