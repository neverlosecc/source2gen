#pragma once

#include <optional>

namespace source2_gen {
    enum class Language {
        cpp,
        c,
    };

    struct Options {
        Language emit_language{};
        bool static_members{};

        /// @return @ref std::nullopt if "--help" was passed or parsing failed
        [[nodiscard]]
        static std::optional<source2_gen::Options> parse_args(int argc, char* argv[]);
    };
} // namespace source2_gen
