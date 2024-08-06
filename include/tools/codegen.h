// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include <cassert>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>

#include "tools/fnv.h"

namespace codegen {
    constexpr char kTabSym = '\t';
    constexpr std::size_t kTabsPerBlock = 1; // @note: @es3n1n: how many \t characters shall we place per each block
    constexpr std::array kBlacklistedCharacters = {':', ';', '\\', '/'};

    struct generator_t {
        using self_ref = std::add_lvalue_reference_t<generator_t>;

    public:
        constexpr generator_t() = default;
        ~generator_t() = default;
        generator_t& operator=(const generator_t& v) {
            if (this != &v) {
                this->_stream = std::stringstream(v._stream.str());
                /// \todo @es3n1n: Such stats counters should be moved to their own structure
                this->_tabs_count = v._tabs_count;
                this->_tabs_count_backup = v._tabs_count_backup;
                this->_unions_count = v._unions_count;
                this->_pads_count = v._pads_count;
                this->_forward_decls = v._forward_decls;
            }
            return *this;
        }

    public:
        self_ref pragma(const std::string& val) {
            return push_line(std::format("#pragma {}", val));
        }

        self_ref include(const std::string& item) {
            return push_line(std::format("#include {}", item));
        }

        self_ref disable_warnings(const std::string& codes) {
            return push_warning().pragma(std::format("warning(disable: {})", codes));
        }

        self_ref push_warning() {
            return pragma("warning(push)");
        }

        self_ref pop_warning() {
            return pragma("warning(pop)");
        }

        self_ref next_line() {
            return push_line("");
        }

        // @todo: @es3n1n: `self_ref prev_line()`

        self_ref access_modifier(const std::string& modifier) {
            dec_tabs_count(1);
            push_line(std::format("{}:", modifier));
            restore_tabs_count();

            return *this;
        }

        self_ref begin_block(const std::string& text, const std::string& access_modifier = "", const bool increment_tabs_count = true,
                             const bool move_cursor_to_next_line = true) {
            push_line(text, move_cursor_to_next_line);

            // @note: @es3n1n: we should reset tabs count if we aren't moving cursor to
            // the next line
            const auto backup_tabs_count = _tabs_count;
            if (!move_cursor_to_next_line)
                _tabs_count = 0;

            push_line("{", move_cursor_to_next_line);

            if (!access_modifier.empty())
                push_line(std::format("{}:", access_modifier), move_cursor_to_next_line);

            // @note: @es3n1n: restore tabs count
            if (!move_cursor_to_next_line)
                _tabs_count = backup_tabs_count;

            if (increment_tabs_count)
                inc_tabs_count(kTabsPerBlock);

            return *this;
        }

        self_ref end_block(const bool decrement_tabs_count = true, const bool move_cursor_to_next_line = true) {
            if (decrement_tabs_count)
                dec_tabs_count(kTabsPerBlock);

            push_line("};");
            if (move_cursor_to_next_line)
                next_line();

            return *this;
        }

        self_ref begin_class(const std::string& class_name, const std::string& access_modifier = "public") {
            return begin_block(std::format("class {}", escape_name(class_name)), access_modifier);
        }

        self_ref begin_class_with_base_type(const std::string& class_name, const std::string& base_type, const std::string& access_modifier = "public") {
            if (base_type.empty())
                return begin_class(class_name, access_modifier);
            else
                return begin_block(std::format("class {} : public {}", escape_name(class_name), base_type), access_modifier);
        }

        self_ref end_class() {
            return end_block();
        }

        self_ref begin_namespace(const std::string_view namespace_name) {
            return begin_block(std::format("namespace {}", namespace_name));
        }

        self_ref end_namespace() {
            return end_block();
        }

        self_ref begin_enum_class(const std::string& enum_name, const std::string& base_typename = "") {
            return begin_block(std::format("enum class {}{}", escape_name(enum_name), base_typename.empty() ? base_typename : (" : " + base_typename)));
        }

