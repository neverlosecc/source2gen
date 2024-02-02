// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

template <typename S, typename T>
class CUtlMap {
public:
    auto begin() noexcept {
        return base_;
    }

    auto end() noexcept {
        return base_ + size_;
    }

    [[nodiscard]] auto begin() const {
        return base_;
    }

    [[nodiscard]] auto end() const {
        return base_ + size_;
    }

    T& operator[](int i) {
        return base_[i];
    }

    S& Count() const;
    S& Count();

    T& Element(int i);
    const T& Element(int i) const;
private:
    T* base_ = nullptr;
    S size_ = 0;
};

template <typename S, typename T>
inline S& CUtlMap<S, T>::Count() const {
    return size_;
}

template <typename S, typename T>
inline S& CUtlMap<S, T>::Count() {
    return size_;
}

template <typename S, typename T>
inline T& CUtlMap<S, T>::Element(int i) {
    assert(i < size_);
    return base_[i];
}

template <typename S, typename T>
const T& CUtlMap<S, T>::Element(int i) const {
    assert(i < size_);
    return base_[i];
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
