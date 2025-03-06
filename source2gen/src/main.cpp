// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include "options.hpp"
#include "tools/platform.h"
#include <argparse/argparse.hpp>
#include <Include.h>
#include <optional>
#include <stdexcept>
#include <string_view>

std::optional<source2_gen::Language> parse_language(std::string_view str) {
    using enum source2_gen::Language;

    if (str == "c") {
        return c;
    } else if (str == "cpp") {
        return cpp;
    } else {
        return std::nullopt;
    }
}

// TOOD: make this function a static member of Options
/// @return @ref std::nullopt if "--help" was passed or parsing failed
std::optional<source2_gen::Options> parse_args(int argc, char* argv[]) {
    argparse::ArgumentParser parser{"source2gen"};

    // TOOD: .choices("cpp", "c") breaks the parser when more flags are given
    parser.add_argument("--emit-language").default_value("cpp").help("Programming language to be used for the generated SDK");
    parser.add_argument("--no-static-members").default_value(false).help("Don't generate getters for static member variables");

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

    return source2_gen::Options{.emit_language = language.value(), .static_members = !parser.is_used("no-static-members")};
}

int main(const int argc, char* argv[]) {
    int exit_code = 1;

    const auto maybe_options = parse_args(argc, argv);

    if (maybe_options.has_value()) {
        const auto options{maybe_options.value()};

        if (source2_gen::Dump(options)) {
            std::cout << std::format("Successfully dumped Source 2 SDK, now you can safely close this console.") << std::endl;
            std::cout << kPoweredByMessage << std::endl;
            exit_code = 0;
        }
    }

    /// Errors would be logged in the `source2_gen::Dump` itself
    /// We don't want to call getch on linux as the program would be started within a terminal anyway.
#if TARGET_OS == WINDOWS
    (void)std::getchar();
#endif
    return exit_code;
}

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
