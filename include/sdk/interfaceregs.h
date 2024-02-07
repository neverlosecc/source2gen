// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

namespace sdk {
    using InstantiateInterfaceFn = void* (*)();

    class InterfaceReg {
    public:
        InstantiateInterfaceFn m_create_fn_;
        const char* m_name_;
        InterfaceReg* m_next_;
    };

    inline const InterfaceReg* GetInterfaces(const char* library) {
        const auto library_handle = GetModuleHandleA(library);

        const auto createinterface_symbol = reinterpret_cast<std::uintptr_t>(GetProcAddress(library_handle, "CreateInterface"));

        const uintptr_t interface_list = createinterface_symbol + *reinterpret_cast<int32_t*>(createinterface_symbol + 3) + 7;

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
// Copyright 2023 neverlosecc
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