        self_ref end_enum_class() {
            return end_block();
        }

        template <typename T>
        self_ref enum_item(const std::string& name, T value) {
            return push_line(std::vformat(sizeof(T) >= 2 ? "{} = {:#x}," : "{} = {},", std::make_format_args(name, value)));
        }

        self_ref begin_struct(const std::string& name, const std::string& access_modifier = "public") {
            return begin_block(std::format("struct {}", escape_name(name)), access_modifier);
        }

        self_ref begin_struct_with_base_type(const std::string& name, const std::string& base_type, const std::string& access_modifier = "public") {
            if (base_type.empty())
                return begin_struct(std::cref(name), access_modifier);

            return begin_block(std::format("struct {} : public {}", escape_name(name), base_type), access_modifier);
        }

        self_ref end_struct() {
            return end_block();
        }

        // @todo: @es3n1n: add func params
        self_ref begin_function(const std::string& prefix, const std::string& type_name, const std::string& func_name,
                                const bool increment_tabs_count = true, const bool move_cursor_to_next_line = true) {
            return begin_block(std::format("{}{} {}()", prefix, type_name, escape_name(func_name)), "", increment_tabs_count, move_cursor_to_next_line);
        }

        self_ref end_function(const bool decrement_tabs_count = true, const bool move_cursor_to_next_line = true) {
            return end_block(decrement_tabs_count, move_cursor_to_next_line);
        }

        self_ref return_value(const std::string& value, const bool move_cursor_to_next_line = true) {
            return push_line(std::format("return {};", value), move_cursor_to_next_line);
        }

        self_ref static_field_getter(const std::string& type_name, const std::string& prop_name, const std::string& mod_name,
                                     const std::string& decl_class, const std::size_t index) {
            begin_function("static ", type_name, std::format("&Get_{}", prop_name), false, false);

            // @note: @es3n1n: reset tabs count temporary
            //
            const auto backup_tabs_count = _tabs_count;
            _tabs_count = 0;

            const auto getter = std::format(
                R"(*reinterpret_cast<{}*>(interfaces::g_schema->FindTypeScopeForModule("{}")->FindDeclaredClass("{}")->GetStaticFields()[{}]->m_pInstance))",
                type_name, mod_name, decl_class, index);
            return_value(getter, false);
            end_function(false, false);

            // @note: @es3n1n: restore tabs count
            //
            _tabs_count = backup_tabs_count;
            return *this;
        }

        self_ref static_assert_size(std::string_view type_name, int expected_size, const bool move_cursor_to_next_line = true) {
            assert(expected_size > 0);

            return push_line(std::format("static_assert(sizeof({}) == {:#x});", type_name, expected_size));
        }

        self_ref static_assert_offset(std::string_view class_name, std::string_view prop_name, int expected_offset,
                                      const bool move_cursor_to_next_line = true) {
            assert(expected_offset >= 0);

            return push_line(std::format("static_assert(offsetof({}, {}) == {:#x});", class_name, prop_name, expected_offset));
        }

        self_ref comment(const std::string& text, const bool move_cursor_to_next_line = true) {
            return push_line(std::format("// {}", text), move_cursor_to_next_line);
        }

        self_ref prop(const std::string& type_name, const std::string& name, bool move_cursor_to_next_line = true) {
            const auto line = move_cursor_to_next_line ? std::format("{} {};", type_name, name) : std::format("{} {}; ", type_name, name);
            return push_line(line, move_cursor_to_next_line);
        }

        self_ref forward_declaration(const std::string& type_name) {
            // @note: @es3n1n: forward decl only once
            const auto fwd_decl_hash = fnv32::hash_runtime(type_name.data());
            if (_forward_decls.contains(fwd_decl_hash))
                return *this;

            _forward_decls.insert(fwd_decl_hash);

            // @fixme: split method to class_forward_declaration & struct_forward_declaration
            // one for `struct uwu_t` and the other one for `class c_uwu`
            return push_line(std::format("struct {};", type_name));
        }

