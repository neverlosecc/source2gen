// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include <cassert>
#include <cstring>
#include <sdk/interfaces/tier0/IMemAlloc.h>
#include <tools/loader/loader.h>

namespace {
    // don't use any allocating functions in here, it'll cause a recursive call
    // to operator new()!
    IMemAlloc* MemAllocSystemInitialize() {
        // Attempt to load tier0
        auto tier0 = loader::find_module_handle(LOADER_GET_MODULE_FILE_NAME("tier0"));

        assert(tier0 != nullptr && "You cannot use any allocating functions before tier0 has been loaded."
                                   "That includes std::string{} and std::format()!"
                                   "We're overriding operator new() and depend on tier0."
                                   "Run a backtrace in your debugger to see where an allocation attempt was made, then adjust that code.");

        // Continuously try to get the g_pMemAlloc pointer until it's successful
        auto g_ppMemAlloc_wrap = loader::find_module_symbol<IMemAlloc**>(tier0, "g_pMemAlloc");
        assert(g_ppMemAlloc_wrap.has_value());
        auto g_ppMemAlloc = *g_ppMemAlloc_wrap;

        // If g_pMemAlloc is not found, try initializing it
        if (*g_ppMemAlloc == nullptr) {
            static const auto CMemAllocSystemInitialize = loader::find_module_symbol<void (*)()>(tier0, "CMemAllocSystemInitialize");

            if (CMemAllocSystemInitialize.has_value()) {
                (*CMemAllocSystemInitialize)();
            }
        }

        if (*g_ppMemAlloc == nullptr) {
            // there is no way to recover; we don't have an allocator, abort
            const auto rc = std::fputs("could not initialize g_pMemAlloc, source2gen does not work with this version of the game.\n", stderr);
            if (rc == EOF)
                std::perror("failed to use fputs to print error message");

            std::abort();
        }

        return *g_ppMemAlloc;
    }
} // namespace

bool ValueIsPowerOfTwo(const std::size_t value) // don't clash with mathlib definition
{
    return (value & (value - 1)) == 0;
}

void* IMemAlloc::AllocAligned(std::size_t nSize, std::size_t nAlign) {
    unsigned char* memory;

    if (!ValueIsPowerOfTwo(nAlign))
        return nullptr;

    nAlign = (nAlign > sizeof(void*) ? nAlign : sizeof(void*)) - 1;

    if ((memory = static_cast<unsigned char*>(Alloc(sizeof(void*) + nAlign + nSize))) == static_cast<unsigned char*>(nullptr))
        return nullptr;

    auto res = reinterpret_cast<unsigned char*>(reinterpret_cast<std::size_t>(memory + sizeof(void*) + nAlign) & ~nAlign);
    reinterpret_cast<unsigned char**>(res)[-1] = memory;

    return reinterpret_cast<void*>(res);
}

void* IMemAlloc::ReallocAligned(void* pMemory, std::size_t nSize, std::size_t nAlign) {
    if (!ValueIsPowerOfTwo(nAlign))
        return nullptr;

    // Don't change alignment between allocation + reallocation.
    if ((reinterpret_cast<std::size_t>(pMemory) & (nAlign - 1)) != 0)
        return nullptr;

    if (!pMemory)
        return AllocAligned(nSize, nAlign);

    // Figure out the actual allocation point
    void* memory = pMemory;
    memory = reinterpret_cast<void*>((reinterpret_cast<std::size_t>(memory) & ~(sizeof(void*) - 1)) - sizeof(void*));
    memory = *static_cast<void**>(memory);

    // See if we have enough space
    auto nOffset = reinterpret_cast<std::size_t>(pMemory) - reinterpret_cast<std::size_t>(memory);
    auto nOldSize = GetSize(memory);
    if (nOldSize >= nSize + nOffset)
        return pMemory;

    void* res = AllocAligned(nSize, nAlign);
    memcpy(res, pMemory, nOldSize - nOffset);
    Free(memory);

    return res;
}

// @note: @og: basically, rebuild of MemAlloc_CallocFunc
void* IMemAlloc::Calloc(std::size_t num, std::size_t nSize) {
    const auto total_size = num * nSize;
    const auto memory = Alloc(total_size);
    if (memory) {
        static auto V_tier0_memset = loader::find_module_symbol<void(__cdecl*)(void*, std::int8_t, std::size_t)>(
            loader::find_module_handle(loader::get_module_file_name("tier0")), "V_tier0_memset");

        if (V_tier0_memset.has_value())
            (*V_tier0_memset)(memory, 0, total_size);
    }
    return memory;
}

