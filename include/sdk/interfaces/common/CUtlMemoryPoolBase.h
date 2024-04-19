// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <type_traits>

#if defined(CS2) || defined(DOTA2)
constexpr auto kUtlMemoryPoolBaseVersion = 2;
#else
constexpr auto kUtlMemoryPoolBaseVersion = 1;
#endif


// Ways the memory pool can grow when it needs to make a new blob.
enum class MemoryPoolGrowType_t : std::int32_t {
    UTLMEMORYPOOL_GROW_NONE = 0, // Don't allow new blobs.
    UTLMEMORYPOOL_GROW_FAST = 1, // New blob size is numElements * (i+1)  (ie: the blocks it allocates get larger and larger each time it allocates one).
    UTLMEMORYPOOL_GROW_SLOW = 2, // New blob size is numElements.
    UTLMEMORYPOOL_GROW_RBTREE = 3 // No blobs. All blocks are stored in RBTree.
};

enum MemAllocAttribute_t {
    MemAllocAttribute_Unk0 = 0,
    MemAllocAttribute_Unk1 = 1,
    MemAllocAttribute_Unk2 = 2
};

class CUtlMemoryPoolBaseV1 {
public:
    struct FreeList_t {
        FreeList_t* m_pNext;
    };

    class CBlob {
    public:
        CBlob* m_pNext;
        int m_NumBytes; // Number of bytes in this blob.
        char m_Data[1];
        char m_Padding[3]; // to int align the struct
    };

    int m_BlockSize;
    int m_BlocksPerBlob;

    MemoryPoolGrowType_t m_GrowMode;

    CInterlockedInt m_BlocksAllocated;
    CInterlockedInt m_PeakAlloc;

    std::uint16_t m_nAlignment;
    std::uint16_t m_NumBlobs;

    FreeList_t** m_ppTailOfFreeList;
    FreeList_t* m_pHeadOfFreeList;

    CBlob** m_ppBlobTail;
    CBlob* m_pBlobHead;

    MemAllocAttribute_t m_AllocAttribute;

    bool m_Unk1;
};
static_assert(sizeof(CUtlMemoryPoolBaseV1) == 0x40);

class CUtlMemoryPoolBaseV2 {
public:
    struct FreeList_t {
        FreeList_t* m_pNext;
    };

    class CBlob {
    public:
        CBlob* m_pNext;
        int m_NumBytes; // Number of bytes in this blob.
        char m_Data[1];
        char m_Padding[3]; // to int align the struct
    };

    int m_BlockSize;
    int m_BlocksPerBlob;

    MemoryPoolGrowType_t m_GrowMode;

    CInterlockedInt m_BlocksAllocated;
    CInterlockedInt m_PeakAlloc;
    std::uint16_t m_nAlignment;
    std::uint16_t m_NumBlobs;

    CTSListBase m_FreeBlocks;

    MemAllocAttribute_t m_AllocAttribute;

    CThreadMutex m_Mutex;

    CBlob* m_pBlobHead;

    int m_TotalSize; // m_BlocksPerBlob * (m_NumBlobs + 1) + (m_nAligment + 14)
};
static_assert(sizeof(CUtlMemoryPoolBaseV2) == 0x80);

using CUtlMemoryPoolBase = std::conditional_t<kUtlMemoryPoolBaseVersion == 1, CUtlMemoryPoolBaseV1, CUtlMemoryPoolBaseV2>;

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
