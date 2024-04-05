// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

class CUtlString {
public:
    [[nodiscard]] const char* Get() const {
        return reinterpret_cast<const char*>(m_Memory.m_pMemory);
    }

    CUtlMemory<std::uint8_t> m_Memory;
    int m_nActualLength;
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
