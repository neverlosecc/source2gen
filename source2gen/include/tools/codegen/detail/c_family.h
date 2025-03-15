#pragma once

#include <absl/strings/str_join.h>
#include <absl/strings/str_split.h>
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
        {"uint64", "uint64_t"} // clang-format on
    });

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
        }
    }

    /**
     * @param name Unqualified. If this is qualified, namespace separators will be replaced with underscores.
     * @return A name that can be used to name entities
     */
    [[nodiscard]]
    inline std::string escape_name(std::string_view name) {
        std::string result;
        result.resize(name.size());

        for (std::size_t i = 0; i < name.size(); i++)
            result[i] =
                std::ranges::find(detail::c_family::kBlacklistedCharacters, name[i]) == std::end(detail::c_family::kBlacklistedCharacters) ? name[i] : '_';

        const auto starts_with_underscore{!result.empty() && result[0] == '_'};

        // collapse multiple underscores into one
        // because names containing double underscores are reserved in C
        result = absl::StrJoin(absl::StrSplit(result, '_', absl::SkipWhitespace{}), "_");

        // restore underscore if we had one at the beginning, e.g. "_pad_10"
        if (starts_with_underscore)
            result = '_' + result;

        return result;
    }
} // namespace codegen::detail::c_family
