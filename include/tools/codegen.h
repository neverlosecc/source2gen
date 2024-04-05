// Copyright (C) 2023 neverlosecc
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

    struct generator_t {
        using self_ref = std::add_lvalue_reference_t<generator_t>;

    public:
        constexpr generator_t() = default;
        constexpr ~generator_t() = default;
        constexpr self_ref operator=(self_ref v) {
            return v;
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

        self_ref begin_block(const std::string& text, const std::string& access_modifier = "", bool increment_tabs_count = true,
                             bool move_cursor_to_next_line = true) {
            push_line(text, move_cursor_to_next_line);

            // @note: @es3n1n: we should reset tabs count if we aren't moving cursor to
            // the next line
            auto backup_tabs_count = _tabs_count;
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

        self_ref end_block(bool decrement_tabs_count = true, bool move_cursor_to_next_line = true) {
            if (decrement_tabs_count)
                dec_tabs_count(kTabsPerBlock);

            push_line("};");
            if (move_cursor_to_next_line)
                next_line();

            return *this;
        }

        self_ref begin_class(const std::string& class_name, const std::string access_modifier = "public") {
            return begin_block(std::format("class {}", class_name), access_modifier);
        }

        self_ref begin_class_with_base_type(const std::string& class_name, const std::string& base_type, const std::string access_modifier = "public") {
            if (base_type.empty())
                return begin_class(std::cref(class_name), access_modifier);

            return begin_block(std::format("class {} : public {}", class_name, base_type), access_modifier);
        }

        self_ref end_class() {
            return end_block();
        }

        self_ref begin_namespace(const std::string& namespace_name) {
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
            return push_line(std::vformat(sizeof(T) >= 4 ? "{} = {:#x}," : "{} = {},", std::make_format_args(name, value)));
        }

        self_ref begin_struct(const std::string& name, const std::string access_modifier = "public") {
            return begin_block(std::format("struct {}", escape_name(name)), access_modifier);
        }

        self_ref begin_struct_with_base_type(const std::string& name, const std::string& base_type, const std::string access_modifier = "public") {
            if (base_type.empty())
                return begin_struct(std::cref(name), access_modifier);

            return begin_block(std::format("struct {} : public {}", escape_name(name), base_type), access_modifier);
        }

        self_ref end_struct() {
            return end_block();
        }

        // @todo: @es3n1n: add func params
        self_ref begin_function(const std::string& prefix, const std::string& type_name, const std::string& func_name, bool increment_tabs_count = true,
                                bool move_cursor_to_next_line = true) {
            return begin_block(std::format("{}{} {}()", prefix, type_name, escape_name(func_name)), "", increment_tabs_count, move_cursor_to_next_line);
        }

        self_ref end_function(bool decrement_tabs_count = true, bool move_cursor_to_next_line = true) {
            return end_block(decrement_tabs_count, move_cursor_to_next_line);
        }

        self_ref return_value(const std::string& value, bool move_cursor_to_next_line = true) {
            return push_line(std::format("return {};", value), move_cursor_to_next_line);
        }

        self_ref static_field_getter(const std::string& type_name, const std::string& prop_name, const std::string& mod_name,
                                     const std::string& decl_class, const std::size_t index) {
            begin_function("static ", type_name, std::format("&Get_{}", prop_name), false, false);

            // @note: @es3n1n: reset tabs count temporary
            //
            auto backup_tabs_count = _tabs_count;
            _tabs_count = 0;

            auto getter = std::format(
                "*reinterpret_cast<{}*>(interfaces::g_schema->FindTypeScopeForModule(\"{}\")->FindDeclaredClass(\"{}\")->m_static_fields[{}]->m_instance)",
                type_name, mod_name, decl_class, index);
            return_value(getter, false);
            end_function(false, false);

            // @note: @es3n1n: restore tabs count
            //
            _tabs_count = backup_tabs_count;
            return *this;
        }

        self_ref comment(const std::string& text, const bool move_cursor_to_next_line = true) {
            return push_line(std::format("// {}", text), move_cursor_to_next_line);
        }

        self_ref prop(const std::string& type_name, const std::string& name, bool move_cursor_to_next_line = true) {
            const auto line = move_cursor_to_next_line ? std::format("{} {};", type_name, name) : std::format("{} {}; ", type_name, name);
            return push_line(line, move_cursor_to_next_line);
        }

        self_ref forward_declaration(const std::string& text) {
            // @note: @es3n1n: forward decl only once
            const auto fwd_decl_hash = fnv32::hash_runtime(text.data());
            if (_forward_decls.contains(fwd_decl_hash))
                return *this;

            _forward_decls.insert(fwd_decl_hash);

            // @fixme: split method to class_forward_declaration & struct_forward_declaration
            // one for `struct uwu_t` and the other one for `class c_uwu`
            return push_line(std::format("struct {};", text));
        }

        self_ref struct_padding(const std::optional<std::ptrdiff_t> pad_offset, const std::size_t padding_size, const bool move_cursor_to_next_line = true,
                                const bool is_private_field = false, const std::size_t bitfield_size = 0ull) {
            // @note: @es3n1n: mark private fields as maybe_unused to silence -Wunused-private-field
            std::string type_name = bitfield_size ? guess_bitfield_type(bitfield_size) : "uint8_t";
            if (is_private_field)
                type_name = "[[maybe_unused]] " + type_name;

            auto pad_name = pad_offset.has_value() ? std::format("__pad{:04x}", pad_offset.value()) : std::format("__pad{:d}", _pads_count++);
            if (!bitfield_size)
                pad_name = pad_name + std::format("[{:#x}]", padding_size);

            return prop(type_name, bitfield_size ? std::format("{}: {}", pad_name, bitfield_size) : pad_name, move_cursor_to_next_line);
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
            return begin_struct("", "");
        }

        self_ref end_bitfield_block(const bool move_cursor_to_next_line = true) {
            dec_tabs_count(1);
            return push_line(move_cursor_to_next_line ? "};" : "}; ", move_cursor_to_next_line);
        }

    public:
        [[nodiscard]] std::string str() {
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

        std::string escape_name(const std::string& name) {
            std::string result;
            result.resize(name.size());

            for (std::size_t i = 0; i < name.size(); i++)
                result[i] =
                    std::find(kBlacklistedCharacters.begin(), kBlacklistedCharacters.end(), name[i]) == std::end(kBlacklistedCharacters) ? name[i] : '_';

            return result;
        }

    public:
        self_ref inc_tabs_count(std::size_t count = 1) {
            _tabs_count_backup = _tabs_count;
            _tabs_count += count;
            return *this;
        }

        self_ref dec_tabs_count(std::size_t count = 1) {
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
// Copyright 2023 neverlosecc
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
