// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <cassert>
#include <cstdint>
#include <string_view>
#include <iostream>

#include <tools/loader/loader.h>

namespace sdk {
    using InstantiateInterfaceFn = void* (*)();

    class InterfaceReg {
    public:
        InstantiateInterfaceFn m_create_fn_;
        const char* m_name_;
        InterfaceReg* m_next_;
    };

    inline const InterfaceReg* GetInterfaces(const char* library) {
        auto* const library_handle = loader::find_module_handle(library);
        assert(library_handle != nullptr &&
               "Tried to call GetInterfaces() on a library that is not loaded. Is the library listed in get_required_modules()?");

        const auto maybe_createinterface_symbol = loader::find_module_symbol<uintptr_t>(library_handle, "CreateInterface");

        if(&maybe_createinterface_symbol == nullptr) {
            std::cerr << "Could not find CreateInterface" << std::endl;
            return nullptr;
        }

        return maybe_createinterface_symbol
    .transform([](auto createinterface_symbol) -> const InterfaceReg* {
        if constexpr (current_platform == platform::windows) {
            // Windows logic
            auto interface_list = createinterface_symbol + *reinterpret_cast<int32_t*>(createinterface_symbol + 3) + 7;
            return *reinterpret_cast<InterfaceReg**>(interface_list);
        } else if constexpr (current_platform == platform::linux) {
            // Linux logic
            // Fixed by Bubbles -- thank you Ghidra and chatgpt bc this is super dumb
            // Offsets from Ghidra, relative to CreateInterface function start
            constexpr uintptr_t kInstrOffset = 0x10;   // offset of 'mov rbx, [rip+disp32]' in CreateInterface
            constexpr int32_t   kDisp        = 0x000328d9; // displacement in that instruction

            // Compute RIP-relative address
            uintptr_t rip_after = reinterpret_cast<uintptr_t>(createinterface_symbol) + kInstrOffset + 7; // instruction length = 7
            auto interface_list_ptr = reinterpret_cast<const InterfaceReg* const*>(rip_after + kDisp);
                if (!interface_list_ptr) {
                std::cerr << "interface_list_ptr is null\n";
                return nullptr;
            }

                const InterfaceReg* head = *interface_list_ptr;
                if (!head) {
                std::cerr << "InterfaceReg head is null, check offsets!\n";
                return nullptr;
            }

            return head;
        }
    })
    .value_or(nullptr);
    }

    template <typename T = void*>
    T* GetInterface(const char* library, const char* partial_version) {
        for (const InterfaceReg* current = GetInterfaces(library); current != nullptr; current = current->m_next_) {
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
