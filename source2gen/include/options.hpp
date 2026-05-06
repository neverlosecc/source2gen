#pragma once

#include <optional>
#include <vector>
#include <string>

namespace source2_gen {
    enum class Language {
        cpp,
        c,
        /// C, but can be parsed by IDA.
        /// Derivations from @ref c
        /// - single file
        /// - no `#include` macros
        /// - no `static_assert`
        c_ida,
    };

    struct Options {
        Language emit_language{};
        bool static_members{};
        bool static_assertions{};
        std::vector<std::string> known_types{};

        /// @return @ref std::nullopt if "--help" was passed or parsing failed
        [[nodiscard]]
        static std::optional<source2_gen::Options> parse_args(int argc, char* argv[]);
    };
} // namespace source2_gen
