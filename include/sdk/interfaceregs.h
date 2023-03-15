#pragma once

namespace sdk
{
    using InstantiateInterfaceFn = void* (*)();

    class InterfaceReg
    {
    public:
        InstantiateInterfaceFn m_create_fn_;
        const char*            m_name_;
        InterfaceReg*          m_next_;
    };

    inline const InterfaceReg* GetInterfaces(const char* library)
    {
        const auto library_handle = GetModuleHandleA(library);

        const auto createinterface_symbol = reinterpret_cast<std::uintptr_t>(
            GetProcAddress(library_handle, "CreateInterface")
        );

        const uintptr_t interface_list = createinterface_symbol + *reinterpret_cast<int32_t*>(
            createinterface_symbol + 3
        ) + 7;

        return *reinterpret_cast<InterfaceReg**>(interface_list);
    }

    template <typename T = void*>
    T* GetInterface(const char* library, const char* partial_version)
    {
        for (const InterfaceReg* current = GetInterfaces(library); current; current = current->m_next_)
        {
            if (std::string_view(current->m_name_).find(partial_version) != std::string_view::npos)
                return reinterpret_cast<T*>(
                    current->m_create_fn_()
                );
        }

        return nullptr;
    }
}
