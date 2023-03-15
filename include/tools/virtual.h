#ifndef VIRTUAL_H__
#define VIRTUAL_H__

#pragma once

#include <Include.h>

namespace Virtual
{
    template <typename T>
    static __forceinline T Get(void *instance, const unsigned int index)
    {
        return (*static_cast<T **>(instance))[index];
    }

    template <typename T>
    T Read(const std::uintptr_t location) { return *reinterpret_cast<T *>(location); }

    template <typename T>
    void Write(const std::uintptr_t location, const T &data) { *reinterpret_cast<T *>(location) = data; }
}

#endif
