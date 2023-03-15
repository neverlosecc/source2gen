#include <Include.h>
#include <SDK/Interfaces/tier0/IMemAlloc.h>

IMemAlloc* GetMemAlloc()
{
    static auto p_mem_alloc = *reinterpret_cast<IMemAlloc**>(GetProcAddress(
                                                                            GetModuleHandleA("tier0.dll"),
                                                                            "g_pMemAlloc"
                                                                           ));
    return p_mem_alloc;
}

_VCRT_ALLOCATOR void* __CRTDECL operator new(std::size_t s) noexcept(false)
{
    void* p = GetMemAlloc()->Alloc(s);
    return p;
}

_VCRT_ALLOCATOR void* __CRTDECL operator new[](std::size_t n) noexcept(false)
{
    void* p = GetMemAlloc()->Alloc(n);
    return p;
}

void __CRTDECL operator delete(void* p) { GetMemAlloc()->Free(p); }

void __CRTDECL operator delete(void* p, std::size_t size) { GetMemAlloc()->Free(p); }

void __CRTDECL operator delete[](void* p) throw() { GetMemAlloc()->Free(p); }
