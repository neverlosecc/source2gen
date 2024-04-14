#pragma once

#include <dlfcn.h>

// TODO: move this code or rename this file, this is unrelated to proc
#define LIBRARY(base_name) "lib" base_name ".so"

using HMODULE = void*;

[[nodiscard]] inline auto GetModuleHandleA(char const* name) -> void* {
    return dlopen(name, RTLD_LAZY | RTLD_NOLOAD);
}

[[nodiscard]] inline auto LoadLibraryA(char const* name) -> void* {
    return dlopen(name, RTLD_LAZY);
}

[[nodiscard]] inline auto GetProcAddress(void* handle, char const* name) -> void* {
    return dlsym(handle, name);
}