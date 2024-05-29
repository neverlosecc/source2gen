// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include <type_traits>

#if defined(CS2) || defined(DOTA2)
constexpr auto kUtlTsHashVersion = 2;
#else
constexpr auto kUtlTsHashVersion = 1;
#endif

//=============================================================================
//
// Threadsafe Hash
//
// Number of buckets must be a power of 2.
// Key must be intp sized (32-bits on x32, 64-bits on x64)
// Designed for a usage pattern where the data is semi-static, and there
// is a well-defined point where we are guaranteed no queries are occurring.
//
// Insertions are added into a thread-safe list, and when Commit() is called,
// the insertions are moved into a lock-free list
//
// Elements are never individually removed; clears must occur at a time
// where we and guaranteed no queries are occurring
//
using UtlTsHashHandleT = std::uint64_t;

template <class T>
class ITSHashConstructor {
public:
    virtual void Construct(T* pElement) = 0;
};

template <class T>
class CDefaultTSHashConstructor : public ITSHashConstructor<T> {
public:
    virtual void Construct(T* pElement) {
        ::Construct(pElement);
    }
};

inline unsigned HashIntConventional(const int n) // faster but less effective
{
    // first byte
    unsigned hash = 0xAAAAAAAA + (n & 0xFF);
    // second byte
    hash = (hash << 5) + hash + ((n >> 8) & 0xFF);
    // third byte
    hash = (hash << 5) + hash + ((n >> 16) & 0xFF);
    // fourth byte
    hash = (hash << 5) + hash + ((n >> 24) & 0xFF);

    return hash;

    /* this is the old version, which would cause a load-hit-store on every
       line on a PowerPC, and therefore took hundreds of clocks to execute!

    byte *p = (byte *)&n;
    unsigned hash = 0xAAAAAAAA + *p++;
    hash = ( ( hash << 5 ) + hash ) + *p++;
    hash = ( ( hash << 5 ) + hash ) + *p++;
    return ( ( hash << 5 ) + hash ) + *p;
    */
}
template <class KEYTYPE = std::uint64_t>
class CUtlTSHashGenericHash {
public:
    static int Hash(const KEYTYPE& key, int nBucketMask) {
        int nHash = HashIntConventional((std::uint64_t)key);
        if (nBucketMask <= USHRT_MAX) {
            nHash ^= (nHash >> 16);
        }
        if (nBucketMask <= UCHAR_MAX) {
            nHash ^= (nHash >> 8);
        }
        return (nHash & nBucketMask);
    }

    static bool Compare(const KEYTYPE& lhs, const KEYTYPE& rhs) {
        return lhs == rhs;
    }
};

template <class KEYTYPE>
class CUtlTSHashUseKeyHashMethod {
public:
    static int Hash(const KEYTYPE& key, int nBucketMask) {
        std::uint32_t nHash = key.HashValue();
        return (nHash & nBucketMask);
    }

    static bool Compare(const KEYTYPE& lhs, const KEYTYPE& rhs) {
        return lhs == rhs;
    }
};

template <class T, class Keytype = std::uint64_t, int BucketCount = 256, class HashFuncs = CUtlTSHashGenericHash<Keytype>>
class CUtlTSHashV1 {
public:
    // Invalid handle.
    static UtlTsHashHandleT InvalidHandle(void) {
        return static_cast<UtlTsHashHandleT>(0);
    }

    // Returns the number of elements in the hash table
    [[nodiscard]] int BlockSize() const {
        return m_EntryMemory.m_BlocksPerBlob;
    }

    [[nodiscard]] int PeakAlloc() const {
        return m_EntryMemory.m_PeakAlloc;
    }

    // Returns elements in the table
    std::vector<T> GetElements(void);

public:
    // Templatized for memory tracking purposes
    template <typename Data_t>
    struct HashFixedDataInternal_t {
        Keytype m_uiKey;
        HashFixedDataInternal_t<Data_t>* m_pNext;
        Data_t m_Data;
    };

