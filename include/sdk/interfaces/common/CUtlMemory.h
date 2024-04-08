// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include "tools/virtual.h"
#include <sdk/interfaces/tier0/IMemAlloc.h>

template <class T>
inline T* Construct(T* pMemory) {
    return ::new (pMemory) T;
}

template <class T, typename ARG1>
inline T* Construct(T* pMemory, ARG1 a1) {
    return ::new (pMemory) T(a1);
}

template <class T, typename ARG1, typename ARG2>
inline T* Construct(T* pMemory, ARG1 a1, ARG2 a2) {
    return ::new (pMemory) T(a1, a2);
}

template <class T, typename ARG1, typename ARG2, typename ARG3>
inline T* Construct(T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3) {
    return ::new (pMemory) T(a1, a2, a3);
}

template <class T, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
inline T* Construct(T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4) {
    return ::new (pMemory) T(a1, a2, a3, a4);
}

template <class T, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
inline T* Construct(T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4, ARG5 a5) {
    return ::new (pMemory) T(a1, a2, a3, a4, a5);
}

template <class T>
inline T* CopyConstruct(T* pMemory, const T& src) {
    return ::new (pMemory) T(src);
}

template <class T>
inline void Destruct(T* pMemory) {
    pMemory->~T();

#ifdef _DEBUG
    memset(pMemory, 0xDD, sizeof(T));
#endif
}

template <class T, class I = int>
class CUtlMemory {
public:
    // constructor, destructor
    CUtlMemory(int nGrowSize = 0, int nInitSize = 0);
    CUtlMemory(T* pMemory, int numElements);
    CUtlMemory(const T* pMemory, int numElements);
    ~CUtlMemory();

    // Set the size by which the memory grows
    void Init(int nGrowSize = 0, int nInitSize = 0);

    class Iterator_t {
    public:
        Iterator_t(I i): index(i) { }

        I index;

        bool operator==(const Iterator_t& it) const {
            return index == it.index;
        }
        bool operator!=(const Iterator_t& it) const {
            return index != it.index;
        }
    };

    Iterator_t First() const {
        return Iterator_t(IsIdxValid(0) ? 0 : InvalidIndex());
    }

    Iterator_t Next(const Iterator_t& it) const {
        return Iterator_t(IsIdxValid(it.index + 1) ? it.index + 1 : InvalidIndex());
    }

    I GetIndex(const Iterator_t& it) const {
        return it.index;
    }
    bool IsIdxAfter(I i, const Iterator_t& it) const {
        return i > it.index;
    }
    bool IsValidIterator(const Iterator_t& it) const {
        return IsIdxValid(it.index);
    }
    Iterator_t InvalidIterator() const {
        return Iterator_t(InvalidIndex());
    }

    // element access
    T& operator[](I i);
    const T& operator[](I i) const;
    T& Element(I i);
    const T& Element(I i) const;

    bool IsIdxValid(I i) const;

    static const I INVALID_INDEX = (I)-1; // For use with COMPILE_TIME_ASSERT
    static I InvalidIndex() {
        return INVALID_INDEX;
    }

    T* Base();
    const T* Base() const;

    void SetExternalBuffer(T* pMemory, int numElements);
    void SetExternalBuffer(const T* pMemory, int numElements);
    void AssumeMemory(T* pMemory, int nSize);
    T* Detach();
    void* DetachMemory();

    void Swap(CUtlMemory<T, I>& mem);
    void ConvertToGrowableMemory(int nGrowSize);
    int NumAllocated() const;
    int Count() const;
    void Grow(int num = 1);
    void EnsureCapacity(int num);
    void Purge();
    void Purge(int numElements);
    bool IsExternallyAllocated() const;
    bool IsReadOnly() const;
    void SetGrowSize(int size);

protected:
    void ValidateGrowSize() const { }

    enum {
        EXTERNAL_BUFFER_MARKER = -1,
        EXTERNAL_CONST_BUFFER_MARKER = -2,
    };

public:
    T* m_pMemory;
    int m_nAllocationCount;
    int m_nGrowSize;
};

