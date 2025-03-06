#include "tools/codegen/codegen.h"
#include "tools/codegen/cpp.h"
#include <gtest/gtest.h>

TEST(CodeGenCpp, Simple) {
    auto builder = codegen::generator_cpp_t{};

    builder.preamble();
    builder.next_line();
    builder.comment("it's a-me", true);
    builder.forward_declaration("Mario");

    EXPECT_EQ(builder.str(), "#pragma once\n"
                             "\n"
                             "#include <source2sdk/source2gen/source2gen_user_types.hpp>\n"
                             "#include <cstddef>\n"
                             "#include <cstdint>\n"
                             "\n"
                             "// it's a-me\n"
                             "struct Mario;\n"
                             "");
}

TEST(CodeGenCpp, Class) {
    auto builder = codegen::generator_cpp_t{};

    builder.begin_class("Test", "public");
    builder.access_modifier("private");
    builder.end_class();

    EXPECT_EQ(builder.str(), "class Test\n"
                             "{\n"
                             "public:\n"
                             "private:\n"
                             "};\n");
}

TEST(CodeGenCpp, ClassWithBase) {
    auto builder = codegen::generator_cpp_t{};

    builder.begin_class_with_base_type("Player", "Entity", "public");

    EXPECT_EQ(builder.str(), "class Player : public Entity\n"
                             "{\n"
                             "public:\n");
}

TEST(CodeGenCpp, ClassContents) {
    auto builder = codegen::generator_cpp_t{};

    builder.begin_class("Test", "public");
    builder.static_field_getter("int", "power", "tier0", "Game", 19);
    builder.prop(codegen::Prop{.type_category = codegen::TypeCategory::built_in, .type_name = "int", .name = "up"});
    builder.end_class();

    EXPECT_EQ(builder.str(), "class Test\n"
                             "{\n"
                             "public:\n"
                             "\tstatic int &Get_power(){return "
                             "*reinterpret_cast<int*>(interfaces::g_schema->FindTypeScopeForModule(\"tier0\")->FindDeclaredClass(\"Game\")->"
                             "GetStaticFields()[19]->m_pInstance);};\n"
                             "\tint up;\n"
                             "};\n");
}

TEST(CodeGenCpp, Struct) {
    auto builder = codegen::generator_cpp_t{};

    builder.begin_struct("Test", "public");
    builder.end_struct();

    EXPECT_EQ(builder.str(), "struct Test\n"
                             "{\n"
                             "public:\n"
                             "};\n");
}

TEST(CodeGenCpp, StructWithBase) {
    auto builder = codegen::generator_cpp_t{};

    builder.begin_struct_with_base_type("Player", "Entity", "public");

    EXPECT_EQ(builder.str(), "struct Player : public Entity\n"
                             "{\n"
                             "public:\n");
}

TEST(CodeGenCpp, StructWithContents) {
    auto builder = codegen::generator_cpp_t{};

    builder.begin_struct("Test", "public");
    builder.struct_padding(codegen::Padding{
        .pad_offset = 0x100,
        .size = codegen::Padding::Bytes{0x200},
        .is_private_field = false,
        .move_cursor_to_next_line = true,
    });
    builder.struct_padding(codegen::Padding{
        .pad_offset = 0x300,
        .size = codegen::Padding::Bits{7},
        .is_private_field = false,
        .move_cursor_to_next_line = true,
    });
    builder.end_struct();

    EXPECT_EQ(builder.str(), "struct Test\n"
                             "{\n"
                             "public:\n"
                             "\tuint8_t __pad0100[0x200];\n"
                             "\tuint8_t __pad0300: 7;\n"
                             "};\n");
}

TEST(CodeGenCpp, Namespace) {
    auto builder = codegen::generator_cpp_t{};

    builder.begin_namespace("sourcesdk");
    builder.end_namespace();

    EXPECT_EQ(builder.str(), "namespace sourcesdk\n"
                             "{\n"
                             "};\n" // this semicolon is unnecessary in C++
    );
}

TEST(CodeGenCpp, Enum) {
    auto builder = codegen::generator_cpp_t{};

    builder.begin_enum("Choice");
    builder.enum_item("Chocolate", 7);
    builder.enum_item("Strawberries", 9);
    builder.end_enum();

    EXPECT_EQ(builder.str(), "enum class Choice\n"
                             "{\n"
                             "\tChocolate = 0x7,\n"
                             "\tStrawberries = 0x9,\n"
                             "};\n");
}

TEST(CodeGenCpp, Function) {
    auto builder = codegen::generator_cpp_t{};

    builder.begin_function("static ", "int", "jump", true, true);
    builder.return_value("1234", true);
    builder.end_function(true, true);

    EXPECT_EQ(builder.str(), "static int jump()\n"
                             "{\n"
                             "\treturn 1234;\n"
                             "};\n" // this semicolon is unnecessary in C++
    );
}

TEST(CodeGenCpp, BitfieldBlock) {
    auto builder = codegen::generator_cpp_t{};

    builder.begin_bitfield_block();
    builder.end_bitfield_block(true);

    EXPECT_EQ(builder.str(), "// start of bitfield block\n"
                             "// end of bitfield block\n");
}
