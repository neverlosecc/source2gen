#pragma once
#include <cstring>
#include <string>

namespace loader {
    class LoadModuleError {
    public:
        /// @return Lifetime bound to this @ref LoadModuleError
        [[nodiscard]] auto as_string() const -> std::string_view {
            return this->m_errorMessage;
        }

        static auto from_string(std::string_view str) {
            return LoadModuleError{str};
        }

    private:
        // we can't use allocating C++ functions (std::string) in here, see doc
        // of LoadModuleError in loader.h.
        char m_errorMessage[512]{};

        explicit LoadModuleError(std::string_view str) {
            std::strncpy(this->m_errorMessage, str.data(), std::min(std::size(m_errorMessage), std::size(str)));
        }
    };
} // namespace loader
