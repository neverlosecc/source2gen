#include "options.hpp"
#include <argparse/argparse.hpp>
#include <iostream>

[[nodiscard]]
static std::optional<source2_gen::Language> parse_language(std::string_view str) {
    using enum source2_gen::Language;

    if (str == "c") {
        return c;
    } else if (str == "c-ida") {
        return c_ida;
    } else if (str == "cpp") {
        return cpp;
    } else {
        return std::nullopt;
    }
}

std::optional<source2_gen::Options> source2_gen::Options::parse_args(int argc, char* argv[]) {
    argparse::ArgumentParser parser{"source2gen"};

    parser.add_argument("--emit-language")
        .choices("cpp", "c", "c-ida")
        .default_value("cpp")
        .help("Programming language to be used for the generated SDK [cpp, c, c-ida]");
    parser.add_argument("--no-static-members").default_value(false).help("Don't generate getters for static member variables");
    parser.add_argument("--no-static-assertions")
        .default_value(false)
        .help("Don't generate static assertions for class size and field offsets (Generated SDK might not work. You can get banned for writing to wrong "
              "offsets!)");

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return std::nullopt;
    }

    const auto language{parse_language(parser.get<std::string>("emit-language"))};

    if (!language.has_value()) {
        std::cerr << "invalid value for --emit-language" << std::endl;
        return std::nullopt;
    }

    return source2_gen::Options{.emit_language = language.value(),
                                .static_members = (language.value() != Language::c_ida) && !parser.is_used("no-static-members"),
                                .static_assertions = (language.value() != Language::c_ida) && !parser.is_used("no-static-assertions")};
}
