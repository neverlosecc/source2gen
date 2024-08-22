// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
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
