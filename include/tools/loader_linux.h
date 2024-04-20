#include <cassert>
#include <dlfcn.h>
#include <string>
#include <string_view>

// keep in sync with get_module_file_name()
#define LOADER_LINUX_GET_MODULE_FILE_NAME(name) "lib" name ".so"

namespace Loader::Linux {
    using module_handle_t = void*;

    // keep in sync with LOADER_LINUX_GET_MODULE_FILE_NAME
    [[nodiscard]] inline auto get_module_file_name(std::string name) -> std::string {
        return name.insert(0, "lib").append(".so");
    }

    [[nodiscard]] inline auto find_module_handle(std::string_view name) -> void* {
        return dlopen(name.data(), RTLD_LAZY | RTLD_NOLOAD);
    }

    [[nodiscard]] inline auto load_module(std::string_view name) -> void* {
        return dlopen(name.data(), RTLD_LAZY);
    }

    [[nodiscard]] inline auto find_module_symbol(module_handle_t handle, std::string_view name) -> void* {
        assert(handle != nullptr && "If you need RTLD_DEFAULT, write a new function to avoid magic values. Most of the time when handle=nullptr, a "
                                    "developer made a mistake and we want to catch that.");
        return dlsym(handle, name.data());
    }
} // namespace Loader::Linux