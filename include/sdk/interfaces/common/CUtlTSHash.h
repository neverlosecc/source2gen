// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include <type_traits>

#if defined(CS2) || defined(DOTA2)
constexpr auto kUtlTsHashVersion = 2;
#else
constexpr auto kUtlTsHashVersion = 1;
#endif

using UtlTsHashHandleT = std::uint64_t;

class CUtlMemoryPool {
public:
    // returns number of allocated blocks
    int BlockSize() const {
        return m_nBlocksPerBlob;
    }
    int BlocksAllocated() const {
        return m_nBlocksAllocated;
    }
    int AllocatedSize() const {
        return m_nBloockAllocatedSize;
    }
    int PeakCount() const {
        return m_nPeakAlloc;
    }
private:
    int32_t m_nBlocksSize = 0; // 0x0000
    int32_t m_nBlocksPerBlob = 0; // 0x0004
    int32_t m_nGrowSize; // 0x0008
    int32_t m_nBlocksAllocated = 0; // 0x000C
    int32_t m_nBloockAllocatedSize = 0; // 0x0010
    int32_t m_nPeakAlloc = 0; // 0x0014
};
static_assert(sizeof(CUtlMemoryPool) == 0x18);

template <class T, class Keytype = std::uint64_t>
class CUtlTSHashV1 {
public:
    // Invalid handle.
    static UtlTsHashHandleT InvalidHandle(void) {
        return static_cast<UtlTsHashHandleT>(0);
    }

    // Returns the number of elements in the hash table
    [[nodiscard]] int BlockSize() const {
        return m_entry_memory_.BlockSize();
    }
    [[nodiscard]] int Count() const {
        return m_entry_memory_.AllocatedSize();
    }

    // Returns elements in the table
    std::vector<T> GetElements(void);
public:
    // Templatized for memory tracking purposes
    template <typename DataT>
    struct HashFixedDataInternalT {
        Keytype m_ui_key;
        HashFixedDataInternalT<DataT>* m_next;
        DataT m_data;
    };

    using HashFixedDataT = HashFixedDataInternalT<T>;

    // Templatized for memory tracking purposes
    template <typename DataT>
    struct HashFixedStructDataInternalT {
        DataT m_data;
        Keytype m_ui_key;
        char pad_0x0020[0x8];
    };

    using HashFixedStructDataT = HashFixedStructDataInternalT<T>;

    struct HashStructDataT {
        char pad_0x0000[0x10]; // 0x0000
        std::array<HashFixedStructDataT, 256> m_list;
    };

    struct HashAllocatedDataT {
    public:
        auto GetList() {
            return m_list_;
        }
    private:
        char pad_0x0000[0x18]; // 0x0000
        std::array<HashFixedDataT, 128> m_list_;
    };

    // Templatized for memory tracking purposes
    template <typename DataT>
    struct HashBucketDataInternalT {
        DataT m_data;
        HashFixedDataInternalT<DataT>* m_next;
        Keytype m_ui_key;
    };

    using HashBucketDataT = HashBucketDataInternalT<T>;

    struct HashUnallocatedDataT {
        HashUnallocatedDataT* m_next_ = nullptr; // 0x0000
        Keytype m_6114; // 0x0008
        Keytype m_ui_key; // 0x0010
        Keytype m_i_unk_1; // 0x0018
        std::array<HashBucketDataT, 256> m_current_block_list; // 0x0020
    };

    struct HashBucketT {
        HashStructDataT* m_struct_data = nullptr;
        void* m_mutex_list = nullptr;
        HashAllocatedDataT* m_allocated_data = nullptr;
        HashUnallocatedDataT* m_unallocated_data = nullptr;
    };

    CUtlMemoryPool m_entry_memory_;
    HashBucketT m_buckets_;
    bool m_needs_commit_ = false;
};

template <class T, class Keytype>
inline std::vector<T> CUtlTSHashV1<T, Keytype>::GetElements(void) {
    std::vector<T> list;

    const int n_count = Count();
    auto n_index = 0;
    auto& unallocated_data = m_buckets_.m_unallocated_data;
    for (auto element = unallocated_data; element; element = element->m_next_) {
        for (auto i = 0; i < BlockSize() && i != n_count; i++) {
            list.emplace_back(element->m_current_block_list.at(i).m_data);
            n_index++;

            if (n_index >= n_count)
                break;
        }
    }
    return list;
}

