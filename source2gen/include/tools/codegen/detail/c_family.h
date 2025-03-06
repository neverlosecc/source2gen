#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <format>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace codegen::detail::c_family {
    constexpr std::array kBlacklistedCharacters = {':', ';', '\\', '/'};

    // @note: @es3n1n: a list of possible integral types for bitfields (would be used in `guess_bitfield_type`)
    //
    constexpr auto kBitfieldIntegralTypes = std::to_array<std::pair<std::size_t, std::string_view>>({
        // clang-format off
        {8, "uint8_t"},
        {16, "uint16_t"},
        {32, "uint32_t"},
        {64, "uint64_t"},

        // @todo: @es3n1n: define uint128_t/uint256_t/... as custom structs in the very beginning of the file
        {128, "uint128_t"},
        {256, "uint256_t"},
        {512, "uint512_t"},
        // clang-format on
    });

    /// Without a namespace qualifier.
    /// Key is a @ref CSchemaType::m_pszName.
    constexpr auto kNumericTypes = std::to_array<std::pair<std::string_view, std::string_view>>({
        // clang-format off
        {"float32", "float"},
        {"float64", "double"},

        {"int8", "int8_t"},
        {"int16", "int16_t"},
        {"int32", "int32_t"},
        {"int64", "int64_t"},

        {"uint8", "uint8_t"},
        {"uint16", "uint16_t"},
        {"uint32", "uint32_t"},
        {"uint64", "uint64_t"}
        // clang-format on
    });

    // TOOD: remove?
    [[nodiscard]]
    inline std::string guess_bitfield_type(const std::size_t bits_count) {
        for (const auto& p : kBitfieldIntegralTypes) {
            if (bits_count > p.first)
                continue;

            return std::string{p.second};
        }

        throw std::runtime_error(std::format("{} : Unable to guess bitfield type with size {}", __FUNCTION__, bits_count));
    }

    /**
     * Panics on error
     * @return Without namespace qualifier
     */
    [[nodiscard]]
    inline std::string get_uint(const std::size_t bits_count) {
        const auto found = std::ranges::find_if(kBitfieldIntegralTypes, [=](const auto& e) { return std::get<0>(e) == bits_count; });

        if (found != kBitfieldIntegralTypes.end()) {
            return std::string{found->second};
        } else {
            throw std::runtime_error(std::format("{} : Unable to guess bitfield type with size {}", __FUNCTION__, bits_count));
            // TOOD: replace all assert+abort with throw
            assert(false && "unsupported bits_count, see kBitfieldIntegralTypes for supported types");
            std::abort();
        }
    }
} // namespace codegen::detail::c_family
