// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

#pragma warning(push)
#pragma warning(disable:4191)
#include "tools/virtual.h"

class IMemAlloc {
public:
    void* Alloc(std::size_t nSize) {
        return Virtual::Get<void*(__thiscall*)(IMemAlloc*, std::size_t)>(this, 1)(this, nSize);
    }

    void* AllocAligned(std::size_t nSize, std::size_t nAlign);

    void* Realloc(void* pMemory, std::size_t nSize) {
        return Virtual::Get<void*(__thiscall*)(IMemAlloc*, void*, std::size_t)>(this, 2)(this, pMemory, nSize);
    }

    void* ReallocAligned(void* pMemory, std::size_t nSize, std::size_t nAlign);

    void* Calloc(std::size_t nNum, std::size_t nSize);

    void Free(void* pMemory) {
        return Virtual::Get<void(__thiscall*)(IMemAlloc*, void*)>(this, 3)(this, pMemory);
    }

    void FreeAligned(void* pMemory);

    size_t GetSizeAligned(void* pMemory);

    std::size_t GetSize(void* pMemory) {
        return Virtual::Get<std::size_t(__thiscall*)(IMemAlloc*, void*)>(this, 17)(this, pMemory);
    }
};

extern IMemAlloc* GetMemAlloc();

#pragma warning(pop)

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
