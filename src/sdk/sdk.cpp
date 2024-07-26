// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.

// ReSharper disable CppClangTidyClangDiagnosticLanguageExtensionToken
#include "sdk/sdk.h"
#include "tools/codegen/c.h"
#include "tools/codegen/codegen.h"
#include "tools/codegen/cpp.h"
#include <cstdlib>
#include <filesystem>
#include <list>
#include <string_view>
#include <unordered_set>

namespace {
    using namespace std::string_view_literals;

    constexpr std::string_view kOutDirName = "sdk"sv;

    constexpr uint32_t kMaxReferencesForClassEmbed = 2;
    constexpr std::size_t kMinFieldCountForClassEmbed = 2;
    constexpr std::size_t kMaxFieldCountForClassEmbed = 12;

    constinit std::array string_metadata_entries = {
        FNV32("MNetworkChangeCallback"),
        FNV32("MPropertyFriendlyName"),
        FNV32("MPropertyDescription"),
        FNV32("MPropertyAttributeRange"),
        FNV32("MPropertyStartGroup"),
        FNV32("MPropertyAttributeChoiceName"),
        FNV32("MPropertyGroupName"),
        FNV32("MNetworkUserGroup"),
        FNV32("MNetworkAlias"),
        FNV32("MNetworkTypeAlias"),
        FNV32("MNetworkSerializer"),
        FNV32("MPropertyAttributeEditor"),
        FNV32("MPropertySuppressExpr"),
        FNV32("MKV3TransferName"),
        FNV32("MFieldVerificationName"),
        FNV32("MVectorIsSometimesCoordinate"),
        FNV32("MNetworkEncoder"),
        FNV32("MPropertyCustomFGDType"),
        FNV32("MPropertyCustomEditor"),
        FNV32("MVDataUniqueMonotonicInt"),
        FNV32("MScriptDescription"),
        FNV32("MPropertyAttributeSuggestionName"),
        FNV32("MPropertyIconName"),
        FNV32("MVDataOutlinerIcon"),
        FNV32("MPropertyExtendedEditor"),
        FNV32("MParticleReplacementOp"),
        FNV32("MCustomFGDMetadata"),
        FNV32("MCellForDomain"),
        FNV32("MSrc1ImportDmElementType"),
        FNV32("MSrc1ImportAttributeName"),
        FNV32("MResourceBlockType"),
        FNV32("MVDataOutlinerIconExpr"),
        FNV32("MPropertyArrayElementNameKey"),
        FNV32("MPropertyFriendlyName"),
        FNV32("MPropertyDescription"),
        FNV32("MNetworkExcludeByName"),
        FNV32("MNetworkExcludeByUserGroup"),
        FNV32("MNetworkIncludeByName"),
        FNV32("MNetworkIncludeByUserGroup"),
        FNV32("MNetworkUserGroupProxy"),
        FNV32("MNetworkReplayCompatField"),
        FNV32("MPulseProvideFeatureTag"),
        FNV32("MPulseEditorHeaderIcon"),
    };

    constinit std::array string_class_metadata_entries = {
        FNV32("MResourceTypeForInfoType"),
        FNV32("MDiskDataForResourceType"),
    };

    constinit std::array var_name_string_class_metadata_entries = {
        FNV32("MNetworkVarNames"),          FNV32("MNetworkOverride"),   FNV32("MNetworkVarTypeOverride"),
        FNV32("MPulseCellOutflowHookInfo"), FNV32("MScriptDescription"), FNV32("MParticleDomainTag"),
    };

    constinit std::array integer_metadata_entries = {
        FNV32("MNetworkVarEmbeddedFieldOffsetDelta"),
        FNV32("MNetworkBitCount"),
        FNV32("MNetworkPriority"),
        FNV32("MParticleOperatorType"),
        FNV32("MPropertySortPriority"),
        FNV32("MParticleMinVersion"),
        FNV32("MParticleMaxVersion"),
        FNV32("MNetworkEncodeFlags"),
        FNV32("MResourceVersion"),
        FNV32("MVDataNodeType"),
        FNV32("MVDataOverlayType"),
        FNV32("MAlignment"),
        FNV32("MGenerateArrayKeynamesFirstIndex"),
    };

    constinit std::array float_metadata_entries = {
        FNV32("MNetworkMinValue"),
        FNV32("MNetworkMaxValue"),
    };

