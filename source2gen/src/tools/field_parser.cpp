// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include "tools/field_parser.h"
#include "tools/codegen/codegen.h"
#include <algorithm>
#include <array>
#include <bit>
#include <Include.h>
#include <sdk/interfaces/client/game/datamap_t.h>

namespace field_parser {
    namespace detail {
        namespace {
            using namespace std::string_view_literals;

            // The real prefix is "bitfield:", but we're escaping names
            // before parsing them
            constexpr std::string_view kBitfieldTypePrefix = "bitfield_"sv;

            // @note: @es3n1n: a list of possible integral types for bitfields (would be used in `guess_bitfield_type`)
            //
            constexpr auto kBitfieldIntegralTypes = std::to_array<std::pair<std::size_t, std::string_view>>({
                {8, "uint8_t"},
                {16, "uint16_t"},
                {32, "uint32_t"},
                {64, "uint64_t"},
            });

            constexpr auto kDatamapToCpp = std::to_array<std::pair<fieldtype_t, std::string_view>>({
                {fieldtype_t::FIELD_FLOAT32, "float"sv},
                {fieldtype_t::FIELD_TIME, "GameTime_t"sv},
                {fieldtype_t::FIELD_ENGINE_TIME, "float"sv},
                {fieldtype_t::FIELD_FLOAT64, "double"sv},
                {fieldtype_t::FIELD_INT16, "int16_t"sv},
                {fieldtype_t::FIELD_INT32, "int32_t"sv},
                {fieldtype_t::FIELD_INT64, "int64_t"sv},
                {fieldtype_t::FIELD_UINT8, "uint8_t"sv},
                {fieldtype_t::FIELD_UINT16, "uint16_t"sv},
                {fieldtype_t::FIELD_UINT32, "uint32_t"sv},
                {fieldtype_t::FIELD_UINT64, "uint64_t"sv},
                {fieldtype_t::FIELD_BOOLEAN, "bool"sv},
                {fieldtype_t::FIELD_CHARACTER, "char"sv},
                {fieldtype_t::FIELD_VOID, "void"sv},
                {fieldtype_t::FIELD_STRING, "CUtlSymbolLarge"sv},
                {fieldtype_t::FIELD_VECTOR, "Vector"sv},
                {fieldtype_t::FIELD_POSITION_VECTOR, "Vector"sv},
                {fieldtype_t::FIELD_NETWORK_ORIGIN_CELL_QUANTIZED_VECTOR, "Vector"sv},
                {fieldtype_t::FIELD_DIRECTION_VECTOR_WORLDSPACE, "Vector"sv},
                {fieldtype_t::FIELD_NETWORK_QUANTIZED_VECTOR, "Vector"sv},
                {fieldtype_t::FIELD_VECTOR2D, "Vector2D"sv},
                {fieldtype_t::FIELD_VECTOR4D, "Vector4D"sv},
                {fieldtype_t::FIELD_QANGLE, "QAngle"sv},
                {fieldtype_t::FIELD_QANGLE_WORLDSPACE, "QAngle"sv},
                {fieldtype_t::FIELD_QUATERNION, "Quaternion"sv},
                {fieldtype_t::FIELD_CSTRING, "const char *"sv},
                {fieldtype_t::FIELD_UTLSTRING, "CUtlString"sv},
                {fieldtype_t::FIELD_UTLSTRINGTOKEN, "CUtlStringToken"sv},
                {fieldtype_t::FIELD_COLOR32, "Color"sv},
                {fieldtype_t::FIELD_WORLD_GROUP_ID, "WorldGroupId_t"sv},
                {fieldtype_t::FIELD_ROTATION_VECTOR, "RotationVector"sv},
                {fieldtype_t::FIELD_CTRANSFORM_WORLDSPACE, "CTransform"sv},
                {fieldtype_t::FIELD_EHANDLE, "CHandle< CBaseEntity >"sv},
                {fieldtype_t::FIELD_CUSTOM, "void"sv},
                {fieldtype_t::FIELD_HMODEL, "CStrongHandle< InfoForResourceTypeCModel >"sv},
                {fieldtype_t::FIELD_HMATERIAL, "CStrongHandle< InfoForResourceTypeIMaterial2 >"sv},
                {fieldtype_t::FIELD_SHIM, "SHIM"sv},
                {fieldtype_t::FIELD_FUNCTION, "void*"sv},
            });
        } // namespace