//-----------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------

template <class T, class I>
CUtlMemory<T, I>::CUtlMemory(int nGrowSize, int nInitAllocationCount): m_pMemory(0), m_nAllocationCount(nInitAllocationCount), m_nGrowSize(nGrowSize) {
    ValidateGrowSize();
    assert(nGrowSize >= 0);
    if (m_nAllocationCount) {
        m_pMemory = (T*)GetMemAlloc()->Alloc(m_nAllocationCount * sizeof(T));
    }
}

template <class T, class I>
CUtlMemory<T, I>::CUtlMemory(T* pMemory, int numElements): m_pMemory(pMemory), m_nAllocationCount(numElements) {
    // Special marker indicating externally supplied modifyable memory
    m_nGrowSize = EXTERNAL_BUFFER_MARKER;
}

template <class T, class I>
CUtlMemory<T, I>::CUtlMemory(const T* pMemory, int numElements): m_pMemory((T*)pMemory), m_nAllocationCount(numElements) {
    // Special marker indicating externally supplied modifyable memory
    m_nGrowSize = EXTERNAL_CONST_BUFFER_MARKER;
}

template <class T, class I>
CUtlMemory<T, I>::~CUtlMemory() {
    Purge();
}

template <class T, class I>
void CUtlMemory<T, I>::Init(int nGrowSize /*= 0*/, int nInitSize /*= 0*/) {
    Purge();

    m_nGrowSize = nGrowSize;
    m_nAllocationCount = nInitSize;
    ValidateGrowSize();
    assert(nGrowSize >= 0);
    if (m_nAllocationCount) {
        m_pMemory = (T*)GetMemAlloc()->Alloc(m_nAllocationCount * sizeof(T));
    }
}

//-----------------------------------------------------------------------------
// Fast swap
//-----------------------------------------------------------------------------
template <class T>
FORCEINLINE void V_swap(T& x, T& y) {
    T temp = x;
    x = y;
    y = temp;
}

template <class T, class I>
void CUtlMemory<T, I>::Swap(CUtlMemory<T, I>& mem) {
    V_swap(m_nGrowSize, mem.m_nGrowSize);
    V_swap(m_pMemory, mem.m_pMemory);
    V_swap(m_nAllocationCount, mem.m_nAllocationCount);
}

//-----------------------------------------------------------------------------
// Switches the buffer from an external memory buffer to a reallocatable buffer
//-----------------------------------------------------------------------------
template <class T, class I>
void CUtlMemory<T, I>::ConvertToGrowableMemory(int nGrowSize) {
    if (!IsExternallyAllocated())
        return;

    m_nGrowSize = nGrowSize;
    if (m_nAllocationCount) {
        int nNumBytes = m_nAllocationCount * sizeof(T);
        T* pMemory = (T*)GetMemAlloc()->Alloc(nNumBytes);
        memcpy(pMemory, m_pMemory, nNumBytes);
        m_pMemory = pMemory;
    } else {
        m_pMemory = NULL;
    }
}

//-----------------------------------------------------------------------------
// Attaches the buffer to external memory....
//-----------------------------------------------------------------------------
template <class T, class I>
void CUtlMemory<T, I>::SetExternalBuffer(T* pMemory, int numElements) {
    // Blow away any existing allocated memory
    Purge();

    m_pMemory = pMemory;
    m_nAllocationCount = numElements;

    // Indicate that we don't own the memory
    m_nGrowSize = EXTERNAL_BUFFER_MARKER;
}

template <class T, class I>
void CUtlMemory<T, I>::SetExternalBuffer(const T* pMemory, int numElements) {
    // Blow away any existing allocated memory
    Purge();

    m_pMemory = const_cast<T*>(pMemory);
    m_nAllocationCount = numElements;

    // Indicate that we don't own the memory
    m_nGrowSize = EXTERNAL_CONST_BUFFER_MARKER;
}

