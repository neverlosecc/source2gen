// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

class CThreadSpinRWLock {
public:
    struct LockInfo_t {
        std::uint32_t m_writerId;
        std::int32_t m_nReaders;
    };

public:
    void* m_pThreadSpin;
    LockInfo_t m_lockInfo;
    const char* m_pszDebugName;
};
static_assert(sizeof(CThreadSpinRWLock) == 0x18);

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