        // @note: @es3n1n: basically the same thing as std::atoi
        // but an exception would be thrown if we are unable to parse the string
        //
        int wrapped_atoi(const char* s) {
            const int result = std::atoi(s);

            if (result == 0 && s && s[0] != '0')
                throw std::runtime_error(std::format("{} : Unable to parse '{}'", __FUNCTION__, s));

            return result;
        }

        /// only sets @p result.m_type if @p type_name indicates that this field is a bitfield
        void parse_bitfield(const codegen::IGenerator& generator, field_info_t& result, const std::string& type_name) {
            // @note: @es3n1n: in source2 schema, every bitfield var name would start with the "bitfield:" prefix
            // so if there's no such prefix we would just skip the bitfield parsing.
            if (type_name.size() < kBitfieldTypePrefix.size())
                return;

            if (const auto s = type_name.substr(0, kBitfieldTypePrefix.size()); s != kBitfieldTypePrefix.data())
                return;

            // @note: @es3n1n: type_name starts with the "bitfield:" prefix,
            // now we can parse the bitfield size
            const auto bitfield_size_str = type_name.substr(kBitfieldTypePrefix.size(), type_name.size() - kBitfieldTypePrefix.size());
            const auto bitfield_size = static_cast<unsigned int>(wrapped_atoi(bitfield_size_str.data()));

            // @note: @es3n1n: saving parsed value
            result.m_bitfield_size = bitfield_size;
            result.m_type = generator.get_uint(std::max(8u, std::bit_ceil(bitfield_size)));
        }

        // @note: @es3n1n: we are assuming that this function would be executed right after
        // @ref parse_bitfield() and the type would be already set if item is a bitfield
        // or array
        //
        void parse_type(const codegen::IGenerator& generator, field_info_t& result, const std::string& type_name) {
            auto [unwrapped_name, unwrapped_pointers] = split_type_name_pointers(type_name);

            if (const auto found = generator.find_built_in(unwrapped_name); found.has_value()) {
                result.m_type = found.value() + unwrapped_pointers;
            } else {
                // result.m_type may already be set if parse_bitfield() identified a bitfield
                if (result.m_type.empty()) {
                    result.m_type = type_name;
                }
            }
        }

        // @note: @og: as above just modified for datamaps
        void parse_type(field_info_t& result, const fieldtype_t& type_name) {
            if (result.m_field_type == fieldtype_t::FIELD_UNUSED)
                result.m_field_type = type_name;

            // @note: @es3n1n: applying kDatamapToCpp rules
            for (auto& rule : kDatamapToCpp) {
                if (result.m_field_type != rule.first)
                    continue;

                result.m_type = rule.second;
                break;
            }
        }
    } // namespace detail

    std::pair<std::string, std::string> split_type_name_pointers(const std::string& type_name) {
        const auto pos = type_name.find('*');
        const auto base = (pos == std::string::npos) ? type_name : type_name.substr(0, pos);
        const auto pointers = (pos == std::string::npos) ? "" : type_name.substr(pos);

        return std::make_pair(base, pointers);
    }

    std::string guess_bitfield_type(const std::size_t bits_count) {
        for (auto p : detail::kBitfieldIntegralTypes) {
            if (bits_count > p.first)
                continue;

            return p.second.data();
        }

        throw std::runtime_error(std::format("{} : Unable to guess bitfield type with size {}", __FUNCTION__, bits_count));
    }

    field_info_t parse(const codegen::IGenerator& generator, const std::string& type_name, const std::string& name,
                       const std::vector<std::size_t>& array_sizes) {
        field_info_t result = {};
        result.m_name = name;

        std::copy(array_sizes.begin(), array_sizes.end(), std::back_inserter(result.m_array_sizes));

        detail::parse_bitfield(generator, result, type_name);
        detail::parse_type(generator, result, type_name);

        return result;
    }

    field_info_t parse(fieldtype_t field_type, const std::string& name, std::size_t array_sizes) {
        field_info_t result = {};
        result.m_name = name;

        if (array_sizes > 1)
            result.m_array_sizes.emplace_back(array_sizes);

        detail::parse_type(result, field_type);

        return result;
    }
} // namespace field_parser

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
