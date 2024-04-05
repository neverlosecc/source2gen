// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include "CUtlRBTree.h"

struct base_utlmap_t {
public:
    // This enum exists so that FOR_EACH_MAP and FOR_EACH_MAP_FAST cannot accidentally
    // be used on a type that is not a CUtlMap. If the code compiles then all is well.
    // The check for IsUtlMap being true should be free.
    // Using an enum rather than a static const bool ensures that this trick works even
    // with optimizations disabled on gcc.
    enum {
        IsUtlMap = true
    };
};

//-----------------------------------------------------------------------------
// An associative container. Pretty much identical to std::map.
//-----------------------------------------------------------------------------
template <typename K, typename T, typename I = unsigned short>
class CUtlMap {
public:
    typedef K KeyType_t;
    typedef T ElemType_t;
    typedef I IndexType_t;

    // Less func typedef
    // Returns true if the first parameter is "less" than the second
    typedef bool (*LessFunc_t)(const KeyType_t&, const KeyType_t&);

    // constructor, destructor
    // Left at growSize = 0, the memory will first allocate 1 element and double in size
    // at each increment.
    // LessFunc_t is required, but may be set after the constructor using SetLessFunc() below
    CUtlMap(int growSize = 0, int initSize = 0, LessFunc_t lessfunc = 0): m_Tree(growSize, initSize, CKeyLess(lessfunc)) { }

    CUtlMap(LessFunc_t lessfunc): m_Tree(CKeyLess(lessfunc)) { }

    void EnsureCapacity(int num) {
        m_Tree.EnsureCapacity(num);
    }

    // gets particular elements
    ElemType_t& Element(IndexType_t i) {
        return m_Tree.Element(i).elem;
    }
    const ElemType_t& Element(IndexType_t i) const {
        return m_Tree.Element(i).elem;
    }
    ElemType_t& operator[](IndexType_t i) {
        return m_Tree.Element(i).elem;
    }
    const ElemType_t& operator[](IndexType_t i) const {
        return m_Tree.Element(i).elem;
    }
    KeyType_t& Key(IndexType_t i) {
        return m_Tree.Element(i).key;
    }
    const KeyType_t& Key(IndexType_t i) const {
        return m_Tree.Element(i).key;
    }

    // Num elements
    unsigned int Count() const {
        return m_Tree.Count();
    }

    // Max "size" of the vector
    IndexType_t MaxElement() const {
        return m_Tree.MaxElement();
    }

    // Checks if a node is valid and in the map
    bool IsValidIndex(IndexType_t i) const {
        return m_Tree.IsValidIndex(i);
    }

    // Checks if the map as a whole is valid
    bool IsValid() const {
        return m_Tree.IsValid();
    }

    // Invalid index
    static IndexType_t InvalidIndex() {
        return CTree::InvalidIndex();
    }

    // Insert method (inserts in order)
    IndexType_t Insert(const KeyType_t& key, const ElemType_t& insert) {
        Node_t node;
        node.key = key;
        node.elem = insert;
        return m_Tree.Insert(node);
    }

    IndexType_t Insert(const KeyType_t& key) {
        Node_t node;
        node.key = key;
        return m_Tree.Insert(node);
    }

    // Find method
    IndexType_t Find(const KeyType_t& key) const {
        Node_t dummyNode;
        dummyNode.key = key;
        return m_Tree.Find(dummyNode);
    }

    // Iteration
    IndexType_t FirstInorder() const {
        return m_Tree.FirstInorder();
    }
    IndexType_t NextInorder(IndexType_t i) const {
        return m_Tree.NextInorder(i);
    }
    IndexType_t PrevInorder(IndexType_t i) const {
        return m_Tree.PrevInorder(i);
    }
    IndexType_t LastInorder() const {
        return m_Tree.LastInorder();
    }

    struct Node_t {
        Node_t() { }

        Node_t(const Node_t& from): key(from.key), elem(from.elem) { }

        KeyType_t key;
        ElemType_t elem;
    };

    class CKeyLess {
    public:
        CKeyLess(LessFunc_t lessFunc): m_LessFunc(lessFunc) { }

        bool operator!() const {
            return !m_LessFunc;
        }

        bool operator()(const Node_t& left, const Node_t& right) const {
            return m_LessFunc(left.key, right.key);
        }

        LessFunc_t m_LessFunc;
    };

    typedef CUtlRBTree<Node_t, I, CKeyLess> CTree;

    CTree* AccessTree() {
        return &m_Tree;
    }

protected:
    CTree m_Tree;
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
