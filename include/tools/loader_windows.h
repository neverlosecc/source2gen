#include <cassert>
#include <string>
#include <string_view>
#include <windows.h>

// keep in sync with get_module_file_name()
#define LOADER_WINDOWS_GET_MODULE_FILE_NAME(name) name ".dll"

namespace Loader::Windows {
    using module_handle_t = HMODULE;

    // keep in sync with LOADER_WINDOWS_GET_MODULE_FILE_NAME
    [[nodiscard]] inline auto get_module_file_name(std::string name) -> std::string {
        return name.append(".dll");
    }

    [[nodiscard]] inline auto find_module_handle(std::string_view name) -> void* {
        return GetModuleHandleA(name.data());
    }

    [[nodiscard]] inline auto load_module(std::string_view name) -> void* {
        return LoadLibraryA(name.data());
    }

    [[nodiscard]] inline auto find_module_symbol(module_handle_t handle, std::string_view name) -> void* {
        assert(handle != nullptr);
        return GetProcAddress(handle, name.data());
    }
} // namespace Loader::Windows
