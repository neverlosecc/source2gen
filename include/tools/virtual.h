// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <cstdint>

namespace Virtual {
    template <typename T>
    inline T Get(void* instance, const unsigned int index) {
        return (*static_cast<T**>(static_cast<void*>(instance)))[index];
    }

    template <typename T>
    inline T Get(const void* instance, const unsigned int index) {
        return (*static_cast<T* const*>(static_cast<const void*>(instance)))[index];
    }

    template <typename T>
    inline T Read(const std::uintptr_t location) {
        return *reinterpret_cast<T*>(location);
    }

    template <typename T>
    inline void Write(const std::uintptr_t location, const T& data) {
        *reinterpret_cast<T*>(location) = data;
    }
} // namespace Virtual

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