    using HashFixedData_t = HashFixedDataInternal_t<T>;

    // @note: @og: Actually this is hacky-way to obtain data
    template <typename Data_t>
    struct HashBucketFixedDataInternal_t {
        Keytype m_Keytype;
        HashFixedDataInternal_t<Data_t>* m_pNext;
        Data_t m_Data;
    };

    using HashBucketFixedData_t = HashBucketFixedDataInternal_t<T>;

    template <typename Data_t>
    struct HashBucketDataInternalT {
        Data_t m_Data;
        HashBucketFixedData_t* m_pNext;
        Keytype m_Keytype;
    };

    using HashBucketDataT = HashBucketDataInternalT<T>;

    struct CBlob_Unallocated_t {
        CBlob_Unallocated_t* m_pNext = nullptr; // 0x0000
        Keytype m_NumBytes; // 0x0008
        Keytype m_Data; // 0x0010
        Keytype m_Padding; // 0x0018
        std::array<HashBucketDataT, BucketCount> m_List; // 0x0020
    };

    struct HashBucket_t {
        CThreadSpinRWLock m_AddLock;
        HashFixedData_t* m_pFirst;
        HashFixedData_t* m_pFirstUncommitted;
    };

    CUtlMemoryPoolBase m_EntryMemory;
    std::array<HashBucket_t, BucketCount> m_aBuckets;

#if defined(DOTA2)
    bool m_bNeedsCommit;
    CInterlockedInt m_ContentionCheck;
#endif
};

// @note: @og: notice this is hacky-way to obtain elements from CUtlTSHash but its works, so why not
template <class T, class Keytype, int BucketCount, class HashFuncs>
std::vector<T> CUtlTSHashV1<T, Keytype, BucketCount, HashFuncs>::GetElements(void) {
    std::vector<T> list;

    const int n_count = PeakAlloc();
    auto n_index = 0;

    if (!m_EntryMemory.m_pBlobHead)
        return list;

    auto unallocated_data = reinterpret_cast<CBlob_Unallocated_t*>(m_EntryMemory.m_pBlobHead);
    for (auto element = unallocated_data; element; element = element->m_pNext) {
        for (auto i = 0; i < BucketCount; i++) {
            list.emplace_back(element->m_List.at(i).m_Data);
            n_index++;

            if (n_index >= n_count)
                break;
        }
    }
    return list;
}

template <class T, class Keytype = std::uint64_t, int BucketCount = 256, class HashFuncs = CUtlTSHashGenericHash<Keytype>>
class CUtlTSHashV2 {
public:
    // Invalid handle.
    static UtlTsHashHandleT InvalidHandle(void) {
        return static_cast<UtlTsHashHandleT>(0);
    }

    // Returns the number of elements in the hash table
    [[nodiscard]] int BlockSize() const {
        return m_EntryMemory.m_BlockSize;
    }
    [[nodiscard]] int PeakAlloc() const {
        return m_EntryMemory.m_PeakAlloc;
    }
    [[nodiscard]] int BlocksAllocated() const {
        return m_EntryMemory.m_BlocksAllocated;
    }
    [[nodiscard]] int Count() const {
        return BlocksAllocated() == 0 ? PeakAlloc() : BlocksAllocated();
    }

    // Returns elements in the table
    std::vector<T> GetElements(int nFirstElement = 0);

private:
    template <typename Predicate>
    std::vector<T> merge_without_duplicates(const std::vector<T>& allocated_list, const std::vector<T>& un_allocated_list, Predicate pred);

public:
    class HashAllocatedBlob_t {
    public:
        HashAllocatedBlob_t* m_unAllocatedNext; // 0x0000
        char pad_0008[8]; // 0x0008
        T m_unAllocatedData; // 0x0010
        char pad_0018[8]; // 0x0018
    }; // Size: 0x0020

