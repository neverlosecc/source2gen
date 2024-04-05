// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include "CUtlMemory.h"

//-----------------------------------------------------------------------------
// A red-black binary search tree
//-----------------------------------------------------------------------------

template <class I>
struct UtlRBTreeLinks_t {
    I m_Left;
    I m_Right;
    I m_Parent;
    I m_Tag;
};

template <class T, class I>
struct UtlRBTreeNode_t : public UtlRBTreeLinks_t<I> {
    T m_Data;
};

class Node_t;

template <class T, class I = unsigned short, typename L = bool (*)(const T&, const T&), class M = CUtlMemory<UtlRBTreeNode_t<T, I>, I>>
class CUtlRBTree {
public:
    typedef T KeyType_t;
    typedef T ElemType_t;
    typedef I IndexType_t;

    // Less func typedef
    // Returns true if the first parameter is "less" than the second
    typedef L LessFunc_t;

    void EnsureCapacity(int num);

    // gets particular elements
    T& Element(I i) {
        return m_Elements[i].m_Data;
    }
    T const& Element(I i) const {
        return m_Elements[i].m_Data;
    }
    T& operator[](I i) {
        return m_Elements[i].m_Data;
    }
    T const& operator[](I i) const {
        return m_Elements[i].m_Data;
    }

    // Gets the root
    I Root() const {
        return m_Root;
    }

    // Num elements
    unsigned int Count() const {
        return (unsigned int)m_NumElements;
    }

    // Max "size" of the vector
    // it's not generally safe to iterate from index 0 to MaxElement()-1
    // it IS safe to do so when using CUtlMemory as the allocator,
    // but we should really remove patterns using this anyways, for safety and generality
    I MaxElement() const {
        return (I)m_Elements.NumAllocated();
    }

    // Gets the children
    I Parent(I i) const {
        return Links(i).m_Parent;
    }
    I LeftChild(I i) const {
        return Links(i).m_Left;
    }
    I RightChild(I i) const {
        return Links(i).m_Right;
    }

    // Tests if a node is a left or right child
    bool IsLeftChild(I i) const {
        return LeftChild(Parent(i)) == i;
    }
    bool IsRightChild(I i) const {
        return RightChild(Parent(i)) == i;
    }

    // Tests if root or leaf
    bool IsRoot(I i) const {
        return i == m_Root;
    }
    bool IsLeaf(I i) const {
        return (LeftChild(i) == InvalidIndex()) && (RightChild(i) == InvalidIndex());
    }

    // Checks if a node is valid and in the tree
    bool IsValidIndex(I i) const {
        if (!m_Elements.IsIdxValid(i))
            return false;

        if (m_Elements.IsIdxAfter(i, m_LastAlloc))
            return false; // don't read values that have been allocated, but not constructed

        return LeftChild(i) != i;
    }

    // Checks if the tree as a whole is valid
    bool IsValid() const {
        if (!Count())
            return true;

        if (m_LastAlloc == m_Elements.InvalidIterator())
            return false;

        if (!m_Elements.IsIdxValid(Root()))
            return false;

        if (Parent(Root()) != InvalidIndex())
            return false;

        return true;
    }

    // Invalid index
    static I InvalidIndex() {
        return (I)M::InvalidIndex();
    }

    // returns the tree depth (not a very fast operation)
    int Depth(I node) const {
        if (node == InvalidIndex())
            return 0;

        int depthright = Depth(RightChild(node));
        int depthleft = Depth(LeftChild(node));
        return std::max(depthright, depthleft) + 1;
    }

    int Depth() const {
        return Depth(Root());
    }

    // Find method
    I Find(T const& search) const {
        I current = m_Root;
        while (current != InvalidIndex()) {
            if (m_LessFunc(search, Element(current)))
                current = LeftChild(current);
            else if (m_LessFunc(Element(current), search))
                current = RightChild(current);
            else
                break;
        }
        return current;
    }

    bool HasElement(T const& search) const {
        return Find(search) != InvalidIndex();
    }

    // Iteration
    I FirstInorder() const {
        I i = m_Root;
        while (LeftChild(i) != InvalidIndex())
            i = LeftChild(i);
        return i;
    }

