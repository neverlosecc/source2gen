// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include <sdk/interfaces/tier0/IMemAlloc.h>

template <class T>
class CUtlVector {
public:
    CUtlVector() {
        m_Size = 0;
        m_pElements = nullptr;
    }

    T& Element(int i);
    const T& Element(int i) const;

    T& operator[](int i) {
        return m_pElements[i];
    }

    [[nodiscard]] int Count() const {
        return m_Size;
    }

    int m_Size;
    T* m_pElements;

    // Adds an element, uses default constructor
    int AddToHead();
    int AddToTail();
    int InsertBefore(int elem);
    int InsertAfter(int elem);
    // Adds an element, uses copy constructor
    int AddToHead(const T& src);
    int AddToTail(const T& src);
    int InsertBefore(int elem, const T& src);
    int InsertAfter(int elem, const T& src);

    // Grows the vector
    void GrowVector(int num = 1);

    // Shifts elements....
    void ShiftElementsRight(int elem, int num = 1);
    void ShiftElementsLeft(int elem, int num = 1);

    // Element removal
    void FastRemove(int elem); // doesn't preserve order
    void Remove(int elem); // preserves order, shifts elements
    bool FindAndRemove(const T& src); // removes first occurrence of src, preserves order, shifts elements
    bool FindAndFastRemove(const T& src); // removes first occurrence of src, doesn't preserve order

    // Finds an element (element needs operator== defined)
    int GetOffset(const T& src) const;
};

template <typename T>
inline T& CUtlVector<T>::Element(int i) {
    assert(i < m_Size);
    return m_pElements[i];
}

template <typename T>
inline const T& CUtlVector<T>::Element(int i) const {
    assert(i < m_Size);
    return m_pElements[i];
}

template <typename T>
void CUtlVector<T>::GrowVector(int num) {
    m_Size += num;
    if (m_pElements)
        m_pElements = (T*)GetMemAlloc()->ReAlloc(m_pElements, m_Size * sizeof(T));
    else
        m_pElements = (T*)GetMemAlloc()->Alloc(m_Size * sizeof(T));
}

//-----------------------------------------------------------------------------
// Adds an element, uses default constructor
//-----------------------------------------------------------------------------
template <typename T>
inline int CUtlVector<T>::AddToHead() {
    return InsertBefore(0);
}

template <typename T>
inline int CUtlVector<T>::AddToTail() {
    return InsertBefore(m_Size);
}

template <typename T>
inline int CUtlVector<T>::InsertAfter(int elem) {
    return InsertBefore(elem + 1);
}

template <typename T>
int CUtlVector<T>::InsertBefore(int elem) {
    // Can insert at the end
    GrowVector();
    ShiftElementsRight(elem);
    Construct(&Element(elem));
    return elem;
}

//-----------------------------------------------------------------------------
// Adds an element, uses copy constructor
//-----------------------------------------------------------------------------
template <typename T>
inline int CUtlVector<T>::AddToHead(const T& src) {
    return InsertBefore(0, src);
}

template <typename T>
inline int CUtlVector<T>::AddToTail(const T& src) {
    return InsertBefore(m_Size, src);
}

template <typename T>
inline int CUtlVector<T>::InsertAfter(int elem, const T& src) {
    return InsertBefore(elem + 1, src);
}

template <typename T>
int CUtlVector<T>::InsertBefore(int elem, const T& src) {
    GrowVector();
    ShiftElementsRight(elem);
    CopyConstruct(&Element(elem), src);
    return elem;
}

//-----------------------------------------------------------------------------
// Shifts elements
//-----------------------------------------------------------------------------
template <typename T>
void CUtlVector<T>::ShiftElementsRight(int elem, int num) {
    int numToMove = m_Size - elem - num;
    if ((numToMove > 0) && (num > 0))
        memmove(&Element(elem + num), &Element(elem), numToMove * sizeof(T));
}

template <typename T>
void CUtlVector<T>::ShiftElementsLeft(int elem, int num) {
    int numToMove = m_Size - elem - num;
    if ((numToMove > 0) && (num > 0)) {
        memmove(&Element(elem), &Element(elem + num), numToMove * sizeof(T));

#ifdef _DEBUG
        memset(&Element(m_Size - num), 0xDD, num * sizeof(T));
#endif
    }
}

//-----------------------------------------------------------------------------
// Element removal
//-----------------------------------------------------------------------------
template <typename T>
void CUtlVector<T>::FastRemove(int elem) {
    Destruct(&Element(elem));
    if (m_Size > 0) {
        if (elem != m_Size - 1)
            memcpy(&Element(elem), &Element(m_Size - 1), sizeof(T));
        --m_Size;
    }
}

template <typename T>
void CUtlVector<T>::Remove(int elem) {
    Destruct(&Element(elem));
    ShiftElementsLeft(elem);
    --m_Size;
}

template <typename T>
bool CUtlVector<T>::FindAndRemove(const T& src) {
    int elem = GetOffset(src);
    if (elem != -1) {
        Remove(elem);
        return true;
    }
    return false;
}

template <typename T>
bool CUtlVector<T>::FindAndFastRemove(const T& src) {
    int elem = GetOffset(src);
    if (elem != -1) {
        FastRemove(elem);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Finds an element (element needs operator== defined)
//-----------------------------------------------------------------------------
template <typename T>
int CUtlVector<T>::GetOffset(const T& src) const {
    for (auto i = 0; i < Count(); ++i) {
        if (Element(i) == src)
            return i;
    }
    return -1;
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