template <class T, class I>
void CUtlMemory<T, I>::AssumeMemory(T* pMemory, int numElements) {
    // Blow away any existing allocated memory
    Purge();

    // Simply take the pointer but don't mark us as external
    m_pMemory = pMemory;
    m_nAllocationCount = numElements;
}

template <class T, class I>
void* CUtlMemory<T, I>::DetachMemory() {
    if (IsExternallyAllocated())
        return NULL;

    void* pMemory = m_pMemory;
    m_pMemory = 0;
    m_nAllocationCount = 0;
    return pMemory;
}

template <class T, class I>
inline T* CUtlMemory<T, I>::Detach() {
    return (T*)DetachMemory();
}

//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template <class T, class I>
inline T& CUtlMemory<T, I>::operator[](I i) {
    assert(!IsReadOnly());
    assert(IsIdxValid(i));
    return m_pMemory[i];
}

template <class T, class I>
inline const T& CUtlMemory<T, I>::operator[](I i) const {
    assert(IsIdxValid(i));
    return m_pMemory[i];
}

template <class T, class I>
inline T& CUtlMemory<T, I>::Element(I i) {
    assert(!IsReadOnly());
    assert(IsIdxValid(i));
    return m_pMemory[i];
}

template <class T, class I>
inline const T& CUtlMemory<T, I>::Element(I i) const {
    assert(IsIdxValid(i));
    return m_pMemory[i];
}

//-----------------------------------------------------------------------------
// is the memory externally allocated?
//-----------------------------------------------------------------------------
template <class T, class I>
bool CUtlMemory<T, I>::IsExternallyAllocated() const {
    return (m_nGrowSize < 0);
}

//-----------------------------------------------------------------------------
// is the memory read only?
//-----------------------------------------------------------------------------
template <class T, class I>
bool CUtlMemory<T, I>::IsReadOnly() const {
    return (m_nGrowSize == EXTERNAL_CONST_BUFFER_MARKER);
}

template <class T, class I>
void CUtlMemory<T, I>::SetGrowSize(int nSize) {
    assert(!IsExternallyAllocated());
    assert(nSize >= 0);
    m_nGrowSize = nSize;
    ValidateGrowSize();
}

//-----------------------------------------------------------------------------
// Gets the base address (can change when adding elements!)
//-----------------------------------------------------------------------------
template <class T, class I>
inline T* CUtlMemory<T, I>::Base() {
    assert(!IsReadOnly());
    return m_pMemory;
}

template <class T, class I>
inline const T* CUtlMemory<T, I>::Base() const {
    return m_pMemory;
}

//-----------------------------------------------------------------------------
// Size
//-----------------------------------------------------------------------------
template <class T, class I>
inline int CUtlMemory<T, I>::NumAllocated() const {
    return m_nAllocationCount;
}

template <class T, class I>
inline int CUtlMemory<T, I>::Count() const {
    return m_nAllocationCount;
}

//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template <class T, class I>
inline bool CUtlMemory<T, I>::IsIdxValid(I i) const {
    // GCC warns if I is an unsigned type and we do a ">= 0" against it (since the comparison is always 0).
    // We Get the warning even if we cast inside the expression. It only goes away if we assign to another variable.
    long x = i;
    return (x >= 0) && (x < m_nAllocationCount);
}

//-----------------------------------------------------------------------------
// Grows the memory
//-----------------------------------------------------------------------------
inline int UtlMemory_CalcNewAllocationCount(int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem) {
    if (nGrowSize) {
        nAllocationCount = ((1 + ((nNewSize - 1) / nGrowSize)) * nGrowSize);
    } else {
        if (!nAllocationCount) {
            // Compute an allocation which is at least as big as a cache line...
            nAllocationCount = (31 + nBytesItem) / nBytesItem;
        }

        while (nAllocationCount < nNewSize) {
#ifndef _X360
            nAllocationCount *= 2;
#else
            int nNewAllocationCount = (nAllocationCount * 9) / 8; // 12.5 %
            if (nNewAllocationCount > nAllocationCount)
                nAllocationCount = nNewAllocationCount;
            else
                nAllocationCount *= 2;
#endif
        }
    }

    return nAllocationCount;
}

