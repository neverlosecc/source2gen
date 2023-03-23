#pragma once
#include <cstdint>
#include <fmt/format.h>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>

#include "tools/fnv.h"

namespace codegen {
    constexpr char kTabSym = '\t';
    constexpr std::size_t kTabsPerBlock = 1; // @note: @es3n1n: how many \t characters shall we place per each block
    constexpr std::initializer_list<char> kBlacklistedCharacters = {':', ';', '\\', '/'};

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
            return push_line(fmt::format("#pragma {}", val));
        }

        self_ref include(const std::string& item) {
            return push_line(fmt::format("#include {}", item));
        }

        self_ref disable_warnings(const std::string& codes) {
            return push_line("#pragma warning(push)").push_line(fmt::format("#pragma warning(disable: {})", codes));
        }

        self_ref pop_warning() {
            return push_line("#pragma warning(pop)");
        }

        self_ref next_line() {
            return push_line("");
        }

        // @todo: @es3n1n: `self_ref prev_line()`

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
                push_line(access_modifier, move_cursor_to_next_line);

            if (increment_tabs_count)
                inc_tabs_count(kTabsPerBlock);

            // @note: @es3n1n: restore tabs count
            if (!move_cursor_to_next_line)
                _tabs_count = backup_tabs_count;

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

        self_ref begin_class(const std::string& class_name) {
            return begin_block(fmt::format("class {}", class_name), "public:");
        }

        self_ref begin_class(const std::string& class_name, std::vector<std::string_view>& base_types) {
            if (base_types.empty())
                return begin_class(std::cref(class_name));

            return begin_block(fmt::format("class {} : public {}", class_name, fmt::join(base_types, ", ")), "public:");
        }

        self_ref end_class() {
            return end_block();
        }

        self_ref begin_namespace(const std::string& namespace_name) {
            return begin_block(fmt::format("namespace {}", namespace_name));
        }

        self_ref end_namespace() {
            return end_block();
        }

        self_ref begin_enum_class(const std::string& enum_name, const std::string& base_typename = "") {
            return begin_block(fmt::format("enum class {}{}", escape_name(enum_name), base_typename.empty() ? base_typename : (" : " + base_typename)));
        }

        self_ref end_enum_class() {
            return end_block();
        }

        template <typename T>
        self_ref enum_item(const std::string& name, T value) {
            return push_line(fmt::vformat(sizeof(T) >= 4 ? "{} = {:#x}," : "{} = {},", fmt::make_format_args(name, value)));
        }

        self_ref begin_struct(const std::string& name) {
            return begin_block(fmt::format("struct {}", escape_name(name)), "public:");
        }

        self_ref begin_struct(const std::string& name, std::vector<std::string_view>& base_types) {
            if (base_types.empty())
                return begin_struct(std::cref(name));

            return begin_block(fmt::format("struct {} : public {}", escape_name(name), fmt::join(base_types, ", ")), "public:");
        }

        self_ref end_struct() {
            return end_block();
        }

        // @todo: @es3n1n: add func params
        self_ref begin_function(const std::string& prefix, const std::string& type_name, const std::string& func_name, bool increment_tabs_count = true,
                                bool move_cursor_to_next_line = true) {
            return begin_block(fmt::format("{}{} {}()", prefix, type_name, escape_name(func_name)), "", increment_tabs_count, move_cursor_to_next_line);
        }

        self_ref end_function(bool decrement_tabs_count = true, bool move_cursor_to_next_line = true) {
            return end_block(decrement_tabs_count, move_cursor_to_next_line);
        }

        self_ref return_value(const std::string& value, bool move_cursor_to_next_line = true) {
            return push_line(fmt::format("return {};", value), move_cursor_to_next_line);
        }

        self_ref static_field_getter(const std::string& type_name, const std::string& prop_name, const std::string& mod_name,
                                     const std::string& decl_class, const std::size_t index) {
            begin_function("static ", type_name, fmt::format("&Get_{}", prop_name), false, false);

            // @note: @es3n1n: reset tabs count temporary
            //
            auto backup_tabs_count = _tabs_count;
            _tabs_count = 0;

            auto getter = fmt::format(
                "*reinterpret_cast<{}*>(interfaces::g_schema->FindTypeScopeForModule(\"{}\")->FindDeclaredClass(\"{}\")->m_static_fields[{}]->m_instance)",
                type_name, mod_name, decl_class, index);
            return_value(getter, false);
            end_function(false, false);

            // @note: @es3n1n: restore tabs count
            //
            _tabs_count = backup_tabs_count;
            return *this;
        }

        self_ref comment(const std::string& text) {
            return push_line(fmt::format("// {}", text));
        }

        self_ref prop(const std::string& type_name, const std::string& name, bool move_cursor_to_next_line = true) {
            return push_line(fmt::format("{} {}; ", type_name, name), move_cursor_to_next_line);
        }

        self_ref forward_declartion(const std::string& text) {
            // @note: @es3n1n: forward decl only once
            const auto fwd_decl_hash = fnv32::hash_runtime(text.data());
            if (_forward_decls.contains(fwd_decl_hash))
                return *this;

            _forward_decls.insert(fwd_decl_hash);

            // @fixme: split method to class_forward_declaration & struct_forward_declaration
            // one for `struct uwu_t` and the other one for `class c_uwu`
            return push_line(fmt::format("struct {};", text));
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
    protected:
        self_ref inc_tabs_count(std::size_t count = 1) {
            _tabs_count += count;
            return *this;
        }

        self_ref dec_tabs_count(std::size_t count = 1) {
            if (_tabs_count)
                _tabs_count -= count;
            return *this;
        }
    private:
        std::stringstream _stream = {};
        std::size_t _tabs_count = 0;
        std::set<fnv32::hash> _forward_decls = {};
    };

    __forceinline generator_t get() {
        return generator_t{};
    }
} // namespace codegen