    // @note: @es3n1n: some more utils
    //
    std::string GetMetadataValue(const SchemaMetadataEntryData_t metadata_entry) {
        std::string value;

        const auto value_hash_name = fnv32::hash_runtime(metadata_entry.m_szName);

        // clang-format off
        if (std::ranges::find(var_name_string_class_metadata_entries, value_hash_name) != var_name_string_class_metadata_entries.end())
        {
            const auto &var_value = metadata_entry.m_pNetworkValue->m_VarValue;
            if (var_value.m_pszType && var_value.m_pszName)
                value = std::format("{} {}", var_value.m_pszType, var_value.m_pszName);
            else if (var_value.m_pszName && !var_value.m_pszType)
                value = var_value.m_pszName;
            else if (!var_value.m_pszName && var_value.m_pszType)
                value = var_value.m_pszType;
        }
        else if (std::ranges::find(string_class_metadata_entries, value_hash_name) != string_class_metadata_entries.end())
        {
            auto clean_string = [](const std::string_view& input) {
                std::string result;
                for (const char &ch : input) {
                    if (std::isalpha(static_cast<unsigned char>(ch))) {
                        result += ch;
                    } else {
                        break;
                    }
                }
                return result;
            };

            value = clean_string(metadata_entry.m_pNetworkValue->m_szValue.data());
        }
        else if (std::ranges::find(string_metadata_entries, value_hash_name) != string_metadata_entries.end())
            value = metadata_entry.m_pNetworkValue->m_pszValue;
        else if (std::ranges::find(integer_metadata_entries, value_hash_name) != integer_metadata_entries.end())
            value = std::to_string(metadata_entry.m_pNetworkValue->m_nValue);
        else if (std::ranges::find(float_metadata_entries, value_hash_name) != float_metadata_entries.end())
            value = std::to_string(metadata_entry.m_pNetworkValue->m_fValue);
        // clang-format on

        return value;
    };
} // namespace

namespace sdk {
    namespace {
        void PrintClassInfo(codegen::IGenerator::self_ref builder, CSchemaClassBinding* class_info) {
            builder.comment(std::format("Alignment: {}", class_info->GetAligment())).comment(std::format("Size: {:#x}", class_info->m_nSizeOf));

            if ((class_info->m_nClassFlags & SCHEMA_CF1_HAS_VIRTUAL_MEMBERS) != 0) // @note: @og: its means that class probably does have vtable
                builder.comment("Has VTable");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_IS_ABSTRACT) != 0)
                builder.comment("Is Abstract");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_HAS_TRIVIAL_CONSTRUCTOR) != 0)
                builder.comment("Has Trivial Constructor");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_HAS_TRIVIAL_DESTRUCTOR) != 0)
                builder.comment("Has Trivial Destructor");

#if defined(CS2) || defined(DOTA2)
            if ((class_info->m_nClassFlags & SCHEMA_CF1_CONSTRUCT_ALLOWED) != 0)
                builder.comment("Construct allowed");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_CONSTRUCT_DISALLOWED) != 0)
                builder.comment("Construct disallowed");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_INFO_TAG_MConstructibleClassBase) != 0)
                builder.comment("MConstructibleClassBase");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_INFO_TAG_MClassHasCustomAlignedNewDelete) != 0)
                builder.comment("MClassHasCustomAlignedNewDelete");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_INFO_TAG_MClassHasEntityLimitedDataDesc) != 0)
                builder.comment("MClassHasEntityLimitedDataDesc");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_INFO_TAG_MDisableDataDescValidation) != 0)
                builder.comment("MDisableDataDescValidation");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_INFO_TAG_MIgnoreTypeScopeMetaChecks) != 0)
                builder.comment("MIgnoreTypeScopeMetaChecks");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_INFO_TAG_MNetworkNoBase) != 0)
                builder.comment("MNetworkNoBase");
            if ((class_info->m_nClassFlags & SCHEMA_CF1_INFO_TAG_MNetworkAssumeNotNetworkable) != 0)
                builder.comment("MNetworkAssumeNotNetworkable");
