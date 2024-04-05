// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

//-----------------------------------------------------------------------------
//
// A super-fast thread-safe integer A simple class encapsulating the notion of an
// atomic integer used across threads that uses the built in and faster
// "interlocked" functionality rather than a full-blown mutex. Useful for simple
// things like reference counts, etc.
//
//-----------------------------------------------------------------------------

template <typename T>
class CInterlockedIntT {
public:
    CInterlockedIntT(): m_value(0) {
        static_assert(sizeof(T) == sizeof(std::int32_t));
    }
    CInterlockedIntT(T value): m_value(value) { }

    T operator()(void) const {
        return m_value;
    }
    operator T() const {
        return m_value;
    }

    bool operator!() const {
        return (m_value == 0);
    }
    bool operator==(T rhs) const {
        return (m_value == rhs);
    }
    bool operator!=(T rhs) const {
        return (m_value != rhs);
    }

    T Get() {
        return m_value;
    }

private:
    volatile T m_value;
};

using CInterlockedInt = CInterlockedIntT<int>;
using CInterlockedUInt = CInterlockedIntT<unsigned>;

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
