// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4191)
#endif

#include "tools/platform.h"
#include "tools/virtual.h"
#include <cstddef>

class IMemAlloc {
public:
    void* Alloc(std::size_t nSize) {
        return Virtual::Get<void*(__thiscall*)(IMemAlloc*, std::size_t)>(this, platform_specific{.windows = 1, .linux = 2})(this, nSize);
    }

    void* AllocAligned(std::size_t nSize, std::size_t nAlign);

    void* Realloc(void* pMemory, std::size_t nSize) {
        return Virtual::Get<void*(__thiscall*)(IMemAlloc*, void*, std::size_t)>(this, platform_specific{.windows = 2, .linux = 3})(this, pMemory, nSize);
    }

    void* ReallocAligned(void* pMemory, std::size_t nSize, std::size_t nAlign);

    void* Calloc(std::size_t nNum, std::size_t nSize);

    void Free(void* pMemory) {
        return Virtual::Get<void(__thiscall*)(IMemAlloc*, void*)>(this, platform_specific{.windows = 3, .linux = 4})(this, pMemory);
    }

    void FreeAligned(void* pMemory);

    [[nodiscard]] std::size_t GetSizeAligned(void* pMemory);

    [[nodiscard]] std::size_t GetSize(void* pMemory) {
        return Virtual::Get<std::size_t(__thiscall*)(IMemAlloc*, void*)>(this, platform_specific{.windows = 17, .linux = 18})(this, pMemory);
    }
};

#if defined(_WIN64) || defined(_WIN32)
    extern "C" __declspec(dllimport) IMemAlloc* GetMemAlloc();
#else
    extern "C" IMemAlloc* GetMemAlloc();
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

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
