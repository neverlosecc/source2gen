#pragma once
#include <cstring>
#include <string>

namespace loader {
    class ModuleLookupError {
    public:
        /// @return Lifetime bound to this @ref ModuleLookupError
        [[nodiscard]] auto as_string() const -> std::string_view {
            return this->m_errorMessage;
        }

        static auto from_string(std::string_view str) {
            return ModuleLookupError{str};
        }

    private:
        // we can't use allocating C++ functions (std::string) in here because
        // callers might try to load modules before an g_pMemAlloc has been
        // found.
        char m_errorMessage[512]{};

        explicit ModuleLookupError(std::string_view str) {
            std::strncpy(this->m_errorMessage, str.data(), std::min(std::size(m_errorMessage), std::size(str)));
        }
    };
} // namespace loader
