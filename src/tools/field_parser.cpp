// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#include <Include.h>

namespace field_parser {
    namespace detail {
        namespace {
            using namespace std::string_view_literals;

            constexpr std::string_view kBitfieldTypePrefix = "bitfield:"sv;

            // clang-format off
            constexpr auto kTypeNameToCpp = std::to_array<std::pair<std::string_view, std::string_view>>({
                {"float32"sv, "float"sv}, 
                {"float64"sv, "double"sv},
    
                {"int8"sv, "int8_t"sv},   
                {"int16"sv, "int16_t"sv},   
                {"int32"sv, "int32_t"sv},   
                {"int64"sv, "int64_t"sv},
    
                {"uint8"sv, "uint8_t"sv}, 
                {"uint16"sv, "uint16_t"sv}, 
                {"uint32"sv, "uint32_t"sv}, 
                {"uint64"sv, "uint64_t"sv}
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
            // clang-format on
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

        void parse_bitfield(field_info_t& result, const std::string& type_name) {
            // @note: @es3n1n: in source2 schema, every bitfield var name would start with the "bitfield:" prefix
            // so if there's no such prefix we would just skip the bitfield parsing.
            if (type_name.size() < kBitfieldTypePrefix.size())
                return;

            if (const auto s = type_name.substr(0, kBitfieldTypePrefix.size()); s != kBitfieldTypePrefix.data())
                return;

            // @note: @es3n1n: type_name starts with the "bitfield:" prefix,
            // now we can parse the bitfield size
            const auto bitfield_size_str = type_name.substr(kBitfieldTypePrefix.size(), type_name.size() - kBitfieldTypePrefix.size());
            const auto bitfield_size = wrapped_atoi(bitfield_size_str.data());

            // @note: @es3n1n: saving parsed value
            result.m_bitfield_size = bitfield_size;
            result.m_type = codegen::guess_bitfield_type(bitfield_size);
        }

        // @note: @es3n1n: we are assuming that this function would be executed right after
        // the bitfield/array parsing and the type would be already set if item is a bitfield
        // or array
        //
        void parse_type(field_info_t& result, const std::string& type_name) {
            if (result.m_type.empty())
                result.m_type = type_name;

            // @note: @es3n1n: applying kTypeNameToCpp rules
            for (auto& rule : kTypeNameToCpp) {
                if (result.m_type != rule.first)
                    continue;

                result.m_type = rule.second;
                break;
            }
        }

        // @note: @og: as above just modified for datamaps
        void parse_type(field_info_t& result, const fieldtype_t& type_name) {
            if (result.m_field_type == fieldtype_t::FIELD_UNUSED)
                result.m_field_type = type_name;

            // @note: @es3n1n: applying kTypeNameToCpp rules
            for (auto& rule : kDatamapToCpp) {
                if (result.m_field_type != rule.first)
                    continue;

                result.m_type = rule.second;
                break;
            }
        }
    } // namespace detail

    field_info_t parse(const std::string& type_name, const std::string& name, const std::vector<std::size_t>& array_sizes) {
        field_info_t result = {};
        result.m_name = name;

        std::copy(array_sizes.begin(), array_sizes.end(), std::back_inserter(result.m_array_sizes));

        detail::parse_bitfield(result, type_name);
        detail::parse_type(result, type_name);

        return result;
    }

    field_info_t parse(const fieldtype_t& type_name, const std::string& name, const std::size_t& array_sizes) {
        field_info_t result = {};
        result.m_name = name;

        if (array_sizes > 1)
            result.m_array_sizes.emplace_back(array_sizes);

        detail::parse_type(result, type_name);

        return result;
    }
} // namespace field_parser

// source2gen - Source2 games SDK generator
// Copyright 2023 neverlosecc
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
