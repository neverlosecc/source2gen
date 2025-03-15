#pragma once

#include "codegen.h"
#include "detail/c_family.h"
#include "tools/fnv.h"
#include <absl/strings/str_join.h>
#include <absl/strings/str_split.h>
#include <cassert>
#include <list>
#include <set>
#include <sstream>

namespace codegen {
    struct generator_c_t final : public IGenerator {
        std::string get_uint(std::size_t bits_count) const override {
            return detail::c_family::get_uint(bits_count);
        }

        std::optional<std::string> find_built_in(std::string_view source_name) const override {
            const auto found =
                std::ranges::find(detail::c_family::kNumericTypes, source_name, &decltype(detail::c_family::kNumericTypes)::value_type::first);

            if (found != detail::c_family::kNumericTypes.end()) {
                return std::string{found->second};
            } else {
                return std::nullopt;
            }
        }

        std::string get_file_extension() const override {
            return "h";
        }

        std::string escape_type_name(std::string_view name) const override {
            return detail::c_family::escape_name(name);
        }

        self_ref preamble() override {
            push_line("#pragma once");
            push_line("");
            include("source2sdk/source2gen/source2gen", IncludeOptions{.local = true, .system = false});
            include("stdbool", IncludeOptions{.local = false, .system = true});
            include("stddef", IncludeOptions{.local = false, .system = true});
            include("stdint", IncludeOptions{.local = false, .system = true});

            return *this;
        }

        self_ref include(std::string_view module_or_file_name, IncludeOptions options) override {
            const auto open_bracket = options.local ? '"' : '<';
            const auto close_bracket = options.local ? '"' : '>';
            // ignore options.system because C system headers have file extensions
            const auto maybe_file_extension = ("." + get_file_extension());

            return push_line(std::format("#include {}{}{}{}", open_bracket, module_or_file_name, maybe_file_extension, close_bracket));
        }

        self_ref pack_push(const std::size_t alignment) override {
            return push_line(std::format("#pragma pack(push, {})", alignment));
        }

        self_ref pack_pop() override {
            return push_line("#pragma pack(pop)");
        }

        self_ref next_line() override {
            return push_line("");
        }

        // @todo: @es3n1n: `self_ref prev_line()`

