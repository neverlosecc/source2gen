#include "tools/codegen.h"
#include <gtest/gtest.h>

TEST(CodeGen, Simple) {
    auto builder = codegen::get();

    builder.pragma("once");
    builder.include("<cstdint>");
    builder.next_line();
    builder.comment("it's a-me", true);
    builder.forward_declaration("Mario");

    EXPECT_EQ(builder.str(), "#pragma once\n"
                             "#include <cstdint>\n"
                             "\n"
                             "// it's a-me\n"
                             "struct Mario;\n"
                             "");
}

TEST(CodeGen, Class) {
    auto builder = codegen::get();

    builder.begin_class("Test", "public");
    builder.access_modifier("private");
    builder.end_class();

    EXPECT_EQ(builder.str(), "class Test\n"
                             "{\n"
                             "public:\n"
                             "private:\n"
                             "};\n"
                             "\n");
}

TEST(CodeGen, ClassWithBase) {
    auto builder = codegen::get();

    builder.begin_class_with_base_type("Player", "Entity", "public");

    EXPECT_EQ(builder.str(), "class Player : public Entity\n"
                             "{\n"
                             "public:\n");
}

TEST(CodeGen, ClassContents) {
    auto builder = codegen::get();

    builder.begin_class("Test", "public");
    builder.static_field_getter("int", "power", "tier0", "Game", 19);
    builder.prop("int", "up");
    builder.end_class();

    EXPECT_EQ(
        builder.str(),
        "class Test\n"
        "{\n"
        "public:\n"
        "\tstatic int &Get_power(){return *reinterpret_cast<int*>(interfaces::g_schema->FindTypeScopeForModule(\"tier0\")->FindDeclaredClass(\"Game\")->"
        "GetStaticFields()[19]->m_pInstance);};\n"
        "\tint up;\n"
        "};\n"
        "\n"
        "");
}

TEST(CodeGen, Struct) {
    auto builder = codegen::get();

    builder.begin_struct("Test", "public");
    builder.end_struct();

    EXPECT_EQ(builder.str(), "struct Test\n"
                             "{\n"
                             "public:\n"
                             "};\n"
                             "\n");
}

TEST(CodeGen, StructWithBase) {
    auto builder = codegen::get();

    builder.begin_struct_with_base_type("Player", "Entity", "public");

    EXPECT_EQ(builder.str(), "struct Player : public Entity\n"
                             "{\n"
                             "public:\n");
}

TEST(CodeGen, StructWithContents) {
    auto builder = codegen::get();

    builder.begin_struct("Test", "public");
    builder.struct_padding(0x100, 0x200, true, false, 0);
    builder.struct_padding(0x300, 0x200, true, false, 7);
    builder.end_struct();

    EXPECT_EQ(builder.str(), "struct Test\n"
                             "{\n"
                             "public:\n"
                             "\tuint8_t __pad0100[0x200];\n"
                             "\tuint8_t __pad0300: 7;\n"
                             "};\n"
                             "\n");
}

TEST(CodeGen, Namespace) {
    auto builder = codegen::get();

    builder.begin_namespace("sourcesdk");
    builder.end_namespace();

    EXPECT_EQ(builder.str(), "namespace sourcesdk\n"
                             "{\n"
                             "};\n" // this semicolon is unnecessary in C++
                             "\n");
}

TEST(CodeGen, Enum) {
    auto builder = codegen::get();

    builder.begin_enum_class("Choice");
    builder.enum_item("Chocolate", 7);
    builder.enum_item("Strawberries", 9);
    builder.end_enum_class();

    EXPECT_EQ(builder.str(), "enum class Choice\n"
                             "{\n"
                             "\tChocolate = 0x7,\n"
                             "\tStrawberries = 0x9,\n"
                             "};\n"
                             "\n");
}

TEST(CodeGen, Function) {
    auto builder = codegen::get();

    builder.begin_function("static ", "int", "jump", true, true);
    builder.return_value("1234", true);
    builder.end_function(true, true);

    EXPECT_EQ(builder.str(), "static int jump()\n"
                             "{\n"
                             "\treturn 1234;\n"
                             "};\n" // this semicolon is unnecessary in C++
                             "\n");
}

TEST(CodeGen, BitfieldBlock) {
    auto builder = codegen::get();

    builder.begin_bitfield_block();
    builder.end_bitfield_block(true);

    EXPECT_EQ(builder.str(), "struct \n{\n};\n");
}