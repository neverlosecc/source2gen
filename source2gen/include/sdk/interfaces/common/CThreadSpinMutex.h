// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include "CThreadMutex.h"
#include <type_traits>

#if defined(CS2) || defined(DOTA2) || defined(DEADLOCK)
constexpr auto kThreadSpinMutex = 2;
#else
constexpr auto kThreadSpinMutex = 1;
#endif

class CThreadSpinMutexV1 {
public:
    CThreadSpinMutexV1(const char* pDebugName = NULL): m_ownerID(0), m_depth(0), m_pszDebugName(pDebugName) { }

private:
    volatile ThreadId_t m_ownerID;
    int m_depth;
    const char* m_pszDebugName;
};
static_assert(sizeof(CThreadSpinMutexV1) == platform_specific{.windows = 0x10, .linux = 0x18});

class CThreadSpinMutexV2 {
public:
    CThreadSpinMutexV2([[maybe_unused]] const char* pDebugName = NULL): m_ownerID(0), m_depth(0) { }

private:
    volatile ThreadId_t m_ownerID;
    int m_depth;
};
static_assert(sizeof(CThreadSpinMutexV2) == platform_specific{.windows = 0x8, .linux = 0x10});

using CThreadSpinMutex = std::conditional_t<kThreadSpinMutex == 1, CThreadSpinMutexV1, CThreadSpinMutexV2>;
using CThreadFastMutex = CThreadSpinMutex;

// source2gen - Source2 games SDK generator
// Copyright 2024 neverlosecc
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
