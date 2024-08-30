#pragma once

#include "codegen.h"
#include "detail/c_family.h"
#include <cassert>
#include <list>

namespace codegen {
    struct generator_c_t : public IGenerator {
    public:
        constexpr generator_c_t() = default;
        ~generator_c_t() = default;
        generator_c_t& operator=(const generator_c_t& v) {
            if (this != &v) {
                this->_stream = std::stringstream(v._stream.str());
                /// \todo @es3n1n: Such stats counters should be moved to their own structure
                this->_tabs_count = v._tabs_count;
                this->_tabs_count_backup = v._tabs_count_backup;
                this->_pads_count = v._pads_count;
                this->_forward_decls = v._forward_decls;
            }
            return *this;
        }

    public:
        std::string get_uint(std::size_t bits_count) override {
            return detail::c_family::get_uint(bits_count);
        }

        std::optional<std::string> find_built_in(std::string_view source_name) override {
            const auto found =
                std::ranges::find(detail::c_family::kNumericTypes, source_name, &decltype(detail::c_family::kNumericTypes)::value_type::first);

            if (found != detail::c_family::kNumericTypes.end()) {
                return std::string{found->second};
            } else {
                return std::nullopt;
            }
        }

        std::string get_file_extension() override {
            return "h";
        }

        self_ref preamble() override {
            push_line("#pragma once");
            push_line("");
            include("source2gen_user_types", IncludeOptions{.local = true});
            include("stdbool", IncludeOptions{.local = false});
            include("stdint", IncludeOptions{.local = false});

            return *this;
        }

        self_ref include(std::string_view module_or_file_name, IncludeOptions options) override {
            const auto open_bracket = options.local ? '"' : '<';
            const auto close_bracket = options.local ? '"' : '>';
            // ignore options.system because C system headers have file extensions
            const auto maybe_file_extension = ("." + get_file_extension());

            return push_line(std::format("#include {}{}{}{}", open_bracket, module_or_file_name, maybe_file_extension, close_bracket));
        }

        self_ref next_line() override {
            return push_line("");
        }

        // @todo: @es3n1n: `self_ref prev_line()`

        self_ref access_modifier(const std::string& modifier) override {
            return *this;
        }

        self_ref begin_class(const std::string& class_name, const std::string& access_modifier = "public") override {
            return begin_struct(class_name, access_modifier);
        }

        self_ref begin_class_with_base_type(const std::string& class_name, const std::string& base_type,
                                            const std::string& access_modifier = "public") override {
            return begin_struct_with_base_type(class_name, base_type, access_modifier);
        }

        self_ref end_class() override {
            return end_struct();
        }

        self_ref begin_struct(std::string_view name, const std::string& access_modifier = "public") override {
            assert(!_current_class_or_enum.has_value() && "nested types are not supported");
            _current_class_or_enum = name;

            return begin_block(std::format("struct {}", escape_name(name)));
        }

        self_ref begin_struct_with_base_type(const std::string& name, const std::string& base_type,
                                             const std::string& access_modifier = "public") override {
            if (base_type.empty())
                return begin_struct(name, access_modifier);
            else {
                assert(!_current_class_or_enum.has_value() && "nested types are not supported");
                _current_class_or_enum = name;
            }

            begin_block(std::format("struct {}", escape_name(name)));
            return prop(Prop{.type_name = base_type, .name = "base"});
        }

        self_ref end_struct() override {
            assert(_current_class_or_enum.has_value() && "called end_struct() without calling begin_struct()");

            end_block();
            for (const auto& e : _static_fields) {
                begin_function("static ", e.type_name, std::format("&{}_Get_{}", _current_class_or_enum.value(), e.prop_name), false, false);

                // @note: @es3n1n: reset tabs count temporary
                //
                const auto backup_tabs_count = _tabs_count;
                _tabs_count = 0;

                const auto getter = std::format(
                    R"(*({}*)(interfaces::g_schema->FindTypeScopeForModule("{}")->FindDeclaredClass("{}")->GetStaticFields()[{}]->m_pInstance))",
                    e.type_name, e.mod_name, e.decl_class, e.index);
                return_value(getter, false);
                end_function(false, false);

                // @note: @es3n1n: restore tabs count
                //
                _tabs_count = backup_tabs_count;
            }

            _static_fields.clear();
            _current_class_or_enum = std::nullopt;

            return *this;
        }

