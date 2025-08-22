// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

/// Some environments automatically define the bare macro `linux` (in
/// addition to the portable feature-test macro `__linux__`). This pollutes 
/// the global namespace and collides with our `platform::linux`.
#if defined(linux)
    #undef linux
#endif

#define WINDOWS 0
#define LINUX 1

/// Names copied from Rust https://doc.rust-lang.org/reference/conditional-compilation.html#target_os
#if defined(_WIN32)
    #define TARGET_OS WINDOWS
#elif defined(__linux__)
    #define TARGET_OS LINUX
#else
    #error unsupported operating system
#endif

// we're using __VA_ARGS__ to allow the use of commas, e.g. IF_WINDOWS(element,)
#if TARGET_OS == WINDOWS
    #define IF_WINDOWS(...) __VA_ARGS__
    #define IF_LINUX(...)
#elif TARGET_OS == LINUX
    #define IF_WINDOWS(...)
    #define IF_LINUX(...) __VA_ARGS__
#endif

enum class platform {
    windows,
    linux,
};

constexpr platform current_platform =
#if TARGET_OS == WINDOWS
    platform::windows;
#elif TARGET_OS == LINUX
    platform::linux;
#else
    #error
#endif

template <class Value>
struct [[nodiscard]] platform_specific {
    Value windows;
    Value linux;

    [[nodiscard]] consteval auto get() {
        switch (current_platform) {
        case platform::windows:
            return this->windows;
        case platform::linux:
            return this->linux;
        }
    }

    /* implicit */ [[nodiscard]] consteval operator Value() {
        return this->get();
    }
};

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
