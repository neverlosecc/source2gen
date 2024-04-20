
#pragma once

#include <string>
#include <string_view>
#include <tools/platform.h>

#if TARGET_OS == WINDOWS
    #include "loader_windows.h"
#elif TARGET_OS == LINUX
    #include "loader_linux.h"
#endif

// TODO: incomplete comment, remove
/// Prefer using Loader::get_module_file_name(). Only use these macros if you
/// cannot allocate memory
#if TARGET_OS == WINDOWS
    #define LOADER_GET_MODULE_FILE_NAME(expr) LOADER_WINDOWS_GET_MODULE_FILE_NAME(expr)
#elif TARGET_OS == LINUX
    #define LOADER_GET_MODULE_FILE_NAME(expr) LOADER_LINUX_GET_MODULE_FILE_NAME(expr)
#endif

/// Wrapping the functions in the `Platform` namespace causes compiler errors
/// even when they're unused in the code base (Fail fast). It also provides a
/// uniform interface for the caller.
namespace Loader {
#if TARGET_OS == WINDOWS
    namespace Platform = Windows;
#elif TARGET_OS == LINUX
    namespace Platform = Linux;
#endif

    using module_handle_t = Platform::module_handle_t;

    [[nodiscard]] inline auto get_module_file_name(std::string name) -> std::string {
        return Platform::get_module_file_name(std::move(name));
    }

    [[nodiscard]] inline auto find_module_handle(std::string_view name) -> module_handle_t {
        return Platform::find_module_handle(name);
    }

    [[nodiscard]] inline auto load_module(std::string_view name) -> module_handle_t {
        return Platform::load_module(name);
    }

    [[nodiscard]] inline auto find_module_symbol(module_handle_t handle, std::string_view name) -> void* {
        return Platform::find_module_symbol(handle, name);
    }
} // namespace Loader