        self_ref begin_namespace(const std::string& namespace_name) override {
            comment(std::format("namespace {}", namespace_name));
            return comment("{");
        }

        self_ref end_namespace() override {
            // This semicolon is unnecessary. We're keeping it for consistency
            // with the cpp generator, where it's also unnecessary, but not as
            // easy to remove.
            comment("};");
            return push_line("");
        }

        self_ref begin_enum_class(const std::string& enum_name, const std::string& base_typename = "") override {
            assert(!_current_class_or_enum.has_value() && "nested types are not supported");
            _current_class_or_enum = enum_name;
            return begin_block(std::format("enum {}{}", escape_name(enum_name), base_typename.empty() ? base_typename : (" : " + base_typename)));
        }

        self_ref end_enum_class() override {
            _current_class_or_enum = std::nullopt;
            return end_block();
        }

        self_ref enum_item(const std::string& name, std::uint64_t value) override {
            assert(_current_class_or_enum.has_value() && "called enum_item() without calling begin_enum_class()");

            return push_line(std::format("{}_{} = {:#x},", escape_name(_current_class_or_enum.value()), name, value));
        }

        // @todo: @es3n1n: add func params
        self_ref begin_function(const std::string& prefix, const std::string& type_name, const std::string& func_name,
                                const bool increment_tabs_count = true, const bool move_cursor_to_next_line = true) override {
            return begin_block(std::format("{}{} {}()", prefix, type_name, escape_name(func_name)), increment_tabs_count, move_cursor_to_next_line);
        }

        self_ref end_function(const bool decrement_tabs_count = true, const bool move_cursor_to_next_line = true) override {
            return end_block(decrement_tabs_count, move_cursor_to_next_line);
        }

        self_ref return_value(const std::string& value, const bool move_cursor_to_next_line = true) override {
            return push_line(std::format("return {};", value), move_cursor_to_next_line);
        }

        self_ref static_field_getter(const std::string& type_name, const std::string& prop_name, const std::string& mod_name,
                                     const std::string& decl_class, const std::size_t index) override {
            _static_fields.emplace_back(
                StaticField{.type_name{type_name}, .prop_name{prop_name}, .mod_name{mod_name}, .decl_class{decl_class}, .index{index}});
            return *this;
        }

        self_ref static_assert_size(std::string_view type_name, int expected_size, const bool move_cursor_to_next_line) override {
            assert(expected_size > 0);

            return push_line(std::format("static_assert(sizeof({}) == {:#x});", escape_name(type_name), expected_size));
        }

        self_ref static_assert_offset(std::string_view class_name, std::string_view prop_name, int expected_offset,
                                      const bool move_cursor_to_next_line) override {
            assert(expected_offset >= 0);

            return push_line(std::format("static_assert(offsetof({}, {}) == {:#x});", escape_name(class_name), prop_name, expected_offset));
        }

        self_ref comment(const std::string& text, const bool move_cursor_to_next_line = true) override {
            return push_line(std::format("// {}", text), move_cursor_to_next_line);
        }

        self_ref begin_multi_line_comment(const bool move_cursor_to_next_line = true) override {
            return push_line("/*", move_cursor_to_next_line);
        }

        self_ref end_multi_line_comment(const bool move_cursor_to_next_line = true) override {
            return push_line("*/", move_cursor_to_next_line);
        }

        self_ref prop(Prop prop, bool move_cursor_to_next_line = true) override {
            const auto line =
                std::format("{} {}{};{}", escape_name(prop.type_name), escape_name(prop.name),
                            prop.bitfield_size.has_value() ? std::format(": {}", prop.bitfield_size.value()) : "", move_cursor_to_next_line ? "" : " ");
            return push_line(line, move_cursor_to_next_line);
        }