template <class T, class I>
void CUtlMemory<T, I>::Grow(int num) {
    assert(num > 0);

    if (IsExternallyAllocated()) {
        // Can't grow a buffer whose memory was externally allocated
        assert(0);
        return;
    }

    auto oldAllocationCount = m_nAllocationCount;
    // Make sure we have at least numallocated + num allocations.
    // Use the grow rules specified for this memory (in m_nGrowSize)
    int nAllocationRequested = m_nAllocationCount + num;

    int nNewAllocationCount = UtlMemory_CalcNewAllocationCount(m_nAllocationCount, m_nGrowSize, nAllocationRequested, sizeof(T));

    // if m_nAllocationRequested wraps index type I, recalculate
    if ((int)(I)nNewAllocationCount < nAllocationRequested) {
        if ((int)(I)nNewAllocationCount == 0 && (int)(I)(nNewAllocationCount - 1) >= nAllocationRequested) {
            --nNewAllocationCount; // deal w/ the common case of m_nAllocationCount == MAX_USHORT + 1
        } else {
            if ((int)(I)nAllocationRequested != nAllocationRequested) {
                // we've been asked to grow memory to a size s.t. the index type can't address the requested amount of memory
                assert(0);
                return;
            }
            while ((int)(I)nNewAllocationCount < nAllocationRequested) {
                nNewAllocationCount = (nNewAllocationCount + nAllocationRequested) / 2;
            }
        }
    }

    m_nAllocationCount = nNewAllocationCount;

    if (m_pMemory) {
        auto ptr = GetMemAlloc()->Alloc(m_nAllocationCount * sizeof(T));

        memcpy(ptr, m_pMemory, oldAllocationCount * sizeof(T));
        m_pMemory = (T*)ptr;
    } else {
        m_pMemory = (T*)GetMemAlloc()->Alloc(m_nAllocationCount * sizeof(T));
    }
}

//-----------------------------------------------------------------------------
// Makes sure we've got at least this much memory
//-----------------------------------------------------------------------------
template <class T, class I>
inline void CUtlMemory<T, I>::EnsureCapacity(int num) {
    if (m_nAllocationCount >= num)
        return;

    if (IsExternallyAllocated()) {
        // Can't grow a buffer whose memory was externally allocated
        assert(0);
        return;
    }
    m_nAllocationCount = num;

    if (m_pMemory) {
        m_pMemory = (T*)GetMemAlloc()->ReAlloc(m_pMemory, m_nAllocationCount * sizeof(T));
    } else {
        m_pMemory = (T*)GetMemAlloc()->Alloc(m_nAllocationCount * sizeof(T));
    }
}

//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------
template <class T, class I>
void CUtlMemory<T, I>::Purge() {
    if (!IsExternallyAllocated()) {
        if (m_pMemory) {
            GetMemAlloc()->Free((void*)m_pMemory);
            m_pMemory = 0;
        }
        m_nAllocationCount = 0;
    }
}

template <class T, class I>
void CUtlMemory<T, I>::Purge(int numElements) {
    assert(numElements >= 0);

    if (numElements > m_nAllocationCount) {
        // Ensure this isn't a grow request in disguise.
        assert(numElements <= m_nAllocationCount);
        return;
    }

    // If we have zero elements, simply do a purge:
    if (numElements == 0) {
        Purge();
        return;
    }

    if (IsExternallyAllocated()) {
        // Can't shrink a buffer whose memory was externally allocated, fail silently like purge
        return;
    }

    // If the number of elements is the same as the allocation count, we are done.
    if (numElements == m_nAllocationCount) {
        return;
    }

    if (!m_pMemory) {
        // Allocation count is non zero, but memory is null.
        assert(m_pMemory);
        return;
    }
    m_nAllocationCount = numElements;
    m_pMemory = (T*)GetMemAlloc()->ReAlloc(m_pMemory, m_nAllocationCount * sizeof(T));
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
