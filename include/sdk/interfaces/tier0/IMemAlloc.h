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
        return Virtual::Get<void*(__thiscall*)(IMemAlloc*, const void*, std::size_t)>(this, 3)(this, p, size);
    }

    void Free(const void* p) {
        return Virtual::Get<void(__thiscall*)(IMemAlloc*, const void*)>(this, 5)(this, p);
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