        self_ref forward_declaration(const std::string& text) override {
            // @note: @es3n1n: forward decl only once
            const auto fwd_decl_hash = fnv32::hash_runtime(text.data());
            if (_forward_decls.contains(fwd_decl_hash))
                return *this;

            _forward_decls.insert(fwd_decl_hash);

            // @fixme: split method to class_forward_declaration & struct_forward_declaration
            // one for `struct uwu_t` and the other one for `class c_uwu`
            return push_line(std::format("struct {};", escape_name(text)));
        }

        self_ref struct_padding(Padding options) override {
            const auto is_bitfield = std::holds_alternative<Padding::Bits>(options.size);

            // @note: @es3n1n: mark private fields as maybe_unused to silence -Wunused-private-field
            std::string type_name = is_bitfield ? detail::c_family::guess_bitfield_type(std::get<Padding::Bits>(options.size).value) : "uint8_t";
            if (options.is_private_field)
                type_name = "[[maybe_unused]] " + type_name;

            auto pad_name =
                options.pad_offset.has_value() ? std::format("__pad{:04x}", options.pad_offset.value()) : std::format("__pad{:d}", _pads_count++);
            if (!is_bitfield)
                pad_name = pad_name + std::format("[{:#x}]", std::get<Padding::Bytes>(options.size).value);

            return prop(Prop{.type_name = type_name,
                             .name = pad_name,
                             .bitfield_size = is_bitfield ? std::make_optional(std::get<Padding::Bits>(options.size).value) : std::nullopt},
                        options.move_cursor_to_next_line);
        }

        self_ref begin_bitfield_block() override {
            return begin_block(std::format("struct ", ""));
        }

        self_ref end_bitfield_block(const bool move_cursor_to_next_line = true) override {
            dec_tabs_count(1);
            return push_line(move_cursor_to_next_line ? "};" : "}; ", move_cursor_to_next_line);
        }

        self_ref restore_tabs_count() override {
            _tabs_count = _tabs_count_backup;
            return *this;
        }

        self_ref reset_tabs_count() override {
            _tabs_count_backup = _tabs_count;
            _tabs_count = 0;
            return *this;
        }

    public:
        [[nodiscard]] std::string str() const override {
            return _stream.str();
        }

    private:
        self_ref begin_block(const std::string& text, const bool increment_tabs_count = true, const bool move_cursor_to_next_line = true) {
            push_line(text, move_cursor_to_next_line);

            // @note: @es3n1n: we should reset tabs count if we aren't moving cursor to
            // the next line
            const auto backup_tabs_count = _tabs_count;
            if (!move_cursor_to_next_line)
                _tabs_count = 0;

            push_line("{", move_cursor_to_next_line);

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

        self_ref push_line(const std::string& line, bool move_cursor_to_next_line = true) {
            for (std::size_t i = 0; i < _tabs_count; i++)
                _stream << kTabSym;
            _stream << line;
            if (move_cursor_to_next_line)
                _stream << std::endl;
            return *this;
        }

        static std::string escape_name(std::string_view name) {
            std::string result;
            result.resize(name.size());

            for (std::size_t i = 0; i < name.size(); i++)
                result[i] = std::ranges::find(detail::c_family::kBlacklistedCharacters, name[i]) == std::end(detail::c_family::kBlacklistedCharacters) ?
                                name[i] :
                                '_';

            return result;
        }

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

    private:
        struct StaticField {
            std::string type_name{};
            std::string prop_name{};
            std::string mod_name{};
            std::string decl_class{};
            std::size_t index{};
        };

    private:
        std::stringstream _stream = {};
        std::size_t _tabs_count = 0, _tabs_count_backup = 0;
        std::size_t _pads_count = 0;
        std::optional<std::string> _current_class_or_enum{std::nullopt};
        std::list<StaticField> _static_fields{};
        std::set<fnv32::hash> _forward_decls = {};
    };
} // namespace codegen