void IMemAlloc::FreeAligned(void* pMemory) {
    if (pMemory == nullptr)
        return;

    void* memory = pMemory;

    // pAlloc points to the pointer to starting of the memory block
    memory = reinterpret_cast<void*>((reinterpret_cast<std::size_t>(memory) & ~(sizeof(void*) - 1)) - sizeof(void*));

    // pAlloc is the pointer to the start of memory block
    memory = *static_cast<void**>(memory);
    Free(memory);
}

size_t IMemAlloc::GetSizeAligned(void* pMemory) {
    if (pMemory == nullptr)
        return 0;

    void* memory = pMemory;

    // pAlloc points to the pointer to starting of the memory block
    memory = reinterpret_cast<void*>((reinterpret_cast<std::size_t>(memory) & ~(sizeof(void*) - 1)) - sizeof(void*));

    // pAlloc is the pointer to the start of memory block
    memory = *static_cast<void**>(memory);
    return GetSize(memory) - (static_cast<std::byte*>(pMemory) - static_cast<std::byte*>(memory));
}

IMemAlloc* GetMemAlloc() {
    static auto g_pMemAlloc = MemAllocSystemInitialize();
    return g_pMemAlloc;
}

// void *malloc(size_t size) { return GetMemAlloc()->Alloc(size); }
// void *calloc(size_t size, std::size_t n) { return GetMemAlloc()->Calloc(size, n); }
// void *realloc(void *p, std::size_t newsize) { return GetMemAlloc()->Realloc(p, newsize); }
// void  free(void *p) { GetMemAlloc()->Free(p); }

void operator delete(void* p) noexcept {
    GetMemAlloc()->Free(p);
}
void operator delete[](void* p) noexcept {
    GetMemAlloc()->Free(p);
}

void* operator new(std::size_t n) noexcept(false) {
    return GetMemAlloc()->Alloc(n);
}
void* operator new[](std::size_t n) noexcept(false) {
    return GetMemAlloc()->Alloc(n);
}
void* operator new(std::size_t n, std::align_val_t align) noexcept(false) {
    return GetMemAlloc()->AllocAligned(n, static_cast<std::size_t>(align));
}
void* operator new[](std::size_t n, std::align_val_t align) noexcept(false) {
    return GetMemAlloc()->AllocAligned(n, static_cast<std::size_t>(align));
}

void* operator new(std::size_t count, const std::nothrow_t& tag) noexcept {
    return GetMemAlloc()->Alloc(count);
}
void* operator new[](std::size_t count, const std::nothrow_t& tag) noexcept {
    return GetMemAlloc()->Alloc(count);
}
void* operator new(std::size_t count, std::align_val_t al, const std::nothrow_t&) noexcept {
    return GetMemAlloc()->AllocAligned(count, static_cast<std::size_t>(al));
}
void* operator new[](std::size_t count, std::align_val_t al, const std::nothrow_t&) noexcept {
    return GetMemAlloc()->AllocAligned(count, static_cast<std::size_t>(al));
}

void* operator new(size_t size, [[maybe_unused]] int block_use, [[maybe_unused]] const char* file_name, [[maybe_unused]] int line_number) {
    return GetMemAlloc()->Alloc(size);
}

void* operator new[](size_t size, [[maybe_unused]] int block_use, [[maybe_unused]] const char* file_name, [[maybe_unused]] int line_number) {
    return GetMemAlloc()->Alloc(size);
}

void operator delete(void* p, std::align_val_t al) noexcept {
    GetMemAlloc()->FreeAligned(p);
}
void operator delete[](void* p, std::align_val_t al) noexcept {
    GetMemAlloc()->FreeAligned(p);
}

void operator delete(void* p, std::size_t n, std::align_val_t al) noexcept {
    GetMemAlloc()->FreeAligned(p);
}
void operator delete[](void* p, std::size_t n, std::align_val_t al) noexcept {
    GetMemAlloc()->FreeAligned(p);
}
void operator delete(void* p, std::align_val_t al, const std::nothrow_t&) noexcept {
    GetMemAlloc()->FreeAligned(p);
}
void operator delete[](void* p, std::align_val_t al, const std::nothrow_t&) noexcept {
    GetMemAlloc()->FreeAligned(p);
}

void operator delete(void* p, std::size_t n) noexcept {
    GetMemAlloc()->Free(p);
}
void operator delete[](void* p, std::size_t n) noexcept {
    GetMemAlloc()->Free(p);
}

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