    I NextInorder(I i) const {
        // Don't go into an infinite loop if it's a bad index
        Assert(IsValidIndex(i));
        if (!IsValidIndex(i))
            return InvalidIndex();

        if (RightChild(i) != InvalidIndex()) {
            i = RightChild(i);
            while (LeftChild(i) != InvalidIndex())
                i = LeftChild(i);
            return i;
        }

        I parent = Parent(i);
        while (IsRightChild(i)) {
            i = parent;
            if (i == InvalidIndex())
                break;
            parent = Parent(i);
        }
        return parent;
    }

    I PrevInorder(I i) const {
        // Don't go into an infinite loop if it's a bad index
        Assert(IsValidIndex(i));
        if (!IsValidIndex(i))
            return InvalidIndex();

        if (LeftChild(i) != InvalidIndex()) {
            i = LeftChild(i);
            while (RightChild(i) != InvalidIndex())
                i = RightChild(i);
            return i;
        }

        I parent = Parent(i);
        while (IsLeftChild(i)) {
            i = parent;
            if (i == InvalidIndex())
                break;
            parent = Parent(i);
        }
        return parent;
    }

    I LastInorder() const {
        I i = m_Root;
        while (RightChild(i) != InvalidIndex())
            i = RightChild(i);
        return i;
    }

    I FirstPreorder() const {
        return m_Root;
    }

    I NextPreorder(I i) const {
        if (LeftChild(i) != InvalidIndex())
            return LeftChild(i);

        if (RightChild(i) != InvalidIndex())
            return RightChild(i);

        I parent = Parent(i);
        while (parent != InvalidIndex()) {
            if (IsLeftChild(i) && (RightChild(parent) != InvalidIndex()))
                return RightChild(parent);
            i = parent;
            parent = Parent(parent);
        }
        return InvalidIndex();
    }

    I PrevPreorder(I i) const {
        // not implemented yet
        return InvalidIndex();
    }

    I LastPreorder() const {
        I i = m_Root;
        while (1) {
            while (RightChild(i) != InvalidIndex())
                i = RightChild(i);

            if (LeftChild(i) != InvalidIndex())
                i = LeftChild(i);
            else
                break;
        }
        return i;
    }

    I FirstPostorder() const {
        I i = m_Root;
        while (!IsLeaf(i)) {
            if (LeftChild(i))
                i = LeftChild(i);
            else
                i = RightChild(i);
        }
        return i;
    }

    I NextPostorder(I i) const {
        I parent = Parent(i);
        if (parent == InvalidIndex())
            return InvalidIndex();

        if (IsRightChild(i))
            return parent;

        if (RightChild(parent) == InvalidIndex())
            return parent;

        i = RightChild(parent);
        while (!IsLeaf(i)) {
            if (LeftChild(i))
                i = LeftChild(i);
            else
                i = RightChild(i);
        }
        return i;
    }

private:
    // Can't copy the tree this way!
    CUtlRBTree<T, I, L, M>& operator=(const CUtlRBTree<T, I, L, M>& other);

protected:
    enum NodeColor_t {
        RED = 0,
        BLACK
    };

    typedef UtlRBTreeNode_t<T, I> Node_t;
    typedef UtlRBTreeLinks_t<I> Links_t;

    // Gets at the links
    Links_t const& Links(I i) const {
        // Sentinel node, makes life easier
        static Links_t s_Sentinel = {InvalidIndex(), InvalidIndex(), InvalidIndex(), CUtlRBTree<T, I, L, M>::BLACK};

        return (i != InvalidIndex()) ? *(Links_t*)&m_Elements[i] : *(Links_t*)&s_Sentinel;
    }
    Links_t& Links(I i) {
        return *(Links_t*)&m_Elements[i];
    }

    // Checks if a link is red or black
    bool IsRed(I i) const {
        return (Links(i).m_Tag == RED);
    }
    bool IsBlack(I i) const {
        return (Links(i).m_Tag == BLACK);
    }

    // copy constructors not allowed
    CUtlRBTree(CUtlRBTree<T, I, L, M> const& tree);

    // Used for sorting.
    LessFunc_t m_LessFunc;

    M m_Elements;
    I m_Root;
    I m_NumElements;
    I m_FirstFree;
    typename M::Iterator_t m_LastAlloc; // the last index allocated

    Node_t* m_pElements;

    M const& Elements() const {
        return m_Elements;
    }
};

template <class T, class I, typename L, class M>
inline void CUtlRBTree<T, I, L, M>::EnsureCapacity(int num) {
    m_Elements.EnsureCapacity(num);
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
