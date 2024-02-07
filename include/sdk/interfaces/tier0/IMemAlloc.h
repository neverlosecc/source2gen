// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

#pragma warning(push)
#pragma warning(disable:4191)
#include "tools/virtual.h"

class IMemAlloc {
public:
    void* Alloc(std::size_t size) {
        return Virtual::Get<void*(__thiscall*)(IMemAlloc*, std::size_t)>(this, 1)(this, size);
    }

    void* ReAlloc(const void* p, std::size_t size) {
        return Virtual::Get<void*(__thiscall*)(IMemAlloc*, const void*, std::size_t)>(this, 2)(this, p, size);
    }

    void Free(const void* p) {
        return Virtual::Get<void(__thiscall*)(IMemAlloc*, const void*)>(this, 3)(this, p);
    }

    std::size_t GetSize(const void* p) {
        return Virtual::Get<std::size_t(__thiscall*)(IMemAlloc*, const void*)>(this, 21)(this, p);
    }
};

extern IMemAlloc* GetMemAlloc();

_VCRT_ALLOCATOR void* __CRTDECL operator new(std::size_t s);
_VCRT_ALLOCATOR void* __CRTDECL operator new[](std::size_t s);
void __CRTDECL operator delete(void* p);
void __CRTDECL operator delete(void* p, std::size_t s);
void __CRTDECL operator delete[](void* p);

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
