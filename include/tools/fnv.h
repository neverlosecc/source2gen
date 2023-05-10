#pragma once
#include <cstdint>
#include <type_traits>

// Implements FNV-1a hash algorithm
namespace detail {
    template <typename Type, Type OffsetBasis, Type Prime>
    struct SizeDependantData {
        using type = Type;

        constexpr static auto k_offset_basis = OffsetBasis;
        constexpr static auto k_prime = Prime;
    };

    template <std::size_t Bits>
    struct SizeSelector : std::false_type { };

    template <>
    struct SizeSelector<32> : SizeDependantData<std::uint32_t, 0x811c9dc5ul, 16777619ul> { };

    template <>
    struct SizeSelector<64> : SizeDependantData<std::uint64_t, 0xcbf29ce484222325ull, 1099511628211ull> { };

    template <std::size_t Size>
    class FnvHash {
    private:
        using data_t = SizeSelector<Size>;
    public:
        using hash = typename data_t::type;
    private:
        constexpr static auto k_offset_basis = data_t::k_offset_basis;
        constexpr static auto k_prime = data_t::k_prime;
    public:
        static __forceinline constexpr auto hash_init() -> hash {
            return k_offset_basis;
        }

        static __forceinline constexpr auto hash_byte(hash current, std::uint8_t byte) -> hash {
            return (current ^ byte) * k_prime;
        }

        static __forceinline constexpr auto hash_constexpr(const char* str, const std::size_t size) -> hash {
            const auto prev_hash = size == 1 ? hash_init() : hash_constexpr(str, size - 1);
            const auto cur_hash = hash_byte(prev_hash, str[size - 1]);
            return cur_hash;
        }

        template <std::size_t N>
        static __forceinline constexpr auto hash_constexpr(const char (&str)[N], //
                                                           const std::size_t size = N - 1 /* do not hash the null */
                                                           ) -> hash {
            return hash_constexpr((const char*)str, size);
        }

        static auto __forceinline hash_runtime_data(const void* data, const size_t sz) -> hash {
            const auto bytes = static_cast<const uint8_t*>(data);

            auto result = hash_init();
            for (auto i = 0ull; i != sz; ++i)
                result = hash_byte(result, bytes[i]);

            return result;
        }

        static auto __forceinline hash_runtime(const char* str) -> hash {
            auto result = hash_init();
            do
                result = hash_byte(result, *str++);
            while (*str != '\0');

            return result;
        }

        static auto __forceinline hash_runtime(const wchar_t* str) -> hash {
            auto result = hash_init();
            do
                result = hash_byte(result, static_cast<char>(*str++));
            while (*str != L'\0');

            return result;
        }

        static auto __forceinline hash_runtime(const char* str, size_t sz) -> hash {
            auto end = str + sz;
            auto result = hash_init();

            do
                result = hash_byte(result, *str++);
            while (str != end);

            return result;
        }

        static auto __forceinline hash_runtime(const wchar_t* str, size_t sz) -> hash {
            auto end = str + sz;
            auto result = hash_init();

            do
                result = hash_byte(result, static_cast<char>(*str++));
            while (str != end);

            return result;
        }
    };
} // namespace detail

using fnv32 = ::detail::FnvHash<32>;
using fnv64 = ::detail::FnvHash<64>;
using fnv = ::detail::FnvHash<sizeof(void*) * 8>;

#define FNV(str) (std::integral_constant<fnv::hash, fnv::hash_constexpr(str)>::value)
#define FNV32(str) (std::integral_constant<fnv32::hash, fnv32::hash_constexpr(str)>::value)
#define FNV64(str) (std::integral_constant<fnv64::hash, fnv64::hash_constexpr(str)>::value)

constexpr fnv32::hash operator"" _fnv32(const char* s, size_t len) {
    return fnv32::hash_constexpr(s, len);
}

constexpr fnv64::hash operator"" _fnv64(const char* s, size_t len) {
    return fnv64::hash_constexpr(s, len);
}
