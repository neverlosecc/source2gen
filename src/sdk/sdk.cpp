#include <filesystem>
#include "sdk/sdk.h"

namespace sdk
{
    CSchemaSystem* g_schema = nullptr;

    void GenerateTypeScopeSdk(CSchemaSystemTypeScope* current)
    {
        auto ends_with = [](const std::string& str, const std::string& suffix)
        {
            return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
        };

        constexpr std::string_view dll  = ".dll";
        auto                       name = current->GetScopeName();
        if (ends_with(name.data(), dll.data())) name.remove_suffix(dll.size());

        fmt::print("{}\n", name);

        std::filesystem::create_directory("sdk/");

        auto out = fmt::output_file(fmt::format("SDK\\{}.hpp", name));

        out.print("#pragma once\n#include <cstdint>\n#include \"!GlobalTypes.hpp\"\n\n");

        out.print("///////////////////////////////////////////\n");
        out.print("// Binary: {}\n", current->GetScopeName());
        auto classes = current->GetClasses();
        out.print("// Class Count: {}\n", classes.Count());
        auto enums = current->GetEnums();
        out.print("// Enum Count: {}\n", enums.Count());
        out.print("///////////////////////////////////////////\n\n");

        for (auto schema_enum_binding : enums.GetElements())
        {
            auto type = [schema_enum_binding]()
            {
                std::string type_storage;

                switch (schema_enum_binding->m_align_)
                {
                    case 1: type_storage = "uint8_t";
                        break;
                    case 2: type_storage = "uint16_t";
                        break;
                    case 4: type_storage = "uint32_t";
                        break;
                    case 8: type_storage = "uint64_t";
                        break;
                    default: type_storage = "INVALID_TYPE";
                }

                return type_storage;
            };

            if (schema_enum_binding->m_flags_) out.print("// Flags: MEnumFlagsWithOverlappingBits\n");

            out.print("// Aligment: {}\n// Size: {}\n", schema_enum_binding->m_align_, schema_enum_binding->m_size_);

            out.print("enum class {} : {}\n{}\n", schema_enum_binding->m_binding_name_, type(), "{");
            for (auto l = 0; l < schema_enum_binding->m_size_; l++)
            {
                auto& field = schema_enum_binding->m_enum_info_[l];
                if (schema_enum_binding->m_align_ >= 4) out.print("\t{} = {:#x},\n", field.m_name, field.m_value);
                else
                    out.print("\t{} = {},\n", field.m_name,
                              field.m_value == std::numeric_limits<std::size_t>::max() ? -1 : field.m_value);
            }
            out.print("{}\n\n", "};");
        }

        for (const auto schema_class_binding : classes.GetElements())
        {
            if (!schema_class_binding)
                continue;
            const auto class_info = current->FindDeclaredClass(schema_class_binding->m_binary_name);

            out.print("// Aligment: {}\n// Size: {}\n", class_info->m_align, class_info->m_size);

            std::string class_type = "class";
            if (ends_with(class_info->m_name, "_t")) class_type = "struct";

            if (class_info->m_schema_parent)
            {
                std::vector<std::string_view> parents;

                auto parent = class_info->m_schema_parent->m_class;
                do
                {
                    parents.emplace_back(parent->m_name);

                    if (parent->m_schema_parent) parent = parent->m_schema_parent->m_class;
                    else parent                         = nullptr;
                }
                while (parent != nullptr);

                out.print("{} {} : public {}\n{}\npublic:\n", class_type, class_info->m_name,
                          fmt::join(parents, ", "), "{");
            }
            else out.print("{} {}\n{}\npublic:\n", class_type, class_info->m_name, "{");

            if (!class_info->m_align)
            {
                out.print("// <no members described>\n{}\n\n", "};");
                continue;
            }

            auto get_type_name = [&](const CSchemaType* type)
            {
                std::string type_name = type->m_name_;

                for (const auto& [original_name, cpp_name] : s_typedefs)
                    if (original_name == type_name)
                        type_name =
                                cpp_name;

                return type_name;
            };

            for (auto k = 0; k < class_info->m_align; k++)
            {
                const auto field = reinterpret_cast<SchemaClassFieldData_t*>(class_info->m_fields + k * 0x20);
                if (!field) continue;

                auto get_metadata_type = [&](SchemaMetadataEntryData_t metadata_entry)
                {
                    const auto name_hash = fnv32(metadata_entry.m_name);

                    std::string value;
                    switch (name_hash)
                    {
                        case fnv32("MNetworkChangeCallback"):
                        case fnv32("MPropertyFriendlyName"):
                        case fnv32("MPropertyDescription"):
                        case fnv32("MPropertyAttributeRange"):
                        case fnv32("MPropertyStartGroup"):
                        case fnv32("MPropertyAttributeChoiceName"):
                        case fnv32("MPropertyGroupName"):
                        case fnv32("MNetworkUserGroup"):
                        case fnv32("MNetworkAlias"):
                        case fnv32("MNetworkTypeAlias"):
                        case fnv32("MNetworkSerializer"):
                        case fnv32("MPropertyAttributeEditor"):
                        case fnv32("MPropertySuppressExpr"):
                        case fnv32("MKV3TransferName"):
                            {
                                value = fmt::to_string(metadata_entry.m_value->m_sz_value);
                                break;
                            }
                        case fnv32("MNetworkVarEmbeddedFieldOffsetDelta"):
                        case fnv32("MNetworkBitCount"):
                        case fnv32("MNetworkPriority"):
                        case fnv32("MPropertySortPriority"):
                            {
                                value = fmt::to_string(metadata_entry.m_value->m_n_value);
                                break;
                            }
                        case fnv32("MNetworkMinValue"):
                        case fnv32("MNetworkMaxValue"):
                            {
                                value = fmt::to_string(metadata_entry.m_value->m_f_value);
                                break;
                            }
                        default: break;
                    }

                    return value;
                };

                for (auto j = 0; j < field->m_metadata_size; j++)
                {
                    auto field_metadata = field->m_metadata[j];
                    if (auto data = get_metadata_type(field_metadata); data.empty())
                        out.print(
                                  "\t// {}\n", field_metadata.m_name, data);
                    else out.print("\t// {} \"{}\"\n", field_metadata.m_name, data);
                }

                out.print("\t{} {}; // {:#x}\n", get_type_name(field->m_type), field->m_name,
                          field->m_single_inheritance_offset);
            }

            if (class_info->m_static_size)
            {
                out.print("public:\n");
                for (auto s = 0; s < class_info->m_static_size; s++)
                {
                    auto [name, m_type, m_instance, pad_0x0018] = class_info->m_static_fiels[s];

                    out.print(
                              "\tstatic {} &Get_{}() {} return *reinterpret_cast<{}*>(interfaces::g_schema->FindTypeScopeForModule(\"{}\")->FindDeclaredClass(\"{}\")->m_static_fiels[{}]->m_instance); {}\n",
                              get_type_name(m_type), name,
                              "{",
                              get_type_name(m_type), current->GetScopeName(),
                              class_info->m_name, s,
                              "}"
                             );
                }
            }

            out.print("{}\n\n", "};");
        }
    }
}
