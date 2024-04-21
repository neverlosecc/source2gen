// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>
#include <string_view>
#include <windows.h>

// keep in sync with get_module_file_name()
#define LOADER_WINDOWS_GET_MODULE_FILE_NAME(name) name ".dll"

namespace Loader::Windows {
    using module_handle_t = HMODULE;

    class LoadModuleError {
    public:
        /// @return Lifetime bound to this @ref LoadModuleError
        [[nodiscard]] auto as_string() const -> std::string_view {
            return this->m_errorMessage;
        }

        static auto from_error(DWORD error) {
            LPSTR pBuffer = nullptr;

            const auto size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error,
                                             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pBuffer, 0, NULL);

            auto result = LoadModuleError{pBuffer};

            LocalFree(pBuffer);

            return result;
        }

    private:
        // we can't use allocating C++ functions (std::string) in here, see doc
        // of LoadModuleError in loader.h.
        char m_errorMessage[512]{};

        LoadModuleError(std::string_view str) {
            std::strncpy(this->m_errorMessage, str.data(), std::min(std::size(m_errorMessage), std::size(str)));
        }
    };

    // keep in sync with LOADER_WINDOWS_GET_MODULE_FILE_NAME
    [[nodiscard]] inline auto get_module_file_name(std::string name) -> std::string {
        return name.append(".dll");
    }

    [[nodiscard]] inline auto find_module_handle(std::string_view name) -> module_handle_t {
        return GetModuleHandleA(name.data());
    }

    [[nodiscard]] inline auto load_module(std::string_view name) -> module_handle_t {
        return LoadLibraryA(name.data());
    }

    [[nodiscard]] inline auto find_module_symbol(module_handle_t handle, std::string_view name) -> std::expected<module_handle_t, LoadModuleError> {
        assert(handle != nullptr);
        if (auto* const handle{ GetProcAddress(handle, name.data() }) {
            return handle;
        } else {
            return std::unexpected{LoadModuleError::from_error(GetLastError())};
        }
    }
} // namespace Loader::Windows

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