class CThreadSpinRWLock {
public:
    struct LockInfo_t {
        std::uint32_t m_writerId;
        std::int32_t m_nReaders;
    };
public:
    void* m_pThreadSpin;
    LockInfo_t m_lockInfo;
};

template <class T, class Keytype = std::uint64_t, int BucketCount = 256>
class CUtlTSHashV2 {
public:
    // Invalid handle.
    static UtlTsHashHandleT InvalidHandle(void) {
        return static_cast<UtlTsHashHandleT>(0);
    }

    // Returns the number of elements in the hash table
    [[nodiscard]] int BlockSize() const {
        return m_EntryMemory.BlockSize();
    }
    [[nodiscard]] int AllocatedSize() const {
        return m_EntryMemory.AllocatedSize();
    }
    [[nodiscard]] int BlocksAllocated() const {
        return m_EntryMemory.BlocksAllocated();
    }
    [[nodiscard]] int Count() const {
        return m_EntryMemory.BlocksAllocated() == 0 ? m_EntryMemory.AllocatedSize() : m_EntryMemory.BlocksAllocated();
    }

    // Returns elements in the table
    std::vector<T> GetElements(int nFirstElement = 0);
public:
    class Blob_t {
    public:
        char pad_0000[16]; // 0x0000
        T m_Data; // 0x0010
        char pad_0018[8]; // 0x0018
    }; // Size: 0x0020

    class Block_t {
    public:
        std::array<Blob_t, BucketCount> m_Blob; // 0x0000
    }; // Size: 0x2080

    class HashAllocated_t {
    public:
        std::uint64_t m_Token; // 0x0000
        Block_t* m_Block; // 0x0008
        char pad_0010[16]; // 0x0010
    }; // Size: 0x0020

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
        char pad_0000[24]; // 0x0000
        HashFixedData_t* m_pFirst; // 0x0020
        HashFixedData_t* m_pFirstUncommitted; // 0x0020
    }; // Size: 0x0028
    static_assert(sizeof(HashBucket_t) == 0x28);

    CUtlMemoryPool m_EntryMemory;
    char pad_0018[8]; // 0x0018
    HashAllocatedBlob_t* m_unBuckets;
    char pad_0028[72]; // 0x0028
    CThreadSpinRWLock m_ThreadSpinLock; // 0x0070
    std::array<HashBucket_t, BucketCount> m_aBuckets; // 0x0080
    bool m_bNeedsCommit; // 0x2880
    std::int32_t m_ContentionCheck; // 0x2881
};
static_assert(sizeof(CUtlTSHashV2<void*>) == 0x2888);

template <class T, class Keytype, int BucketCount>
std::vector<T> CUtlTSHashV2<T, Keytype, BucketCount>::GetElements(int nFirstElement) {
    int n_count = BlocksAllocated();
    std::vector<T> AllocatedList;
    if (n_count > 0) {
        int nIndex = 0;
        for (int i = 0; i < BucketCount; i++) {
            const HashBucket_t& bucket = m_aBuckets[i];
            for (HashFixedData_t* pElement = bucket.m_pFirstUncommitted; pElement; pElement = pElement->m_pNext) {
                if (--nFirstElement >= 0)
                    continue;

                AllocatedList.emplace_back(pElement->m_Data);
                ++nIndex;

                if (nIndex >= n_count)
                    break;
            }
        }
    }

    /// @note: @og: basically, its hacky-way to obtain first-time commited information to memory
    n_count = AllocatedSize();
    std::vector<T> unAllocatedList;
    if (n_count > 0) {
        auto& unallocated_data = m_unBuckets->m_unAllocatedNext;
        if (unallocated_data != nullptr) {
            if (m_unBuckets->m_unAllocatedData)
                unAllocatedList.emplace_back(m_unBuckets->m_unAllocatedData);

            for (auto unallocated_element = unallocated_data; unallocated_element; unallocated_element = unallocated_element->m_unAllocatedNext) {
                if (unallocated_element->m_unAllocatedData == nullptr)
                    continue;

                unAllocatedList.emplace_back(unallocated_element->m_unAllocatedData);
            }
        }
    }

    return unAllocatedList.size() > AllocatedList.size() ? unAllocatedList : AllocatedList;
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