        self_ref access_modifier([[maybe_unused]] const std::string& modifier) override {
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

        self_ref begin_struct(std::string_view name, [[maybe_unused]] const std::string& access_modifier = "public") override {
            assert(!_current_class_or_enum.has_value() && "nested types are not supported");
            _current_class_or_enum = name;

            // calling encode_current_namespace() here is not ideal because the
            // user has no way of knowing (other than reassembling) what the
            // generated type is actually called in case they want to refer to
            // it later.
            return begin_block(std::format("struct {}", encode_current_namespace(detail::c_family::escape_name(name))));
        }

        self_ref begin_struct_with_base_type(const std::string& name, const std::string& base_type,
                                             [[maybe_unused]] const std::string& access_modifier = "public") override {
            assert(!base_type.empty() && "use begin_struct() for structs with no base type");
            assert(!_current_class_or_enum.has_value() && "nested types are not supported");

            _current_class_or_enum = name;

            begin_block(std::format("struct {}", encode_current_namespace(detail::c_family::escape_name(name))));
            return prop(Prop{.type_category = TypeCategory::class_or_struct, .type_name = base_type, .name = "base"});
        }

        self_ref end_struct() override {
            assert(_current_class_or_enum.has_value() && "called end_struct() without calling begin_struct()");

            if (!_current_struct_has_properties) {
                comment("This is an empty struct. There is no data in this struct. A pad has been generated for compliance with C.");
                prop(Prop{.type_name = "char", .name = "pad_do_not_access"}, true);
            }

            end_block();

            _current_class_or_enum = std::nullopt;
            _current_struct_has_properties = false;

            return *this;
        }

        self_ref begin_namespace(std::string_view namespace_name) override {
            _namespaces.emplace_back(namespace_name);
            comment(std::format("namespace {}", namespace_name));
            return comment("{");
        }

        self_ref end_namespace() override {
            _namespaces.pop_back();
            // This semicolon is unnecessary. We're keeping it for consistency
            // with the cpp generator, where it's also unnecessary, but not as
            // easy to remove.
            comment("};");
            return push_line("");
        }

        self_ref begin_enum(const std::string& enum_name, const std::string& base_typename = "") override {
            assert(!_current_class_or_enum.has_value() && "nested types are not supported");
            _current_class_or_enum = enum_name;
            return begin_block(std::format("enum {}{}", encode_current_namespace(detail::c_family::escape_name(enum_name)),
                                           base_typename.empty() ? base_typename : (" : " + base_typename)));
        }

        self_ref end_enum() override {
            _current_class_or_enum = std::nullopt;
            return end_block();
        }

        self_ref enum_item(const std::string& name, std::uint64_t value) override {
            assert(_current_class_or_enum.has_value() && "called enum_item() without calling begin_enum_class()");

            return push_line(std::format("{}_{} = {:#x},", detail::c_family::escape_name(_current_class_or_enum.value()), name, value));
        }

        // @todo: @es3n1n: add func params
        self_ref begin_function(const std::string& prefix, const std::string& type_name, const std::string& func_name,
                                const bool increment_tabs_count = true, const bool move_cursor_to_next_line = true) override {
            return begin_block(std::format("{}{} {}()", prefix, type_name, detail::c_family::escape_name(func_name)), increment_tabs_count,
                               move_cursor_to_next_line);
        }

        self_ref end_function(const bool decrement_tabs_count = true, const bool move_cursor_to_next_line = true) override {
            return end_block(decrement_tabs_count, move_cursor_to_next_line);
        }

        self_ref return_value(const std::string& value, const bool move_cursor_to_next_line = true) override {
            return push_line(std::format("return {};", value), move_cursor_to_next_line);
        }

        self_ref static_field_getter([[maybe_unused]] const std::string& type_name, [[maybe_unused]] const std::string& prop_name,
                                     [[maybe_unused]] const std::string& mod_name, [[maybe_unused]] const std::string& decl_class,
                                     [[maybe_unused]] const std::size_t index) override {
            // not implemented
            return *this;
        }

        self_ref static_assert_size(std::string_view type_name, int expected_size, const bool move_cursor_to_next_line) override {
            assert(expected_size > 0);

            return push_line(std::format("static_assert(sizeof(struct {}) == {:#x});", detail::c_family::escape_name(type_name), expected_size),
                             move_cursor_to_next_line);
        }

        self_ref static_assert_offset(std::string_view class_name, std::string_view prop_name, int expected_offset,
                                      const bool move_cursor_to_next_line) override {
            assert(expected_offset >= 0);

            return push_line(std::format("static_assert(offsetof(struct {}, {}) == {:#x});", detail::c_family::escape_name(class_name),
                                         detail::c_family::escape_name(prop_name), expected_offset),
                             move_cursor_to_next_line);
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
            _current_struct_has_properties = true;

            const auto type_category_prefix = [&]() -> std::string_view {
                switch (prop.type_category) {
                    using enum TypeCategory;
                case built_in:
                    return "";
                case class_or_struct:
                    return "struct ";
                case union_:
                    return "union ";
                case enum_:
                    return "enum ";
                }
                return "";
            }();

            const auto line =
                std::format("{}{} {}{};{}", type_category_prefix, detail::c_family::escape_name(prop.type_name), detail::c_family::escape_name(prop.name),
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
            return push_line(std::format("struct {};", encode_current_namespace(detail::c_family::escape_name(text))));
        }

        self_ref struct_padding(Padding options, bool move_cursor_to_next_line) override {
            const auto is_bitfield = std::holds_alternative<Padding::Bits>(options.size);

            std::string type_name = is_bitfield ? detail::c_family::guess_bitfield_type(std::get<Padding::Bits>(options.size).value) : "uint8_t";

            auto pad_name =
                options.pad_offset.has_value() ? std::format("_pad{:04x}", options.pad_offset.value()) : std::format("_pad{:d}", _pads_count++);
            if (!is_bitfield)
                pad_name = pad_name + std::format("[{:#x}]", std::get<Padding::Bytes>(options.size).value);

            return prop(Prop{.type_name = type_name,
                             .name = pad_name,
                             .bitfield_size = is_bitfield ? std::make_optional(std::get<Padding::Bits>(options.size).value) : std::nullopt},
                        move_cursor_to_next_line);
        }

        self_ref begin_bitfield_block() override {
            return comment("start of bitfield block");
        }

        self_ref end_bitfield_block(const bool move_cursor_to_next_line = true) override {
            return comment("end of bitfield block", move_cursor_to_next_line);
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
            _stream << std::string(_tabs_count * kIndentWidth, kSpaceSym) // insert spaces
                    << line;

            if (move_cursor_to_next_line) {
                _stream << std::endl;
            }
            return *this;
        }

        [[nodiscard]]
        std::string encode_current_namespace(std::string_view name) {
            return absl::StrJoin(std::list{_namespaces, {std::string{name}}} | std::views::join, "_");
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
        bool _current_struct_has_properties = false;
        std::list<std::string> _namespaces{};
        std::set<fnv32::hash> _forward_decls = {};
    };
} // namespace codegen