#endif

            if (class_info->m_nStaticMetadataSize > 0)
                builder.comment("");

            for (const auto& metadata : class_info->GetStaticMetadata()) {
                if (const auto value = GetMetadataValue(metadata); !value.empty())
                    builder.comment(std::format("{} \"{}\"", metadata.m_szName, value));
                else
                    builder.comment(metadata.m_szName);
            }
        }

        void PrintEnumInfo(codegen::IGenerator::self_ref builder, const CSchemaEnumBinding* enum_binding) {
            builder.comment(std::format("Enumerator count: {}", enum_binding->m_nEnumeratorCount))
                .comment(std::format("Alignment: {}", enum_binding->m_unAlignOf))
                .comment(std::format("Size: {:#x}", enum_binding->m_unSizeOf));

            if (enum_binding->m_nStaticMetadataSize > 0)
                builder.comment("");

            for (const auto& metadata : enum_binding->GetStaticMetadata()) {
                builder.comment(metadata.m_szName);
            }
        }

        void AssembleEnums(codegen::IGenerator::self_ref builder, const std::unordered_set<const CSchemaEnumBinding*>& enums) {
            for (auto schema_enum_binding : enums) {
                // @note: @es3n1n: get type name by align size
                //
                const auto get_type_name = [&builder, schema_enum_binding]() -> std::string {
                    std::string type_storage;

                    switch (schema_enum_binding->m_unAlignOf) {
                    case 1:
                        [[fallthrough]];
                    case 2:
                        [[fallthrough]];
                    case 4:
                        [[fallthrough]];
                    case 8:
                        type_storage = builder.get_uint(8 * schema_enum_binding->m_unAlignOf);
                        break;
                    default:
                        type_storage = "INVALID_TYPE";
                    }

                    return type_storage;
                };

                // @note: @es3n1n: print meta info
                //
                PrintEnumInfo(builder, schema_enum_binding);

                // @note: @es3n1n: begin enum class
                //
                builder.begin_enum_class(schema_enum_binding->m_pszName, get_type_name());

                // @note: @og: build max based on numeric_limits of unAlignOf
                //
                const auto print_enum_item = [schema_enum_binding, &builder](const SchemaEnumeratorInfoData_t& field) {
                    switch (schema_enum_binding->m_unAlignOf) {
                    case 1:
                        builder.enum_item(field.m_szName, field.m_uint8);
                        break;
                    case 2:
                        builder.enum_item(field.m_szName, field.m_uint16);
                        break;
                    case 4:
                        builder.enum_item(field.m_szName, field.m_uint32);
                        break;
                    case 8:
                        builder.enum_item(field.m_szName, field.m_uint64);
                        break;
                    default:
                        builder.enum_item(field.m_szName, field.m_uint64);
                    }
                };

                // @note: @es3n1n: assemble enum items
                //
                for (const auto& field : schema_enum_binding->GetEnumeratorValues()) {
                    // @note: @og: dump enum metadata
                    //
                    for (auto j = 0; j < field.m_nMetadataSize; j++) {
                        auto field_metadata = field.m_pMetadata[j];

                        if (auto data = GetMetadataValue(field_metadata); data.empty())
                            builder.comment(field_metadata.m_szName);
                        else
                            builder.comment(std::format("{} \"{}\"", field_metadata.m_szName, data));
                    }

                    print_enum_item(field);
                }

                // @note: @es3n1n: we are done with this enum
                //
                builder.end_enum_class();
            }
        }

        /// @return {type_name, array_sizes}
        auto parse_array(CSchemaType* type) -> std::pair<std::string, std::vector<std::size_t>> {
            const auto ptr = type->GetRefClass();
            const auto actual_type = ptr ? ptr : type;

            std::string base_type;
            std::vector<std::size_t> sizes;

            if (actual_type->GetTypeCategory() == ETypeCategory::Schema_FixedArray) {
                // dump all sizes.
                auto schema = reinterpret_cast<CSchemaType_FixedArray*>(actual_type);
                while (true) {
                    sizes.emplace_back(schema->m_nElementCount);
                    schema = reinterpret_cast<CSchemaType_FixedArray*>(schema->m_pElementType);

                    if (schema->GetTypeCategory() != ETypeCategory::Schema_FixedArray) {
                        base_type = schema->m_pszName;
                        break;
                    }
                }
            }

            return {base_type, sizes};
        };

        /// @return @ref std::nullopt if the type is not contained in a module, e.g. because it is a built-in type
        auto get_module(CSchemaType* type) -> std::optional<std::string> {
            if (type->m_pTypeScope != nullptr) {
                if (const auto* class_ = type->m_pTypeScope->FindDeclaredClass(type->m_pszName)) {
                    return class_->m_pszModule;
                } else if (const auto* enum_ = type->m_pTypeScope->FindDeclaredEnum(type->m_pszName)) {
                    return enum_->m_pszModule;
                }
            }

            return std::nullopt;
        };

        /// @return {type_name, array_sizes} where type_name is a fully qualified name
        auto get_type(CSchemaType* type) -> std::pair<std::string, std::vector<std::size_t>> {
            const auto maybe_with_module_name = [type](const auto& type_name) {
                return get_module(type)
                    .transform([&](const auto module_name) { return std::format("{}::{}", module_name, type_name); })
                    .value_or(type_name);
            };
            const auto [type_name, array_sizes] = parse_array(type);

            assert(type_name.empty() == array_sizes.empty());

            if (!type_name.empty() && !array_sizes.empty())
                return {maybe_with_module_name(type_name), array_sizes};

            return {maybe_with_module_name(type->m_pszName), {}};
        };

        void AssembleClasses(source2_gen::Options options, codegen::IGenerator::self_ref builder,
                             const std::unordered_set<const CSchemaClassBinding*>& classes) {
            struct class_t {
                CSchemaClassInfo* target_{};
                std::set<CSchemaClassInfo*> refs_;
                uint32_t used_count_{};
                std::list<std::pair<std::string, ptrdiff_t>> cached_fields_;

                struct cached_datamap_t {
                    std::string type_;
                    std::string name_;
                    ptrdiff_t offset_;
                };
                std::list<cached_datamap_t> cached_datamap_fields_;

                [[nodiscard]] CSchemaClassInfo* GetParent() const {
                    if (!target_->m_pBaseClassses)
                        return nullptr;

                    return target_->m_pBaseClassses->m_pClass;
                }

                void AddRefToClass(CSchemaType* type) {
                    if (type->GetTypeCategory() == ETypeCategory::Schema_DeclaredClass) {
                        refs_.insert(reinterpret_cast<CSchemaType_DeclaredClass*>(type)->m_pClassInfo);
                    }
                }

                [[nodiscard]] bool IsDependsOn(const class_t& other) const {
                    // if current class inherit other.
                    const auto parent = this->GetParent();
                    if (parent == other.target_)
                        return true;

                    // if current class contains ref to other.
                    if (this->refs_.contains(other.target_))
                        return true;

                    // otherwise, order doesn`t matter.
                    return false;
                }

                [[nodiscard]] SchemaClassFieldData_t* GetFirstField() const {
                    if (target_->m_nFieldSize)
                        return &target_->m_pFields[0];
                    return nullptr;
                }

                // @note: @es3n1n: Returns the struct size without its parent's size
                //
                [[nodiscard]] std::ptrdiff_t ClassSizeWithoutParent() const {
                    if (const CSchemaClassInfo* class_parent = this->GetParent(); class_parent)
                        return this->target_->m_nSizeOf - class_parent->m_nSizeOf;
                    return this->target_->m_nSizeOf;
                }
            };

            // @note: @soufiw:
            // sort all classes based on refs and inherit, and then print it.
            // ==================
            std::list<class_t> classes_to_dump;
            bool did_forward_decls = false;

            for (const auto* schema_class_binding : classes) {
                assert(schema_class_binding != nullptr);

                const auto class_info = schema_class_binding->m_pTypeScope->FindDeclaredClass(schema_class_binding->m_pszName);

                auto& class_dump = classes_to_dump.emplace_back();
                class_dump.target_ = class_info;
            }

            for (auto& class_dump : classes_to_dump) {
                const auto class_info = class_dump.target_;

                for (auto k = 0; k < class_info->m_nFieldSize; k++) {
                    const auto field = &class_info->m_pFields[k];
                    if (!field)
                        continue;

                    // forward declare all classes.
                    // @todo: maybe we need to forward declare only pointers to classes?
                    auto ptr = field->m_pSchemaType->GetRefClass();

                    if (auto actual_type = ptr ? ptr : field->m_pSchemaType; actual_type->GetTypeCategory() == ETypeCategory::Schema_DeclaredClass) {
                        builder.forward_declaration(actual_type->m_pszName);
                        did_forward_decls = true;
                    }

                    class_dump.AddRefToClass(field->m_pSchemaType);

                    auto field_class = std::ranges::find_if(classes_to_dump, [field](const class_t& cls) {
                        return cls.target_ == reinterpret_cast<CSchemaType_DeclaredClass*>(field->m_pSchemaType)->m_pClassInfo;
                    });
                    if (field_class != classes_to_dump.end())
                        field_class->used_count_++;
                }
            }

            if (did_forward_decls)
                builder.next_line();

            bool did_change = false;
            do {
                did_change = false;

                // swap until we done.
                for (auto first = classes_to_dump.begin(); first != classes_to_dump.end(); ++first) {
                    bool second_below_first = false;

                    for (auto second = classes_to_dump.begin(); second != classes_to_dump.end(); ++second) {
                        if (second == first) {
                            second_below_first = true;
                            continue;
                        }

                        // swap if second class below first, and first depends on second.
                        bool first_depend = first->IsDependsOn(*second);

                        // swap if first class below second, and second depends on first.
                        bool second_depend = second->IsDependsOn(*first);

                        if (first_depend && second_depend) {
                            // classes depends on each other, forward declare them.
                            // @todo: verify that cyclic dependencies is a pointers.
                            continue;
                        }

                        if (second_below_first ? first_depend : second_depend) {
                            std::iter_swap(first, second);
                            did_change = true;
                        }
                    }
                }
            } while (did_change);
            // ==================

            for (auto& class_dump : classes_to_dump) {
                // @note: @es3n1n: get class info, assemble it
                //
                const auto class_parent = class_dump.GetParent();
                const auto class_info = class_dump.target_;

                const auto is_struct = std::string_view{class_info->m_pszName}.ends_with("_t");
                PrintClassInfo(builder, class_info);

                // @note: @es3n1n: get parent name
                //
                std::string parent_cls_name;
                if (auto parent = class_info->m_pBaseClassses ? class_info->m_pBaseClassses->m_pClass : nullptr; parent)
                    parent_cls_name = parent->m_pszName;

                // @note: @es3n1n: start class
                //
                if (is_struct)
                    builder.begin_struct_with_base_type(class_info->m_pszName, parent_cls_name, "");
                else
                    builder.begin_class_with_base_type(class_info->m_pszName, parent_cls_name, "");

                // @note: @es3n1n: field assembling state
                //
                struct {
                    std::size_t last_field_size = 0ull;
                    std::size_t last_field_offset = 0ull;
                    bool assembling_bitfield = false;
                    std::size_t total_bits_count_in_union = 0ull;

                    std::ptrdiff_t collision_end_offset = 0ull; // @fixme: @es3n1n: todo proper collision fix and remove this var
                } state;

                // @note: @es3n1n: if we need to pad first field or if there's no fields in this class
                // and we need to properly pad it to make sure its size is the same as we expect it
                //
                std::optional<std::ptrdiff_t> first_field_offset = std::nullopt;
                if (const auto first_field = class_dump.GetFirstField(); first_field)
                    first_field_offset = first_field->m_nSingleInheritanceOffset;

                const std::ptrdiff_t parent_class_size = class_parent ? class_parent->m_nSizeOf : 0;

                std::ptrdiff_t expected_pad_size = first_field_offset.value_or(class_dump.ClassSizeWithoutParent());
                if (expected_pad_size) // @note: @es3n1n: if there's a pad size we should account the parent class size
                    expected_pad_size -= parent_class_size;

                // @note: @es3n1n: and finally insert a pad
                //
                if (expected_pad_size > 0) // @fixme: @es3n1n: this is wrong, i probably should check for collisions instead
                    builder.access_modifier("private")
                        .struct_padding(codegen::Padding{
                            .pad_offset = parent_class_size,
                            .size = codegen::Padding::Bytes{static_cast<std::size_t>(expected_pad_size)},
                            .is_private_field = true,
                            .move_cursor_to_next_line = false,
                        })
                        .reset_tabs_count()
                        .comment(std::format("{:#x}", parent_class_size))
                        .restore_tabs_count();

                // @todo: @es3n1n: if for some mysterious reason this class describes fields
                // of the base class we should handle it too.
                if (class_parent && first_field_offset.has_value() && first_field_offset.value() < class_parent->m_nSizeOf) {
                    builder.comment(
                        std::format("Collision detected({:#x}->{:#x}), output may be wrong.", first_field_offset.value_or(0), class_parent->m_nSizeOf));
                    state.collision_end_offset = class_parent->m_nSizeOf;
                }

                // @note: @es3n1n: begin public members
                //
                builder.access_modifier("public");

                for (const auto& field : class_info->GetFields()) {
                    // @fixme: @es3n1n: todo proper collision fix and remove this block
                    if (state.collision_end_offset && field.m_nSingleInheritanceOffset < state.collision_end_offset) {
                        builder.comment(std::format("Skipped field \"{}\" @ {:#x} because of the struct collision", field.m_pszName,
                                                    field.m_nSingleInheritanceOffset));
                        continue;
                    }

                    // @note: @es3n1n: obtaining size
                    //
                    const int field_size = field.m_pSchemaType->GetSize().value_or(0);

                    // @note: @es3n1n: parsing type
                    //
                    const auto [type_name, array_sizes] = get_type(field.m_pSchemaType);
                    const auto var_info = field_parser::parse(builder, type_name, field.m_pszName, array_sizes);

                    // @note: @es3n1n: insert padding if needed
                    //
                    const auto expected_offset = state.last_field_offset + state.last_field_size;
                    if (state.last_field_offset && state.last_field_size &&
                        expected_offset < static_cast<std::uint64_t>(field.m_nSingleInheritanceOffset) && !state.assembling_bitfield) {

                        builder.access_modifier("private")
                            .struct_padding(codegen::Padding{
                                .pad_offset = expected_offset,
                                .size{codegen::Padding::Bytes{field.m_nSingleInheritanceOffset - expected_offset}},
                                .is_private_field = true,
                                .move_cursor_to_next_line = false,
                            })
                            .reset_tabs_count()
                            .comment(std::format("{:#x}", expected_offset))
                            .restore_tabs_count()
                            .access_modifier("public");
                    }

                    // @note: @es3n1n: begin union if we're assembling bitfields
                    //
                    if (!state.assembling_bitfield && var_info.is_bitfield()) {
                        builder.begin_bitfield_block();
                        state.assembling_bitfield = true;
                    }

                    // @note: @es3n1n: if we are done with bitfields we should insert a pad and finish union
                    //
                    if (state.assembling_bitfield && !var_info.is_bitfield()) {
                        const auto expected_union_size_bytes = field.m_nSingleInheritanceOffset - state.last_field_offset;
                        const auto expected_union_size_bits = expected_union_size_bytes * 8;

                        const auto actual_union_size_bits = state.total_bits_count_in_union;

                        if (expected_union_size_bits < state.total_bits_count_in_union)
                            throw std::runtime_error(
                                std::format("Unexpected union size: {}. Expected: {}", state.total_bits_count_in_union, expected_union_size_bits));

                        if (expected_union_size_bits > state.total_bits_count_in_union)
                            builder.struct_padding(codegen::Padding{
                                .pad_offset = std::nullopt,
                                .size = codegen::Padding::Bits{expected_union_size_bits - actual_union_size_bits},
                                .is_private_field = false,
                                .move_cursor_to_next_line = true,
                            });

                        state.last_field_offset += expected_union_size_bytes;
                        state.last_field_size = expected_union_size_bytes;

                        builder.end_bitfield_block(false)
                            .reset_tabs_count()
                            .comment(std::format("{:d} bits", expected_union_size_bits))
                            .restore_tabs_count();

                        state.total_bits_count_in_union = 0ull;
                        state.assembling_bitfield = false;
                    }

                    // @note: @es3n1n: dump metadata
                    //
                    for (auto j = 0; j < field.m_nMetadataSize; j++) {
                        auto field_metadata = field.m_pMetadata[j];

                        if (auto data = GetMetadataValue(field_metadata); data.empty())
                            builder.comment(field_metadata.m_szName);
                        else
                            builder.comment(std::format("{} \"{}\"", field_metadata.m_szName, data));
                    }

                    // if this prop is a non-pointer class, check if its worth directly embedding the accumulated offset of it into the metadata
                    auto prop_class =
                        std::ranges::find_if(classes_to_dump, [type_name](const class_t& cls) { return cls.target_->GetName().compare(type_name) == 0; });
                    if (prop_class != classes_to_dump.end()) {
                        // verify for min/max fields count, we don't want to bloat the dump by embeding too much stuff
                        if (prop_class->cached_fields_.size() >= kMinFieldCountForClassEmbed &&
                            prop_class->cached_fields_.size() <= kMaxFieldCountForClassEmbed) {
                            // if a class is used in too many classes its likely not very useful, so ignore it
                            if (prop_class->used_count_ <= kMaxReferencesForClassEmbed) {
                                for (const auto& [cached_field_name, cached_field_offset] : prop_class->cached_fields_) {
                                    const auto accumulated_offset = cached_field_offset + field.m_nSingleInheritanceOffset;
                                    builder.comment(std::format("-> {} - {:#x}", cached_field_name, accumulated_offset));
                                }
                            }
                        }
                    }

                    // @note: @es3n1n: update state
                    //
                    if (field.m_nSingleInheritanceOffset && field_size) {
                        state.last_field_offset = field.m_nSingleInheritanceOffset;
                        state.last_field_size = static_cast<std::size_t>(field_size);
                    }
                    if (var_info.is_bitfield())
                        state.total_bits_count_in_union += var_info.m_bitfield_size;

                    // @note: @es3n1n: push prop
                    //
                    builder.prop(codegen::Prop{.type_name = var_info.m_type, .name = var_info.formatted_name()}, false);
                    if (!var_info.is_bitfield()) {
                        builder.reset_tabs_count().comment(std::format("{:#x}", field.m_nSingleInheritanceOffset), false).restore_tabs_count();
                        class_dump.cached_fields_.emplace_back(var_info.formatted_name(), field.m_nSingleInheritanceOffset);
                    }
                    builder.next_line();
                }

                // @note: @es3n1n: if struct ends with union we should end union before ending the class
                //
                if (state.assembling_bitfield) {
                    const auto actual_union_size_bits = state.total_bits_count_in_union;

                    // @note: @es3n1n: apply 8 bytes align
                    //
                    const auto expected_union_size_bits = actual_union_size_bits + (actual_union_size_bits % 8);

                    if (expected_union_size_bits > actual_union_size_bits)
                        builder.struct_padding(codegen::Padding{
                            .pad_offset = std::nullopt,
                            .size = codegen::Padding::Bits{expected_union_size_bits - actual_union_size_bits},
                            .is_private_field = false,
                            .move_cursor_to_next_line = true,
                        });

                    builder.end_bitfield_block(false).reset_tabs_count().comment(std::format("{:d} bits", expected_union_size_bits)).restore_tabs_count();

                    state.total_bits_count_in_union = 0;
                    state.assembling_bitfield = false;
                }

                // @note: @es3n1n: dump static fields
                //
                if (options.static_members) {
                    if (class_info->m_nStaticFieldsSize) {
                        if (class_info->m_nFieldSize)
                            builder.next_line();
                        builder.comment("Static fields:");
                    }

                    // The current class may be defined in multiple scopes. It doesn't matter which one we use, as all definitions are the same..
                    // TODO: verify the above statement. Are static fields really shared between scopes?
                    const std::string scope_name{class_info->m_pTypeScope->BGetScopeName()};

                    for (auto s = 0; s < class_info->m_nStaticFieldsSize; s++) {
                        auto static_field = &class_info->m_pStaticFields[s];

                        auto [type, mod] = get_type(static_field->m_pSchemaType);
                        const auto var_info = field_parser::parse(builder, type, static_field->m_pszName, mod);
                        builder.static_field_getter(var_info.m_type, var_info.m_name, scope_name, class_info->m_pszName, s);
                    }
                }

                if (class_info->m_pFieldMetadataOverrides && class_info->m_pFieldMetadataOverrides->m_iTypeDescriptionCount > 1) {
                    const auto& dm = class_info->m_pFieldMetadataOverrides;

                    for (std::uint64_t s = 0; s < dm->m_iTypeDescriptionCount; s++) {
                        auto* t = &dm->m_pTypeDescription[s];
                        if (!t)
                            continue;

                        if (t->GetFieldName().empty())
                            continue;

                        const auto var_info = field_parser::parse(t->m_iFieldType, t->GetFieldName().data(), t->m_nFieldSize);

                        std::string field_type = var_info.m_type;
                        if (t->m_iFieldType == fieldtype_t::FIELD_EMBEDDED) {
                            field_type = t->m_pDataMap->m_pszClassName;
                        }

                        std::string field_name = var_info.formatted_name();

                        // @note: @og: if schema dump already has this field, then just skip it
                        if (const auto it = std::ranges::find_if(
                                class_dump.cached_fields_, [t, field_name](const auto& f) { return f.first == field_name && f.second == t->m_iOffset; });
                            it != class_dump.cached_fields_.end())
                            continue;

                        class_dump.cached_datamap_fields_.emplace_back(field_type, field_name, t->m_iOffset);
                    }

                    if (!class_dump.cached_datamap_fields_.empty()) {
                        if (class_info->m_nFieldSize)
                            builder.next_line();

                        builder.comment("Datamap fields:");
                        for (auto& [field_type, field_name, field_offset] : class_dump.cached_datamap_fields_) {
                            builder.comment(std::format("{} {}; // {:#x}", field_type, field_name, field_offset));
                        }
                    }
                }

                if (!class_info->m_nFieldSize && !class_info->m_nStaticMetadataSize)
                    builder.comment("No schema binary for binding");

                if (is_struct) {
                    builder.end_struct();
                } else {
                    builder.end_class();
                }
            }
        }

    } // namespace

    std::unique_ptr<codegen::IGenerator> get_generator_for_language(source2_gen::Language language) {
        switch (language) {
        case source2_gen::Language::cpp:
            return std::make_unique<codegen::generator_cpp_t>();
        case source2_gen::Language::c:
            return std::make_unique<codegen::generator_c_t>();
        }

        assert(false && "unhandled enumerator");
        std::abort();
    }

    /// @param exclude_module_name will not be contained in the return value
    /// @return All modules that are required by properties of @p classes
    std::set<std::string> find_required_modules_of_class(std::string_view exclude_module_name, const CSchemaClassBinding& class_) {
        std::set<std::string> modules{};

        for (const auto& field : std::span{class_.m_pFields, static_cast<std::size_t>(class_.m_nFieldSize)}) {
            if (const auto module = get_module(field.m_pSchemaType)) {
                modules.emplace(module.value());
            }
        }

        for (const auto& field : std::span{class_.m_pStaticFields, static_cast<std::size_t>(class_.m_nStaticFieldsSize)}) {
            if (const auto module = get_module(field.m_pSchemaType)) {
                modules.emplace(module.value());
            }
        }

        if (const auto found = modules.find(std::string{exclude_module_name}); found != modules.end()) {
            modules.erase(found);
        }

        return modules;
    }

    /// @param exclude_module_name will not be contained in the return value
    /// @return All modules that are required by properties of @p classes
    std::set<std::string> find_required_modules(std::string_view exclude_module_name, const std::unordered_set<const CSchemaClassBinding*>& classes) {
        std::set<std::string> modules{};

        for (const auto& class_ : classes) {
            assert(class_ != nullptr);

            const auto partial = find_required_modules_of_class(exclude_module_name, *class_);
            modules.insert(partial.begin(), partial.end());
        }

        return modules;
    }

    void GenerateTypeScopeSdk(source2_gen::Options options, std::string_view module_name, const std::unordered_set<const CSchemaEnumBinding*>& enums,
                              const std::unordered_set<const CSchemaClassBinding*>& classes) {
        // @note: @es3n1n: print debug info
        //
        std::cout << std::format("{}: Assembling {}", __FUNCTION__, module_name) << std::endl;

        // @note: @es3n1n: init codegen
        //
        auto generator = get_generator_for_language(options.emit_language);
        auto& builder = *generator;

        builder.preamble();

        // @note: @es3n1n: include files
        //
        for (const auto& required_module : find_required_modules(module_name, classes)) {
            builder.include(required_module, codegen::IncludeOptions{.local = true, .system = false});
        }

        // @note: @es3n1n: print banner
        //
        builder.next_line()
            .comment("/////////////////////////////////////////////////////////////")
            .comment(std::format("Module: {}", module_name))
            .comment(std::format("Classes count: {}", classes.size()))
            .comment(std::format("Enums count: {}", enums.size()))
            .comment(std::string{kCreatedBySource2genMessage})
            .comment("/////////////////////////////////////////////////////////////")
            .next_line();

        builder.begin_namespace(std::format("source2sdk::{}", module_name));

        // @note: @es3n1n: assemble props
        //
        AssembleEnums(builder, enums);
        AssembleClasses(options, builder, classes);

        builder.end_namespace();

        // @note: @es3n1n: write generated data to output file
        //
        if (!std::filesystem::exists(kOutDirName))
            std::filesystem::create_directories(kOutDirName);

        const std::string out_file_path = std::format("{}/{}.{}", kOutDirName, module_name, builder.get_file_extension());

        std::ofstream f(out_file_path, std::ios::out);
        f << builder.str();
        if (f.bad()) {
            std::cerr << std::format("Could not write to {}: {}", out_file_path, std::strerror(errno)) << std::endl;
            // This std::exit() is bad. Instead, we could return the dumped
            // header name and content to the caller in a std::expected. Let the
            // caller write the file. That would also allow the caller to choose
            // the output directory and handle errors.
            std::exit(1);
        }
    }
} // namespace sdk

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
