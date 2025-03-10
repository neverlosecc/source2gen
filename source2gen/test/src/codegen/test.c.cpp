#include "tools/codegen/c.h"
#include "tools/codegen/codegen.h"
#include <gtest/gtest.h>

TEST(CodeGenC, Simple) {
    auto builder = codegen::generator_c_t{};

    builder.preamble();
    builder.next_line();
    builder.comment("it's a-me", true);
    builder.forward_declaration("Mario");

    EXPECT_EQ(builder.str(), "#pragma once\n"
                             "\n"
                             "#include <source2sdk/source2gen/source2gen_user_types.h>\n"
                             "#include <stdbool.h>\n"
                             "#include <stddef.h>\n"
                             "#include <stdint.h>\n"
                             "\n"
                             "// it's a-me\n"
                             "struct Mario;\n"
                             "");
}

TEST(CodeGenC, Class) {
    auto builder = codegen::generator_c_t{};

    builder.begin_class("Test", "public");
    builder.access_modifier("private"); // noop
    builder.end_class();

    EXPECT_EQ(builder.str(), "struct Test\n"
                             "{\n"
                             "\t// This is an empty struct. There is no data in this struct. A pad has been generated for compliance with C.\n"
                             "\tchar pad_do_not_access;\n"
                             "};\n"
                             "\n");
}

TEST(CodeGenC, ClassWithBase) {
    auto builder = codegen::generator_c_t{};

    builder.begin_class_with_base_type("Player", "Entity", "public");

    EXPECT_EQ(builder.str(), "struct Player\n"
                             "{\n"
                             "\tstruct Entity base;\n");
}

TEST(CodeGenC, ClassContents) {
    auto builder = codegen::generator_c_t{};

    builder.begin_class("Test", "public");
    builder.static_field_getter("int", "power", "tier0", "Game", 19);
    builder.prop(codegen::Prop{.type_name = "int", .name = "up"});
    builder.prop(codegen::Prop{.type_name = "uint8_t", .name = "down", .bitfield_size = 7});
    builder.end_class();

    EXPECT_EQ(builder.str(), "struct Test\n"
                             "{\n"
                             "\tint up;\n"
                             "\tuint8_t down: 7;\n"
                             "};\n"
                             "\n");
}

TEST(CodeGenC, EscapesClassMembers) {
    auto builder = codegen::generator_c_t{};

    builder.begin_class("Test", "public");
    builder.prop(codegen::Prop{.type_name = "CPlayer::Head", .name = "very::big"});
    builder.end_class();

    EXPECT_EQ(builder.str(), "struct Test\n"
                             "{\n"
                             "\tCPlayer_Head very_big;\n"
                             "};\n"
                             "\n");
}

TEST(CodeGenC, Struct) {
    auto builder = codegen::generator_c_t{};

    builder.begin_struct("Test", "public");
    builder.end_struct();

    EXPECT_EQ(builder.str(), "struct Test\n"
                             "{\n"
                             "\t// This is an empty struct. There is no data in this struct. A pad has been generated for compliance with C.\n"
                             "\tchar pad_do_not_access;\n"
                             "};\n"
                             "\n");
}

TEST(CodeGenC, StructWithBase) {
    auto builder = codegen::generator_c_t{};

    builder.begin_struct_with_base_type("Player", "Entity", "public");

    EXPECT_EQ(builder.str(), "struct Player\n"
                             "{\n"
                             "\tstruct Entity base;\n");
}

TEST(CodeGenC, StructWithContents) {
    auto builder = codegen::generator_c_t{};

    builder.begin_struct("Test", "public");
    builder.struct_padding(
        codegen::Padding{
            .pad_offset = 0x100,
            .size = codegen::Padding::Bytes{0x200},
        },
        true);
    builder.struct_padding(
        codegen::Padding{
            .pad_offset = 0x300,
            .size = codegen::Padding::Bits{7},
        },
        true);
    builder.end_struct();

    EXPECT_EQ(builder.str(), "struct Test\n"
                             "{\n"
                             "\tuint8_t _pad0100[0x200];\n"
                             "\tuint8_t _pad0300: 7;\n"
                             "};\n"
                             "\n");
}

TEST(CodeGenC, Namespace) {
    auto builder = codegen::generator_c_t{};

    builder.begin_namespace("sourcesdk");
    builder.end_namespace();

    EXPECT_EQ(builder.str(), "// namespace sourcesdk\n"
                             "// {\n"
                             "// };\n"
                             "\n");
}

TEST(CodeGenC, Enum) {
    auto builder = codegen::generator_c_t{};

    builder.begin_enum("Choice");
    builder.enum_item("Chocolate", 7);
    builder.enum_item("Strawberries", 9);
    builder.end_enum();

    EXPECT_EQ(builder.str(), "enum Choice\n"
                             "{\n"
                             "\tChoice_Chocolate = 0x7,\n"
                             "\tChoice_Strawberries = 0x9,\n"
                             "};\n"
                             "\n");
}

TEST(CodeGenC, Function) {
    auto builder = codegen::generator_c_t{};

    builder.begin_function("static ", "int", "jump", true, true);
    builder.return_value("1234", true);
    builder.end_function(true, true);

    EXPECT_EQ(builder.str(), "static int jump()\n"
                             "{\n"
                             "\treturn 1234;\n"
                             "};\n" // this semicolon is unnecessary in C
                             "\n");
}

TEST(CodeGenC, BitfieldBlock) {
    auto builder = codegen::generator_c_t{};

    builder.begin_bitfield_block();
    builder.end_bitfield_block(true);

    EXPECT_EQ(builder.str(), "// start of bitfield block\n"
                             "// end of bitfield block\n");
}
