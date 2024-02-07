// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.

#pragma once

template <size_t MAX_SIZE, bool AllowHeapAllocation>
class CBufferStringGrowable;

/*
        Main idea of CBufferString is to provide the base class for the CBufferStringGrowable wich implements stack allocation
        with the ability to convert to the heap allocation if allowed.

        Example usage of CBufferStringGrowable class:

        * Basic buffer allocation:
        ```
                CBufferStringGrowable<256> buff;
                buff.Insert(0, "Hello World!");
                printf("Result: %s\n", buff.Get());
        ```
        additionaly the heap allocation of the buffer could be disabled, by providing ``AllowHeapAllocation`` template argument,
        by disabling heap allocation, if the buffer capacity is not enough to perform the operation, the app would exit with an Assert;

        * Additional usage:
        CBufferString::IsStackAllocated() - could be used to check if the buffer is stack allocated;
        CBufferString::IsHeapAllocated() - could be used to check if the buffer is heap allocated;
        CBufferString::Get() - would return a pointer to the data, or an empty string if it's not allocated.

        * Additionaly current length of the buffer could be read via CBufferString::GetTotalNumber()
        and currently allocated amount of bytes could be read via CBufferString::GetAllocatedNumber()

        * Most, if not all the functions would ensure the buffer capacity and enlarge it when needed,
        in case of stack allocated buffers, it would switch to heap allocation instead.
*/

class CBufferString {
protected:
    // You shouldn't be initializing this class, use CBufferStringGrowable instead.
    CBufferString() { }

public:
    enum EAllocationOption_t {
        UNK1 = -1,
        UNK2 = 0,
        UNK3 = (1 << 1),
        UNK4 = (1 << 8),
        UNK5 = (1 << 9),
        ALLOW_HEAP_ALLOCATION = (1 << 31)
    };

    enum EAllocationFlags_t {
        LENGTH_MASK = (1 << 30) - 1,
        FLAGS_MASK = ~LENGTH_MASK,

        STACK_ALLOCATION_MARKER = (1 << 30),
        HEAP_ALLOCATION_MARKER = (1 << 31)
    };

    // Casts to CBufferStringGrowable. Very dirty solution until someone figures out the sane one.
    template <size_t MAX_SIZE = 8, bool AllowHeapAllocation = true, typename T = CBufferStringGrowable<MAX_SIZE, AllowHeapAllocation>>
    T* ToGrowable() {
        return (T*)this;
    }
};

template <size_t MAX_SIZE, bool AllowHeapAllocation = true>
class CBufferStringGrowable : public CBufferString {
    friend class CBufferString;
public:
    CBufferStringGrowable(): m_nTotalCount(0), m_nAllocated(STACK_ALLOCATION_MARKER | (MAX_SIZE & LENGTH_MASK)) {
        memset(m_Memory.m_szString, 0, sizeof(m_Memory.m_szString));
        if (AllowHeapAllocation) {
            m_nAllocated |= ALLOW_HEAP_ALLOCATION;
        }
    }

    CBufferStringGrowable(const CBufferStringGrowable& other): m_nTotalCount(0), m_nAllocated(STACK_ALLOCATION_MARKER | (MAX_SIZE & LENGTH_MASK)) {
        memset(m_Memory.m_szString, 0, sizeof(m_Memory.m_szString));
        if (AllowHeapAllocation) {
            m_nAllocated |= ALLOW_HEAP_ALLOCATION;
        }
        MoveFrom(const_cast<CBufferStringGrowable&>(other));
    }

    ~CBufferStringGrowable() {
        if (IsHeapAllocated() && m_Memory.m_pString) {
            GetMemAlloc()->Free((void*)m_Memory.m_pString);
        }
    }

    inline CBufferStringGrowable& operator=(const CBufferStringGrowable& src) {
        MoveFrom(const_cast<CBufferStringGrowable&>(src));
        return *this;
    }

    inline int GetAllocatedNumber() const {
        return m_nAllocated & LENGTH_MASK;
    }

    inline int GetTotalNumber() const {
        return m_nTotalCount & LENGTH_MASK;
    }

    inline bool IsStackAllocated() const {
        return (m_nAllocated & STACK_ALLOCATION_MARKER) != 0;
    }

    inline bool IsHeapAllocated() const {
        return (m_nTotalCount & HEAP_ALLOCATION_MARKER) != 0;
    }

    inline bool IsInputStringUnsafe(const char* pData) const {
        return ((void*)pData >= this && (void*)pData < &this[1]) ||
               (GetAllocatedNumber() != 0 && pData >= Get() && pData < (Get() + GetAllocatedNumber()));
    }

    inline const char* Get() const {
        if (IsStackAllocated()) {
            return m_Memory.m_szString;
        } else if (GetAllocatedNumber() != 0) {
            return m_Memory.m_pString;
        }

        return "";
    }

    inline void Clear() {
        if (GetAllocatedNumber() != 0) {
            if (IsStackAllocated())
                m_Memory.m_szString[0] = '\0';
            else
                m_Memory.m_pString[0] = '\0';
        }
        m_nTotalCount &= ~LENGTH_MASK;
    }
private:
    int m_nTotalCount;
    int m_nAllocated;

    union {
        char* m_pString;
        char m_szString[MAX_SIZE];
    } m_Memory;
};

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

