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