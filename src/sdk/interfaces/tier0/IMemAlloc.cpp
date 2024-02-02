// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#include <Include.h>
#include <SDK/Interfaces/tier0/IMemAlloc.h>

IMemAlloc* GetMemAlloc() {
    static auto p_mem_alloc = *reinterpret_cast<IMemAlloc**>(GetProcAddress(GetModuleHandleA("tier0.dll"), "g_pMemAlloc"));
    return p_mem_alloc;
}

_VCRT_ALLOCATOR void* __CRTDECL operator new(std::size_t s) noexcept(false) {
    void* p = GetMemAlloc()->Alloc(s);
    return p;
}

_VCRT_ALLOCATOR void* __CRTDECL operator new[](std::size_t n) noexcept(false) {
    void* p = GetMemAlloc()->Alloc(n);
    return p;
}

void __CRTDECL operator delete(void* p) {
    GetMemAlloc()->Free(p);
}

void __CRTDECL operator delete(void* p, std::size_t size) {
    GetMemAlloc()->Free(p);
}

void __CRTDECL operator delete[](void* p) throw() {
    GetMemAlloc()->Free(p);
}

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
