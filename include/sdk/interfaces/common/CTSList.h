// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include <emmintrin.h>

// decls for aligning data
#define DECL_ALIGN(x) __declspec(align(x))

#ifdef _WIN64
constexpr auto TSLIST_HEAD_ALIGNMENT = 16;
constexpr auto TSLIST_NODE_ALIGNMENT = 16;
#else
constexpr auto TSLIST_HEAD_ALIGNMENT = 8;
constexpr auto TSLIST_NODE_ALIGNMENT = 8;
#endif

#ifdef _MSC_VER
    #define TSLIST_HEAD_ALIGN DECL_ALIGN(TSLIST_HEAD_ALIGNMENT)
    #define TSLIST_NODE_ALIGN DECL_ALIGN(TSLIST_NODE_ALIGNMENT)
    #define TSLIST_HEAD_ALIGN_POST
    #define TSLIST_NODE_ALIGN_POST
#elif defined(GNUC)
    #define TSLIST_HEAD_ALIGN
    #define TSLIST_NODE_ALIGN
    #define TSLIST_HEAD_ALIGN_POST DECL_ALIGN(TSLIST_HEAD_ALIGNMENT)
    #define TSLIST_NODE_ALIGN_POST DECL_ALIGN(TSLIST_NODE_ALIGNMENT)
#else
    #error
#endif

struct TSLIST_NODE_ALIGN TSLNodeBase_t {
    TSLNodeBase_t* Next; // name to match Windows
} TSLIST_NODE_ALIGN_POST;

typedef __m128i int128;

union TSLIST_HEAD_ALIGN TSLHead_t {
    struct Value_t {
        TSLNodeBase_t* Next;
        std::int16_t Depth;
        std::int16_t Sequence;
        std::int32_t Padding;
    } value;

    struct Value32_t {
        TSLNodeBase_t* Next_do_not_use_me;
        std::int32_t DepthAndSequence;
    } value32;

    int128 value64x128;
} TSLIST_HEAD_ALIGN_POST;

class TSLIST_HEAD_ALIGN CTSListBase {
public:
    int Count() {
        return m_Head.value.Depth;
    }

public:
    TSLHead_t m_Head;
} TSLIST_HEAD_ALIGN_POST;

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
