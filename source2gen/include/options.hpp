#pragma once

namespace source2_gen {
    enum class Language {
        cpp,
        c,
    };

    struct Options {
        Language emit_language{};
        bool static_members{};
    };
} // namespace source2_gen