#pragma once

namespace source2_gen {
    enum class Language {
        cpp,
        c,
    };

    struct Options {
        Language emit_language{};
    };
} // namespace source2_gen