        self_ref struct_padding(const std::optional<std::ptrdiff_t> pad_offset, const std::size_t padding_size, const bool move_cursor_to_next_line = true,
                                const bool is_private_field = false, const int bitfield_size = 0) {
            assert(bitfield_size >= 0);

            const auto bytes = (bitfield_size == 0) ? padding_size : bitfield_size / 8;
            const auto remaining_bits = bitfield_size % 8;

            // @note: @es3n1n: mark private fields as maybe_unused to silence -Wunused-private-field
            std::string type_name = "std::uint8_t";
            if (is_private_field)
                type_name = "[[maybe_unused]] " + type_name;

            auto pad_name = pad_offset.has_value() ? std::format("pad_0x{:04x}", pad_offset.value()) : std::format("pad_{:d}", _pads_count++);

            if (bytes != 0) {
                prop(type_name, std::format("{}[{:#x}]", pad_name, bytes), move_cursor_to_next_line);
            }

            if (remaining_bits != 0) {
                auto remainder_pad_name =
                    pad_offset.has_value() ? std::format("pad_0x{:04x}", pad_offset.value() + bytes) : std::format("pad_{:d}", _pads_count++);
                prop(type_name, std::format("{}: {}", remainder_pad_name, remaining_bits), move_cursor_to_next_line);
            }

            return *this;
        }

        self_ref begin_union(std::string name = "") {
            if (name.empty())
                name = std::format("_union_{}", _unions_count++);
            return begin_block(std::format("union {}", name));
        }

        self_ref end_union(const bool move_cursor_to_next_line = true) {
            dec_tabs_count(1);
            return push_line(move_cursor_to_next_line ? "};" : "}; ", move_cursor_to_next_line);
        }

        self_ref begin_bitfield_block() {
            return comment("start of bitfield block");
        }

        self_ref end_bitfield_block(const bool move_cursor_to_next_line = true) {
            return comment(std::format("end of bitfield block{}", move_cursor_to_next_line ? "" : " "), move_cursor_to_next_line);
        }

    public:
        [[nodiscard]] std::string str() const {
            return _stream.str();
        }

    private:
        self_ref push_line(const std::string& line, bool move_cursor_to_next_line = true) {
            for (std::size_t i = 0; i < _tabs_count; i++)
                _stream << kTabSym;
            _stream << line;
            if (move_cursor_to_next_line)
                _stream << std::endl;
            return *this;
        }

        static std::string escape_name(const std::string& name) {
            std::string result;
            result.resize(name.size());

            for (std::size_t i = 0; i < name.size(); i++)
                result[i] = std::ranges::find(kBlacklistedCharacters, name[i]) == std::end(kBlacklistedCharacters) ? name[i] : '_';

            return result;
        }

    public:
        self_ref inc_tabs_count(const std::size_t count = 1) {
            _tabs_count_backup = _tabs_count;
            _tabs_count += count;
            return *this;
        }

        self_ref dec_tabs_count(const std::size_t count = 1) {
            _tabs_count_backup = _tabs_count;
            if (_tabs_count)
                _tabs_count -= count;
            return *this;
        }

        self_ref restore_tabs_count() {
            _tabs_count = _tabs_count_backup;
            return *this;
        }

        self_ref reset_tabs_count() {
            _tabs_count_backup = _tabs_count;
            _tabs_count = 0;
            return *this;
        }

    private:
        std::stringstream _stream = {};
        std::size_t _tabs_count = 0, _tabs_count_backup = 0;
        std::size_t _unions_count = 0;
        std::size_t _pads_count = 0;
        std::set<fnv32::hash> _forward_decls = {};
    };

    inline generator_t get() {
        return generator_t{};
    }
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