    // Templatized for memory tracking purposes
    template <typename Data_t>
    struct HashFixedDataInternal_t {
        Keytype m_uiKey;
        HashFixedDataInternal_t<Data_t>* m_pNext;
        Data_t m_Data;
    };

    typedef HashFixedDataInternal_t<T> HashFixedData_t;

    class HashBucket_t {
    public:
        CThreadSpinRWLock m_AddLock; // 0x0000
        HashFixedData_t* m_pFirst; // 0x0020
        HashFixedData_t* m_pFirstUncommitted; // 0x0020
    }; // Size: 0x0028
    static_assert(sizeof(HashBucket_t) == 0x28);

    CUtlMemoryPoolBase m_EntryMemory;
    std::array<HashBucket_t, BucketCount> m_aBuckets;
    bool m_bNeedsCommit;
    CInterlockedInt m_ContentionCheck;
};

template <typename T>
bool ptr_compare(const T& item1, const T& item2) {
    return item1 == item2;
}

template <class T, class Keytype, int BucketCount, class HashFuncs>
template <typename Predicate>
inline std::vector<T> CUtlTSHashV2<T, Keytype, BucketCount, HashFuncs>::merge_without_duplicates(const std::vector<T>& allocated_list,
                                                                                                 const std::vector<T>& un_allocated_list, Predicate pred) {
    std::vector<T> merged_list = allocated_list;

    for (const auto& item : un_allocated_list) {
        if (std::find_if(allocated_list.begin(), allocated_list.end(), [&](const T& elem) { return pred(elem, item); }) == allocated_list.end()) {
            merged_list.push_back(item);
        }
    }

    return merged_list;
}

template <class T, class Keytype, int BucketCount, class HashFuncs>
std::vector<T> CUtlTSHashV2<T, Keytype, BucketCount, HashFuncs>::GetElements(int nFirstElement) {
    int n_count = BlocksAllocated();
    std::vector<T> AllocatedList;
    if (n_count > 0) {
        int nIndex = 0;
        for (int i = 0; i < BucketCount; i++) {
            const HashBucket_t& bucket = m_aBuckets[i];
            for (HashFixedData_t* pElement = bucket.m_pFirstUncommitted; pElement; pElement = pElement->m_pNext) {
                if (--nFirstElement >= 0)
                    continue;

                if (pElement->m_Data == nullptr)
                    continue;

                AllocatedList.emplace_back(pElement->m_Data);
                ++nIndex;

                if (nIndex >= n_count)
                    break;
            }
        }
    }

    /// @note: @og: basically, its hacky-way to obtain first-time commited information to memory
#if defined(CS2_OLD)
    n_count = PeakAlloc();
#elif defined(DOTA2) || defined(CS2)
    n_count = PeakAlloc() - BlocksAllocated();
#endif
    std::vector<T> unAllocatedList;
    if (n_count > 0) {
        int nIndex = 0;
        auto m_unBuckets = *reinterpret_cast<HashAllocatedBlob_t**>(&m_EntryMemory.m_FreeBlocks.m_Head.value32);
        for (auto unallocated_element = m_unBuckets; unallocated_element; unallocated_element = unallocated_element->m_unAllocatedNext) {
            if (unallocated_element->m_unAllocatedData == nullptr)
                continue;

            unAllocatedList.emplace_back(unallocated_element->m_unAllocatedData);
            ++nIndex;

            if (nIndex >= n_count)
                break;
        }
    }

#if defined(CS2_OLD)
    return unAllocatedList.size() > AllocatedList.size() ? unAllocatedList : AllocatedList;
#elif defined(DOTA2) || defined(CS2)
    return merge_without_duplicates(AllocatedList, unAllocatedList, ptr_compare<T>);
#endif
}

template <class Ty>
using CUtlTSHash = std::conditional_t<kUtlTsHashVersion == 1, CUtlTSHashV1<Ty>, CUtlTSHashV2<Ty>>;

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
