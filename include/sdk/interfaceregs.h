// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <cassert>
#include <cstdint>
#include <string_view>
#include <tools/loader.h>

namespace sdk {
    using CreateInterfaceFn = void* (*)(const char* pName, int* pReturnCode);
    using InstantiateInterfaceFn = void* (*)();

    class InterfaceReg {
    public:
        InstantiateInterfaceFn m_create_fn_;
        const char* m_name_;
        InterfaceReg* m_next_;
    };

    inline const InterfaceReg* GetInterfaces(const char* library) {
        const auto library_handle = loader::find_module_handle(library);
        assert(library_handle != 0);

        const auto createinterface_symbol = loader::find_module_symbol<uintptr_t>(library_handle, "CreateInterface").value_or(0);
        assert(createinterface_symbol != 0);

        const auto interface_list = [=] {
            if constexpr (current_platform == platform::windows) {
                return createinterface_symbol + *reinterpret_cast<int32_t*>(createinterface_symbol + 3) + 7;
            } else if constexpr (current_platform == platform::linux) {
                const auto createinterface_impl = createinterface_symbol + *reinterpret_cast<int32_t*>(createinterface_symbol + 1) + 5;
                const auto createinterface_mov = createinterface_impl + 0x10;

                return createinterface_mov + *reinterpret_cast<int32_t*>(createinterface_mov + 3) + 7;
            }
        }();

        return *reinterpret_cast<InterfaceReg**>(interface_list);
    }

    template <typename T = void*>
    T* GetInterface(const char* library, const char* partial_version) {
        for (const InterfaceReg* current = GetInterfaces(library); current; current = current->m_next_) {
            if (std::string_view(current->m_name_).find(partial_version) != std::string_view::npos)
                return reinterpret_cast<T*>(current->m_create_fn_());
        }

        return nullptr;
    }
} // namespace sdk

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
