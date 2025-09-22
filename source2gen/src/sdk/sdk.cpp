// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.

// ReSharper disable CppClangTidyClangDiagnosticLanguageExtensionToken
#include "sdk/sdk.h"
#include "Include.h"
#include "tools/codegen/c.h"
#include "tools/codegen/codegen.h"
#include "tools/codegen/cpp.h"
#include "tools/field_parser.h"
#include "tools/util.h"
#include <absl/strings/str_replace.h>
#include <cstdlib>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <ranges>
#include <set>
#include <span>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace {
    enum class NameSource {
        include,
        forward_declaration,
    };

    struct NameLookup {
        std::string module{};

        /// Decayed
        std::string type_name{};

        NameSource source{};

        auto operator<=>(const NameLookup&) const = default;
    };

    struct BitfieldEntry {
        std::string name{};
        std::size_t size{};
        /// Lifetime of fields' pointers bound to the source2's @ref CSchemaClassInfo
        std::vector<SchemaMetadataEntryData_t> metadata{};
    };

    struct ClassAssemblyState {
        std::optional<std::ptrdiff_t> last_field_size = std::nullopt;
        std::optional<std::ptrdiff_t> last_field_offset = std::nullopt;
        bool assembling_bitfield = false;
        std::vector<BitfieldEntry> bitfield = {};
        std::int32_t bitfield_start = 0;

        std::ptrdiff_t collision_end_offset = 0ull; // @fixme: @es3n1n: todo proper collision fix and remove this var
    };

    /**
     * Project structure is
     * <kOutDirName>
     * - CMakeLists.txt
     * - ...
     * - include/
     *   - <kSdkDirName>
     *     - some_module
     *       - some_header.hpp
     */
    constexpr std::string_view kOutDirName = "sdk";
    constexpr std::string_view kIncludeDirName = "source2sdk";

    constinit std::array string_metadata_entries = {
        FNV32("MCellForDomain"),
        FNV32("MCustomFGDMetadata"),
        FNV32("MFieldVerificationName"),
        FNV32("MKV3TransferName"),
        FNV32("MNetworkAlias"),
        FNV32("MNetworkChangeCallback"),
        FNV32("MNetworkEncoder"),
        FNV32("MNetworkExcludeByName"),
        FNV32("MNetworkExcludeByUserGroup"),
        FNV32("MNetworkIncludeByName"),
        FNV32("MNetworkIncludeByUserGroup"),
        FNV32("MNetworkReplayCompatField"),
        FNV32("MNetworkSerializer"),
        FNV32("MNetworkTypeAlias"),
        FNV32("MNetworkUserGroup"),
        FNV32("MNetworkUserGroupProxy"),
        FNV32("MParticleReplacementOp"),
        FNV32("MPropertyArrayElementNameKey"),
        FNV32("MPropertyAttributeChoiceName"),
        FNV32("MPropertyAttributeEditor"),
        FNV32("MPropertyAttributeRange"),
        FNV32("MPropertyAttributeSuggestionName"),
        FNV32("MPropertyCustomEditor"),
        FNV32("MPropertyCustomFGDType"),
        FNV32("MPropertyDescription"),
        FNV32("MPropertyDescription"),
        FNV32("MPropertyExtendedEditor"),
        FNV32("MPropertyFriendlyName"),
        FNV32("MPropertyFriendlyName"),
        FNV32("MPropertyGroupName"),
        FNV32("MPropertyIconName"),
        FNV32("MPropertyStartGroup"),
        FNV32("MPropertySuppressExpr"),
        FNV32("MPulseCellOutflowHookInfo"),
        FNV32("MPulseEditorHeaderIcon"),
#if !defined(DEADLOCK) && !defined(DOTA2)
        FNV32("MPulseProvideFeatureTag"),
#endif
        FNV32("MResourceBlockType"),
        FNV32("MScriptDescription"),
        FNV32("MSrc1ImportAttributeName"),
        FNV32("MSrc1ImportDmElementType"),
        FNV32("MVDataOutlinerIcon"),
        FNV32("MVDataOutlinerIconExpr"),
        FNV32("MVDataUniqueMonotonicInt"),
        FNV32("MVectorIsSometimesCoordinate"),
    };

    constinit std::array string_class_metadata_entries = {
        FNV32("MResourceTypeForInfoType"),
        FNV32("MDiskDataForResourceType"),
    };

    constinit std::array var_name_string_class_metadata_entries = {
        FNV32("MNetworkVarNames"), FNV32("MNetworkOverride"), FNV32("MNetworkVarTypeOverride"), FNV32("MScriptDescription"), FNV32("MParticleDomainTag"),
    };

    constinit std::array integer_metadata_entries = {
        FNV32("MNetworkVarEmbeddedFieldOffsetDelta"),
        FNV32("MNetworkBitCount"),
        FNV32("MNetworkPriority"),
        FNV32("MParticleOperatorType"),
        FNV32("MPropertySortPriority"),
        FNV32("MParticleMinVersion"),
        FNV32("MParticleMaxVersion"),
#if defined(DEADLOCK) || defined(DOTA2)
        FNV32("MPulseProvideFeatureTag"),
#endif
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

    void warn(std::string_view message) {
        std::cerr << "warning: " << message << '\n';
    }

    // @note: @es3n1n: some more utils
    //
    std::string GetMetadataValue(const SchemaMetadataEntryData_t& metadata_entry) {
        std::string value;

        const auto value_hash_name = fnv32::hash_runtime(metadata_entry.m_szName);

        if (std::ranges::find(var_name_string_class_metadata_entries, value_hash_name) != var_name_string_class_metadata_entries.end()) {
            const auto& var_value = metadata_entry.m_pNetworkValue->m_VarValue;
            const auto check_ptr = [](const char* ptr) -> bool {
                /// @note: hotfix for the deadlock 14/09/24 update,
                ///     where they filled some ptrs with -1 instead of nullptr
                return ptr != nullptr && ptr != reinterpret_cast<const char*>(-1);
            };

            if (check_ptr(var_value.m_pszType) && check_ptr(var_value.m_pszName))
                value = std::format("{} {}", var_value.m_pszType, var_value.m_pszName);
            else if (check_ptr(var_value.m_pszName) && !check_ptr(var_value.m_pszType))
                value = var_value.m_pszName;
            else if (!check_ptr(var_value.m_pszName) && check_ptr(var_value.m_pszType))
                value = var_value.m_pszType;
        } else if (std::ranges::find(string_class_metadata_entries, value_hash_name) != string_class_metadata_entries.end()) {
            /// Explicitly convert to std::string with the size as the string may not end with a nullterm
            /// But if this string does contain a null terminator, we should properly handle this too
            const auto& szValue = metadata_entry.m_pNetworkValue->m_szValue;
            const auto null_pos = std::find(szValue.begin(), szValue.end(), 0x00);
            const auto size = null_pos != szValue.end() ? std::distance(szValue.begin(), null_pos) : szValue.size();

            value = std::string(metadata_entry.m_pNetworkValue->m_szValue.data(), size);
        } else if (std::ranges::find(string_metadata_entries, value_hash_name) != string_metadata_entries.end()) {
            value = metadata_entry.m_pNetworkValue->m_pszValue;
        } else if (std::ranges::find(integer_metadata_entries, value_hash_name) != integer_metadata_entries.end()) {
            value = std::to_string(metadata_entry.m_pNetworkValue->m_nValue);
        } else if (std::ranges::find(float_metadata_entries, value_hash_name) != float_metadata_entries.end()) {
            value = std::to_string(metadata_entry.m_pNetworkValue->m_fValue);
        }

        return value;
    }

    /// https://en.cppreference.com/w/cpp/language/classes#Standard-layout_class
    /// Doesn't check for all requirements, but is strict enough for what we are doing.
    [[nodiscard]] bool IsStandardLayoutClass(std::map<sdk::TypeIdentifier, bool>& cache, const CSchemaClassInfo& class_) {
        const auto id = sdk::TypeIdentifier{.module = std::string{class_.GetModule()}, .name = std::string{class_.GetName()}};

        if (const auto found = cache.find(id); found != cache.end()) {
            return found->second;
        }

        // only one class in the hierarchy has non-static data members.
        // assumes that source2 only has single inheritance.
        {
            const auto* pClass = &class_;
            int classes_with_fields = 0;
            do {
                // also check size because not all members are registered with
                // the schema system.
                classes_with_fields += ((pClass->m_nSizeOf > 1) || (pClass->m_nFieldSize != 0)) ? 1 : 0;

                if (classes_with_fields > 1) {
                    return cache.emplace(id, false).first->second;
                }

                pClass = (pClass->m_pBaseClasses == nullptr) ? nullptr : pClass->m_pBaseClasses->m_pClass;
            } while (pClass != nullptr);
        }

        const auto has_non_standard_layout_field = std::ranges::any_of(
            class_.GetFields() | std::ranges::views::transform([&](const SchemaClassFieldData_t& e) {
                if (const auto* e_class = e.m_pSchemaType->GetAsDeclaredClass(); e_class != nullptr && e_class->m_pClassInfo != nullptr) {
                    return !IsStandardLayoutClass(cache, *e_class->m_pClassInfo);
                } else {
                    // Everything that is not a class has no effect
                    return false;
                }
            }),
            std::identity{});

        if (has_non_standard_layout_field) {
            return cache.emplace(id, false).first->second;
        }

        return cache.emplace(id, true).first->second;
    }

    /// Gets the alignment of a class by recursing through all of its fields.
    /// Does not guess, the returned value is correct if set.
    /// @param cache Used to look up and store alignment of fields
    /// @return @ref GetRegisteredAlignment() if set. Otherwise tries to determine the alignment by recursing through all fields.
    /// Returns @ref std::nullopt if one or more fields have unknown alignment.
    [[nodiscard]] std::optional<int> GetClassAlignmentRecursive(std::map<sdk::TypeIdentifier, std::optional<int>>& cache, const CSchemaClassInfo& class_) {
        const auto id = sdk::TypeIdentifier{.module = std::string{class_.GetModule()}, .name = std::string{class_.GetName()}};

        if (const auto found = cache.find(id); found != cache.end()) {
            return found->second;
        }

        return class_.GetRegisteredAlignment().or_else([&]() {
            int base_alignment = 0;

            if (class_.m_pBaseClasses != nullptr) {
                if (const auto maybe_base_alignment = GetClassAlignmentRecursive(cache, *class_.m_pBaseClasses->m_pClass)) {
                    base_alignment = maybe_base_alignment.value();
                } else {
                    // we have a base class, but it has unknown alignment
                    return cache.emplace(id, std::nullopt).first->second;
                }
            }

            auto field_alignments = class_.GetFields() | std::ranges::views::transform([&](const SchemaClassFieldData_t& e) {
                                        if (const auto* e_class = e.m_pSchemaType->GetAsDeclaredClass(); e_class != nullptr) {
                                            return GetClassAlignmentRecursive(cache, *e_class->m_pClassInfo);
                                        } else {
                                            return e.m_pSchemaType->GetSizeAndAlignment().and_then([](const auto& e) { return std::get<1>(e); });
                                        }
                                    });

            if (field_alignments.empty()) {
                // This is an empty class. The generator will add a single pad with alignment 1.
                return cache.emplace(id, std::make_optional((base_alignment == 0) ? 1 : base_alignment)).first->second;
            } else if (std::ranges::all_of(field_alignments, &std::optional<int>::has_value)) {
                int max_alignment = base_alignment;
                for (const auto& e : field_alignments) {
                    max_alignment = std::max(max_alignment, e.value());
                }
                return cache.emplace(id, std::make_optional(max_alignment)).first->second;
            } else {
                // there are fields with unknown alignment
                return cache.emplace(id, std::nullopt).first->second;
            }
        });
    }

    /// @return For class types, returns @ref GetClassAlignmentRecursive(). Otherwise returns the immediately available size.
    [[nodiscard]]
    std::optional<int> GetAlignmentOfTypeRecursive(std::map<sdk::TypeIdentifier, std::optional<int>>& cache, const CSchemaType& type) {
        if (const auto* class_ = type.GetAsDeclaredClass(); class_ != nullptr && class_->m_pClassInfo != nullptr) {
            return GetClassAlignmentRecursive(cache, *class_->m_pClassInfo);
        } else {
            return type.GetSizeAndAlignment().and_then([](const auto& e) { return std::get<1>(e); });
        }
    }

    [[nodiscard]]
    codegen::TypeCategory GetTypeCategory(const SchemaClassFieldData_t& field) {
        using enum codegen::TypeCategory;

        // these cases and sub-cases aren't exactly correct, but they do the job.
        // we can improve the classification when we need more details.
        switch (field.m_pSchemaType->GetTypeCategory()) {
        case ETypeCategory::Schema_DeclaredEnum:
            return enum_;
        case ETypeCategory::Schema_DeclaredClass:
            return class_or_struct;
        case ETypeCategory::Schema_FixedArray: {
            auto* schema = reinterpret_cast<const CSchemaType_FixedArray*>(field.m_pSchemaType);
            switch (schema->m_pElementType->m_unTypeCategory) {
            case ETypeCategory::Schema_DeclaredEnum:
                return enum_;
            case ETypeCategory::Schema_DeclaredClass:
            case ETypeCategory::Schema_Ptr:
                return class_or_struct;
            default:
                return built_in;
            }
        }
        case ETypeCategory::Schema_Ptr: {
            if (const auto ref_class = field.m_pSchemaType->GetRefClass(); ref_class != nullptr) {
                // there are only pointers to structs/classes in source2, not to enums
                if (ref_class->GetTypeCategory() == ETypeCategory::Schema_DeclaredClass) {
                    return class_or_struct;
                }
            }
            return built_in;
        }
        default:
            return built_in;
        }
    }

    void PrintClassInfo(sdk::GeneratorCache& cache, codegen::IGenerator::self_ref generator, const CSchemaClassBinding& class_) {
        generator.comment(std::format("Registered alignment: {}", class_.GetRegisteredAlignment().transform(&util::to_hex_string).value_or("unknown")));
        generator.comment(
            std::format("Alignment: {}", GetClassAlignmentRecursive(cache.class_alignment, class_).transform(&util::to_hex_string).value_or("unknown")));
        generator.comment(std::format("Standard-layout class: {}", IsStandardLayoutClass(cache.class_has_standard_layout, class_)));
        generator.comment(std::format("Size: {:#x}", class_.m_nSizeOf));

        if ((class_.m_nClassFlags & SCHEMA_CF1_HAS_VIRTUAL_MEMBERS) != 0) // @note: @og: its means that class probably does have vtable
            generator.comment("Has VTable");
        if ((class_.m_nClassFlags & SCHEMA_CF1_IS_ABSTRACT) != 0)
            generator.comment("Is Abstract");
        if ((class_.m_nClassFlags & SCHEMA_CF1_HAS_TRIVIAL_CONSTRUCTOR) != 0)
            generator.comment("Has Trivial Constructor");
        if ((class_.m_nClassFlags & SCHEMA_CF1_HAS_TRIVIAL_DESTRUCTOR) != 0)
            generator.comment("Has Trivial Destructor");

#if defined(CS2) || defined(DOTA2)
        if ((class_.m_nClassFlags & SCHEMA_CF1_CONSTRUCT_ALLOWED) != 0)
            generator.comment("Construct allowed");
        if ((class_.m_nClassFlags & SCHEMA_CF1_CONSTRUCT_DISALLOWED) != 0)
            generator.comment("Construct disallowed");
        if ((class_.m_nClassFlags & SCHEMA_CF1_INFO_TAG_MConstructibleClassBase) != 0)
            generator.comment("MConstructibleClassBase");
        if ((class_.m_nClassFlags & SCHEMA_CF1_INFO_TAG_MClassHasCustomAlignedNewDelete) != 0)
            generator.comment("MClassHasCustomAlignedNewDelete");
        if ((class_.m_nClassFlags & SCHEMA_CF1_INFO_TAG_MClassHasEntityLimitedDataDesc) != 0)
            generator.comment("MClassHasEntityLimitedDataDesc");
        if ((class_.m_nClassFlags & SCHEMA_CF1_INFO_TAG_MDisableDataDescValidation) != 0)
            generator.comment("MDisableDataDescValidation");
        if ((class_.m_nClassFlags & SCHEMA_CF1_INFO_TAG_MIgnoreTypeScopeMetaChecks) != 0)
            generator.comment("MIgnoreTypeScopeMetaChecks");
        if ((class_.m_nClassFlags & SCHEMA_CF1_INFO_TAG_MNetworkNoBase) != 0)
            generator.comment("MNetworkNoBase");
        if ((class_.m_nClassFlags & SCHEMA_CF1_INFO_TAG_MNetworkAssumeNotNetworkable) != 0)
            generator.comment("MNetworkAssumeNotNetworkable");
#endif

        if (class_.m_nStaticMetadataSize > 0)
            generator.comment("");

        for (const auto& metadata : class_.GetStaticMetadata()) {
            if (const auto value = GetMetadataValue(metadata); !value.empty())
                generator.comment(std::format("static metadata: {} \"{}\"", metadata.m_szName, value));
            else
                generator.comment(std::format("static metadata: {}", metadata.m_szName));
        }
    }

    void PrintEnumInfo(codegen::IGenerator::self_ref generator, const CSchemaEnumBinding& enum_binding) {
        generator.comment(std::format("Enumerator count: {}", enum_binding.m_nEnumeratorCount))
            .comment(std::format("Alignment: {}", enum_binding.m_unAlignOf))
            .comment(std::format("Size: {:#x}", enum_binding.m_unSizeOf));

        if (enum_binding.m_nStaticMetadataSize > 0)
            generator.comment("");

        for (const auto& metadata : enum_binding.GetStaticMetadata()) {
            generator.comment(std::format("metadata: {}", metadata.m_szName));
        }
    }

    void AssembleEnum(codegen::IGenerator::self_ref generator, const CSchemaEnumBinding& schema_enum_binding) {
        // @note: @es3n1n: get type name by align size
        //
        const auto underlying_type_name = generator.get_uint(schema_enum_binding.m_unAlignOf * 8);

        // @note: @es3n1n: print meta info
        //
        PrintEnumInfo(generator, schema_enum_binding);

        // @note: @es3n1n: begin enum class
        //
        generator.begin_enum(schema_enum_binding.m_pszName, underlying_type_name);

        // @note: @og: build max based on numeric_limits of unAlignOf
        //
        const auto print_enum_item = [schema_enum_binding, &generator](const SchemaEnumeratorInfoData_t& field) {
            switch (schema_enum_binding.m_unAlignOf) {
            case 1:
                generator.enum_item(field.m_szName, field.m_uint8);
                break;
            case 2:
                generator.enum_item(field.m_szName, field.m_uint16);
                break;
            case 4:
                generator.enum_item(field.m_szName, field.m_uint32);
                break;
            case 8:
                generator.enum_item(field.m_szName, field.m_uint64);
                break;
            default:
                generator.enum_item(field.m_szName, field.m_uint64);
            }
        };

        // @note: @es3n1n: assemble enum items
        //
        for (const auto& field : schema_enum_binding.GetEnumeratorValues()) {
            // @note: @og: dump enum metadata
            //
            for (auto j = 0; j < field.m_nMetadataSize; j++) {
                auto field_metadata = field.m_pMetadata[j];

                if (auto data = GetMetadataValue(field_metadata); data.empty())
                    generator.comment(field_metadata.m_szName);
                else
                    generator.comment(std::format("{} \"{}\"", field_metadata.m_szName, data));
            }

            print_enum_item(field);
        }

        // @note: @es3n1n: we are done with this enum
        //
        generator.end_enum();
    }

    /// @return {type_name, array_sizes}
    std::pair<std::string, std::vector<std::size_t>> ParseArray(const CSchemaType& type) {
        const auto* ptr = type.GetRefClass();
        const auto& actual_type = ptr ? *ptr : type;

        std::string base_type;
        std::vector<std::size_t> sizes;

        if (actual_type.GetTypeCategory() == ETypeCategory::Schema_FixedArray) {
            // dump all sizes.
            auto* schema = reinterpret_cast<const CSchemaType_FixedArray*>(&actual_type);
            while (true) {
                sizes.emplace_back(schema->m_nElementCount);
                schema = reinterpret_cast<const CSchemaType_FixedArray*>(schema->m_pElementType);

                if (schema->GetTypeCategory() != ETypeCategory::Schema_FixedArray) {
                    base_type = schema->m_pszName;
                    break;
                }
            }
        }

        return {base_type, sizes};
    }

    /// @return Lifetime is bound to string viewed by @p type_name
    [[nodiscard]]
    std::string_view DecayTypeName(std::string_view type_name) {
        if (const auto found = type_name.find('['); found != std::string_view::npos) {
            // "array[123]" -> "array"
            type_name = type_name.substr(0, found);
        }
        if (const auto found = type_name.find('*'); found != std::string_view::npos) {
            // "pointer***" -> "pointer"
            type_name = type_name.substr(0, found);
        }

        return type_name;
    }

    /// @return @ref std::nullopt if the type is not contained in a module visible in @p scope
    std::optional<std::string> GetModuleOfTypeInScope(const CSchemaSystemTypeScope& scope, std::string_view type_name) {
        assert((DecayTypeName(type_name) == type_name) &&
               "you need to decay your type names before using them for lookups. you probably need to decay them anyway if you intend to you them for "
               "anything really, so do it before calling this function.");

        if (const auto* class_ = scope.FindDeclaredClass(std::string{type_name})) {
            return class_->m_pszModule;
        } else if (const auto* enum_ = scope.FindDeclaredEnum(std::string{type_name})) {
            return enum_->m_pszModule;
        } else {
            return std::nullopt;
        }
    }

    /// @return @ref std::nullopt if the type is not contained in a module visible in its scope, e.g. because it is a built-in type
    std::optional<std::string> GetModuleOfType(const CSchemaType& type) {
        if (type.m_pTypeScope != nullptr) {
            return GetModuleOfTypeInScope(*type.m_pTypeScope, DecayTypeName(type.m_pszName));
        } else {
            return std::nullopt;
        }
    }

    /// @param type_name Unqualified
    [[nodiscard]] std::string EscapeTypeName(const codegen::IGenerator& generator, const std::string_view type_name) {
        // This is a hack to support nested types.
        // When we define nested types, they're not actually nested, but contain their outer class' name in their name,
        // e.g. "struct Player { struct Hand {}; };" is emitted as
        // "struct Player {}; struct Player__Hand{};".
        // But when used as a property, types expect `Hand` in `Player`, i.e. `Player::Hand m_hand;`
        // Instead of doing this hackery, we should probably declare nested classes as nested classes.
        return absl::StrReplaceAll(generator.escape_type_name(type_name), {{"::", "_"}});
    }

    /// Adds the module specifier to @p type_name, if @p type_name is declared in @p scope. Otherwise returns @p type_name unmodified.
    std::string MaybeWithModuleName(const codegen::IGenerator& generator, const CSchemaSystemTypeScope& scope, const std::string_view type_name) {
        const auto escaped_type_name = EscapeTypeName(generator, type_name);
        return GetModuleOfTypeInScope(scope, type_name)
            .transform([&](const auto module_name) { return std::format("source2sdk::{}::{}", module_name, escaped_type_name); })
            .value_or(escaped_type_name);
    }

    /// Decomposes a templated type into its components, keeping template
    /// syntax for later reassembly by @ref ReassembleRetypedTemplate().
    /// e.g. "HashMap<int, Vector<float>>" -> ["HashMap", '<', "int", ',', "Vector", '<', "float", '>', '>']
    /// @return std::string for types, char for syntax (',', '<', '>'). Spaces are removed.
    [[nodiscard]]
    std::vector<std::variant<std::string, char>> DecomposeTemplate(std::string_view type_name) {
        // TODO: use a library for this once we have a package manager
        const auto trim = [](std::string_view str) {
            if (const auto found = str.find_first_not_of(' '); found != std::string_view::npos) {
                str.remove_prefix(found);
            } else {
                return std::string_view{};
            }

            if (const auto found = str.find_last_not_of(' '); found != std::string_view::npos) {
                str.remove_suffix(str.size() - (found + 1));
            }

            return str;
        };

        /// Preserves separators in output. Removes space.
        const auto split_trim = [trim](std::string_view str, std::string_view separators) -> std::vector<std::variant<std::string, char>> {
            std::vector<std::variant<std::string, char>> result{};
            std::string_view remainder = str;

            while (true) {
                if (const auto found = remainder.find_first_of(separators); found != std::string_view::npos) {
                    if (const auto part = trim(remainder.substr(0, found)); !part.empty()) {
                        result.emplace_back(std::string{part});
                    }
                    result.emplace_back(remainder[found]);
                    remainder.remove_prefix(found + 1);
                } else {
                    if (const auto part = trim(remainder); !part.empty()) {
                        result.emplace_back(std::string{part});
                    }
                    break;
                }
            }

            return result;
        };

        return split_trim(type_name, "<,>");
    }

    /// e.g. "HashMap<int, CUtlVector<float>>" -> ["HashMap", "int", "CUtlVector", "float"]
    /// @return An empty list if @p type_name is not a template or has no template parameters
    [[nodiscard]]
    std::vector<std::string> ParseTemplateRecursive(std::string_view type_name) {
        std::vector<std::string> result{};

        // remove the topmost type and all syntax entries
        for (const auto& el : DecomposeTemplate(type_name)) {
            if (std::holds_alternative<std::string>(el)) {
                result.emplace_back(std::get<std::string>(el));
            }
        }

        return result;
    }

    std::unique_ptr<codegen::IGenerator> GetGeneratorForLanguage(source2_gen::Language language) {
        switch (language) {
        case source2_gen::Language::cpp:
            return std::make_unique<codegen::generator_cpp_t>();
        case source2_gen::Language::c:
            return std::make_unique<codegen::generator_c_t>();
        case source2_gen::Language::c_ida:
            // c-ida uses the c generator.
            // generator options are adjusted by Dump()
            // postprocessing happens in PostProcessCIDA()
            return std::make_unique<codegen::generator_c_t>();
        }

        assert(false && "unhandled enumerator");
        std::abort();
    }

    /// Adds module qualifiers and resolves built-in types.
    std::string ReassembleRetypedTemplate(const codegen::IGenerator& generator, const CSchemaSystemTypeScope& scope,
                                          const std::vector<std::variant<std::string, char>>& decomposed) {
        std::string result{};

        for (const auto& el : decomposed) {
            std::visit(
                [&](const auto& e) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(e)>, char>) {
                        result += e;
                    } else {
                        if (const auto built_in = generator.find_built_in(e)) {
                            result += built_in.value();
                        } else {
                            // e is a dirty name, e.g. "CPlayer*[10]". We need to add the module, but keep it dirty.
                            const auto type_name = DecayTypeName(e);
                            const auto type_name_with_module = MaybeWithModuleName(generator, scope, type_name);
                            const auto dirty_type_name_with_module = std::string{e}.replace(e.find(type_name), type_name.length(), type_name_with_module);
                            result += dirty_type_name_with_module;
                        }
                    }
                },
                el);
        }

        return result;
    }

    /// @return {type_name, array_sizes} where type_name is a fully qualified name
    std::pair<std::string, std::vector<std::size_t>> GetType(const codegen::IGenerator& generator, const CSchemaType& type) {
        const auto [type_name, array_sizes] = ParseArray(type);

        assert(type_name.empty() == array_sizes.empty());

        const auto type_name_with_modules =
            ReassembleRetypedTemplate(generator, *type.m_pTypeScope, DecomposeTemplate(type_name.empty() ? type.m_pszName : type_name));

        if (!type_name.empty() && !array_sizes.empty())
            return {type_name_with_modules, array_sizes};

        return {type_name_with_modules, {}};
    }

    // We assume that everything that is not a pointer is odr-used.
    // This assumption not correct, e.g. template classes that internally store pointers are
    // not always odr-users of a type. It's good enough for what we do though.
    [[nodiscard]]
    constexpr bool IsOdrUse(std::string_view type_name) {
        return !type_name.contains('*');
    }

    /// @return All names used by @p type. Returns multiple names for template
    /// types.
    [[nodiscard]]
    std::set<NameLookup> GetRequiredNamesForType(const CSchemaType& type) {
        // m_pTypeScope can be nullptr for built-in types
        if (type.m_pTypeScope != nullptr) {
            std::set<NameLookup> result{};

            const auto destructured = ParseTemplateRecursive(type.m_pszName);

            // This is a slight hack to break dependency cycles. Some template types don't odr-use
            // their template arguments during their declaration, think std::unique_ptr.
            // There's no foolproof way to detect those template types, so we're hardcoding the ones that are
            // known to cause circular dependencies.
            static std::unordered_set<std::string_view> non_odr_containers{"CHandle"};
            bool is_used_in_non_odr_container = false;

            for (const auto& dirty_type_name : destructured) {
                const auto type_name = DecayTypeName(dirty_type_name);

                if (auto module{GetModuleOfTypeInScope(*type.m_pTypeScope, type_name)}) {
                    const auto source =
                        (!is_used_in_non_odr_container && IsOdrUse(dirty_type_name)) ? NameSource::include : NameSource::forward_declaration;

                    result.emplace(NameLookup{.module = std::move(module.value()), .type_name = std::string{type_name}, .source = source});
                }

                is_used_in_non_odr_container = non_odr_containers.contains(type_name);
            }

            return result;
        } else {
            return {};
        }
    }

    /// @return All names that are required to define @p classes
    std::set<NameLookup> GetRequiredNamesForClass(const CSchemaClassBinding& class_) {
        std::set<NameLookup> result{};

        for (const auto& field : std::span{class_.m_pFields, static_cast<std::size_t>(class_.m_nFieldSize)}) {
            const auto names = GetRequiredNamesForType(*field.m_pSchemaType);
            result.insert(names.begin(), names.end());
        }

        if (const auto* base_classes = class_.m_pBaseClasses; base_classes != nullptr) {
            assert(base_classes->m_pClass->m_pSchemaType != nullptr && "didn't think this could happen, feel free to touch");
            // source2gen doesn't support multiple inheritance, only check class[0]
            const auto includes = GetRequiredNamesForType(*base_classes[0].m_pClass->m_pSchemaType);
            result.insert(includes.begin(), includes.end());
        }

        const auto is_self = [self_module{GetModuleOfType(*class_.m_pSchemaType).value()}, self_type_name{class_.GetName()}](const auto& that) {
            return (that.module == self_module) && (that.type_name == self_type_name);
        };

        // don't forward-declare or include self. happens for self-referencing types, e.g. entity2::CEntityComponentHelper
        if (const auto found = std::ranges::find_if(result, is_self); found != result.end()) {
            result.erase(found);
        }

        return result;
    }

    [[nodiscard]]
    ClassAssemblyState AssembleBitfield(codegen::IGenerator& generator, ClassAssemblyState&& state) {
        state.assembling_bitfield = false;

        std::size_t exact_bitfield_size_bits = 0;
        for (const auto& entry : state.bitfield) {
            exact_bitfield_size_bits += entry.size;
        }
        const auto type_name = field_parser::guess_bitfield_type(exact_bitfield_size_bits);

        generator.begin_bitfield_block();

        for (const auto& entry : state.bitfield) {
            for (const auto& field_metadata : entry.metadata) {
                if (auto data = GetMetadataValue(field_metadata); data.empty())
                    generator.comment(std::format("metadata: {}", field_metadata.m_szName));
                else
                    generator.comment(std::format("metadata: {} \"{}\"", field_metadata.m_szName, data));
            }

            generator.prop(codegen::Prop{.type_name = type_name, .name = entry.name, .bitfield_size = entry.size}, true);
        }

        generator.end_bitfield_block(false).reset_tabs_count().comment(std::format("{:d} bits", exact_bitfield_size_bits)).restore_tabs_count();

        state.bitfield.clear();
        state.last_field_offset = state.last_field_offset.value_or(0) + state.last_field_size.value_or(0);
        // call to bit_ceil() relies on guess_bitfield_type() returning the next highest power of 2
        state.last_field_size = std::bit_ceil(std::max(std::size_t{8}, exact_bitfield_size_bits)) / 8;

        return state;
    }

    /// Does not insert a pad if it would have size 0
    void InsertPadUntil(codegen::IGenerator::self_ref generator, const ClassAssemblyState& state, std::int32_t offset, bool verbose) {
        if (verbose) {
            generator.comment(std::format("last_field_offset={} last_field_size={}",
                                          state.last_field_offset.transform(&util::to_hex_string).value_or("none"),
                                          state.last_field_size.transform(&util::to_hex_string).value_or("none")));
        }

        const auto expected_offset = state.last_field_offset.value_or(0) + state.last_field_size.value_or(0);

        // insert padding only if needed
        if (expected_offset < static_cast<std::int64_t>(offset) && !state.assembling_bitfield) {
            generator
                .struct_padding(
                    codegen::Padding{
                        .pad_offset = expected_offset,
                        .size = codegen::Padding::Bytes{static_cast<std::size_t>(offset - expected_offset)},
                    },
                    false)
                .reset_tabs_count()
                .comment(std::format("{:#x}", expected_offset))
                .restore_tabs_count();
        }
    }

    void AssembleClass(const source2_gen::Options& options, sdk::GeneratorCache& cache, codegen::IGenerator::self_ref generator,
                       const CSchemaClassBinding& class_) {
        static constexpr std::size_t source2_max_align = 8;

        // TODO: when we have a CLI parser: pass this property in from the outside
        constexpr bool verbose = false;

        struct cached_datamap_t {
            std::string type_;
            std::string name_;
            std::ptrdiff_t offset_;
        };

        // @note: @es3n1n: get class info, assemble it
        //
        const auto* class_parent = class_.m_pBaseClasses ? class_.m_pBaseClasses->m_pClass : nullptr;
        const auto class_size = class_.GetSize();
        const auto class_alignment = GetClassAlignmentRecursive(cache.class_alignment, class_);
        // Source2 has alignof(max_align_t)=8, i.e. every class whose size is a multiple of 8 is aligned.
        const auto class_is_aligned = (class_size % class_alignment.value_or(source2_max_align)) == 0;
        const auto is_struct = util::IsStruct(class_.m_pszName);

        if (!class_is_aligned) {
            const auto warning = [&]() {
                if (class_alignment.has_value()) {
                    // ceil size to next possible aligned size
                    const auto aligned_size = class_size + (class_alignment.value() - (class_size % class_alignment.value())) % class_alignment.value();

                    return std::format("Type {} is misaligned. Its size should be {:#x}, but with proper alignment it has size {:#x}.", class_.GetName(),
                                       class_size, aligned_size);
                } else {
                    return std::format("Type {} appears to be misaligned. Its alignment is unknown and it is not aligned to max_align_t ({}).",
                                       class_.GetName(), source2_max_align);
                }
            }();
            warn(warning);
            generator.comment(warning);
            generator.comment("It has been replaced by a dummy. You can try uncommenting the struct below.");
            generator.begin_struct(class_.GetName());
            generator.struct_padding(codegen::Padding{.pad_offset = 0, .size = codegen::Padding::Bytes{static_cast<std::size_t>(class_size)}}, false);
            generator.end_struct();
        }

        PrintClassInfo(cache, generator, class_);

        // @note: @es3n1n: get parent name
        //
        const std::string parent_class_name =
            (class_parent != nullptr) ? MaybeWithModuleName(generator, *class_parent->m_pTypeScope, class_parent->m_pszName) : "";
        const std::optional<std::ptrdiff_t> parent_class_size = class_parent ? std::make_optional(class_parent->m_nSizeOf) : std::nullopt;

        if (!class_is_aligned) {
            generator.begin_multi_line_comment();
        }

        // @note: @es3n1n: field assembling state
        //
        ClassAssemblyState state = {.last_field_size = parent_class_size};

        // @note: @es3n1n: if we need to pad first field or if there's no fields in this class
        // and we need to properly pad it to make sure its size is the same as we expect it
        //
        const auto* first_field = (class_.m_pFields == nullptr) ? nullptr : &class_.m_pFields[0];
        const std::optional<std::ptrdiff_t> first_field_offset =
            (first_field != nullptr) ? std::make_optional(first_field->m_nSingleInheritanceOffset) : std::nullopt;

        // @todo: @es3n1n: if for some mysterious reason this class describes fields
        // of the base class we should handle it too.
        if ((class_parent != nullptr) && first_field_offset.has_value() && first_field_offset.value() < parent_class_size.value()) {
            const auto warning = std::format("Collision detected: {} and its base {} have {:#x} overlapping byte(s)", class_.GetName(), parent_class_name,
                                             parent_class_size.value() - first_field_offset.value());
            warn(warning);
            generator.comment(warning);
            state.collision_end_offset = parent_class_size.value();
        }

        // @note: @es3n1n: start class
        //
        generator.pack_push(1); // we are aligning stuff ourselves
        if (is_struct) {
            if (class_parent != nullptr) {
                generator.begin_struct_with_base_type(class_.m_pszName, parent_class_name);
            } else {
                generator.begin_struct(class_.m_pszName);
            }
        } else {
            if (class_parent != nullptr) {
                generator.begin_class_with_base_type(class_.m_pszName, parent_class_name);
            } else {
                generator.begin_class(class_.m_pszName);
            }
        }

        /// If fields cannot be emitted, e.g. because of collisions, they're added to
        /// this set so we can ignore them when asserting offsets.
        std::unordered_set<std::string> skipped_fields{};
        std::list<std::pair<std::string, std::ptrdiff_t>> cached_fields{};
        std::list<cached_datamap_t> cached_datamap_fields{};

        for (const auto& field : class_.GetFields()) {
            // Fall back to size=1 because there are no 0-sized types.
            // `RenderPrimitiveType_t` is the only type (in CS2 9035763) without size information.
            const auto field_size = field.m_pSchemaType->GetSize().value_or(1);
            const auto field_alignment = GetAlignmentOfTypeRecursive(cache.class_alignment, *field.m_pSchemaType);

            // @note: @es3n1n: parsing type
            //
            // type_name is fully qualified
            const auto [type_name, array_sizes] = GetType(generator, *field.m_pSchemaType);
            const auto var_info = field_parser::parse(generator, type_name, field.m_pszName, array_sizes);

            // @fixme: @es3n1n: todo proper collision fix and remove this block
            if (state.collision_end_offset && field.m_nSingleInheritanceOffset < state.collision_end_offset) {
                skipped_fields.emplace(field.m_pszName);
                // A warning has already been logged at the start of the class
                generator.comment(
                    std::format("Skipped field \"{}\" @ {:#x} because of the struct collision", field.m_pszName, field.m_nSingleInheritanceOffset));
                generator.comment("", false)
                    .reset_tabs_count()
                    .prop(codegen::Prop{.type_category = GetTypeCategory(field), .type_name = var_info.m_type, .name = var_info.formatted_name()})
                    .restore_tabs_count();
                continue;
            }

            // Collect all bitfield entries and emit them later. We need to know
            // how large the bitfield is in order to choose the right type. We
            // only know how large the bitfield is once we've reached its end.
            if (var_info.is_bitfield()) {
                if (!state.assembling_bitfield) {
                    state.assembling_bitfield = true;
                    state.bitfield_start = field.m_nSingleInheritanceOffset;
                }

                state.bitfield.emplace_back(BitfieldEntry{
                    .name = var_info.m_name,
                    .size = var_info.m_bitfield_size,
                    .metadata = std::vector(field.m_pMetadata, field.m_pMetadata + field.m_nMetadataSize),
                });
                continue;
            }

            // At this point, we're never still inside a bitfield. If `assembling_bitfield` is set, that means we're at the first field following a
            // bitfield, but the bitfield has not been emitted yet.
            // note: in CS2, there are no types with padding before a bitfield
            InsertPadUntil(generator, state, state.assembling_bitfield ? state.bitfield_start : field.m_nSingleInheritanceOffset, verbose);

            // This is the first field after a bitfield, i.e. the active bitfield has ended. Emit the bitfield we have collected.
            if (state.assembling_bitfield) {
                state = AssembleBitfield(generator, std::move(state));

                // We need another pad here because the current loop iteration is already on a non-bitfield field which will get emitted right away.
                InsertPadUntil(generator, state, field.m_nSingleInheritanceOffset, verbose);
            }

            // @note: @es3n1n: dump metadata
            //
            for (auto j = 0; j < field.m_nMetadataSize; j++) {
                const auto field_metadata = field.m_pMetadata[j];

                if (auto data = GetMetadataValue(field_metadata); data.empty())
                    generator.comment(std::format("metadata: {}", field_metadata.m_szName));
                else
                    generator.comment(std::format("metadata: {} \"{}\"", field_metadata.m_szName, data));
            }

            // @note: @es3n1n: update state
            //
            state.last_field_offset = field.m_nSingleInheritanceOffset;
            state.last_field_size = static_cast<std::size_t>(field_size);

            if (const auto e_class = field.m_pSchemaType->GetAsDeclaredClass(); e_class != nullptr && e_class->m_pClassInfo == nullptr) {
                // missing class info

                /// @note: @es3n1n: game bug:
                ///     There are some classes that have literally no info about them in schema,
                ///     for these fields we'll just insert a pad.
                generator.comment(std::format("game bug: prop with no declared class info ({})", e_class->m_pszName));
                generator.prop(codegen::Prop{.type_category = codegen::TypeCategory::built_in,
                                             .type_name = "char",
                                             .name = std::format("{}[{:#x}]", var_info.m_name, field_size)},
                               false);
            } else if ((field.m_nSingleInheritanceOffset % field_alignment.value_or(source2_max_align)) != 0) {
                // misaligned field

                const auto warning =
                    field_alignment.has_value() ?
                        std::format("Property {}::{} is misaligned.", class_.GetName(), field.m_pszName) :
                        std::format("Property {}::{} appears to be misaligned. Its alignment is unknown and it is not aligned to max_align_t ({}).",
                                    class_.GetName(), field.m_pszName, source2_max_align);
                warn(warning);
                generator.comment(warning);
                generator.prop(codegen::Prop{.type_category = codegen::TypeCategory::built_in,
                                             .type_name = "char",
                                             .name = std::format("{}[{:#x}]", var_info.m_name, field_size)},
                               true);
                generator.comment("", false)
                    .reset_tabs_count()
                    .prop(codegen::Prop{.type_category = GetTypeCategory(field), .type_name = var_info.m_type, .name = var_info.formatted_name()}, false)
                    .restore_tabs_count();
            } else if (std::string{field.m_pSchemaType->m_pszName}.contains('<')) {
                // template type

                // This is a workaround to get the size of template types right.
                // There are types that have non-type template parameters, e.g.
                // `CUtlLeanVectorFixedGrowable<int, 10>`. The non-type template parameter affects the size of the template type, but the schema system
                // doesn't store information about non-type template parameters. The schema system just says `CUtlLeanVectorFixedGrowable<int>`, which
                // is insufficient to generate a `CUtlLeanVectorFixedGrowable` with correct size.`
                // To still keep the rest of the class in order, we replace all template fields with char arrays.
                // We're applying this workaround to all template type, even those that don't have non-type template parameters, because we can't tell
                // them apart. So we're certainly commenting out more than is necessary.
                generator.comment(std::format(
                    "{} has a template type with potentially unknown template parameters. You can try uncommenting the field below.", var_info.m_name));
                generator.comment("", false);
                generator.reset_tabs_count()
                    .prop(codegen::Prop{.type_category = GetTypeCategory(field), .type_name = var_info.m_type, .name = var_info.formatted_name()}, true)
                    .restore_tabs_count();
                generator.prop(codegen::Prop{.type_category = codegen::TypeCategory::built_in,
                                             .type_name = "char",
                                             .name = std::format("{}[{:#x}]", var_info.m_name, field_size)},
                               false);
            } else {
                // This is the "all normal, all good" `prop()` call
                generator.prop(codegen::Prop{.type_category = GetTypeCategory(field), .type_name = var_info.m_type, .name = var_info.formatted_name()},
                               false);
            }

            if (verbose) {
                generator.reset_tabs_count()
                    .comment(std::format("type.name=\"{}\" offset={:#x} size={:#x} alignment={}", std::string_view{field.m_pSchemaType->m_pszName},
                                         field.m_nSingleInheritanceOffset, field_size,
                                         field_alignment.transform(&util::to_hex_string).value_or("unknown")),
                             false)
                    .restore_tabs_count();
            } else {
                generator.reset_tabs_count().comment(std::format("{:#x}", field.m_nSingleInheritanceOffset), false).restore_tabs_count();
            }
            cached_fields.emplace_back(var_info.formatted_name(), field.m_nSingleInheritanceOffset);

            generator.next_line();
        }

        // @note: @es3n1n: if struct ends with bitfield we should end bitfield before ending the class
        //
        if (state.assembling_bitfield) {
            state = AssembleBitfield(generator, std::move(state));
        }

        // pad the class end.
        const auto last_field_end = state.last_field_offset.value_or(0) + state.last_field_size.value_or(0);
        const auto end_pad = class_size - last_field_end;

        // The `(class_size != 1)` check is here because of empty classes. If
        // we generated a pad for empty classes, they'd no longer have standard-layout.
        // The pad isn't necessary for such classes, because the compiler will make them have size=1.
        if ((end_pad != 0) && (class_size != 1)) {
            generator.struct_padding(
                codegen::Padding{
                    .pad_offset = last_field_end,
                    .size = codegen::Padding::Bytes{static_cast<std::size_t>(end_pad)},
                },
                true);
        } else if (static_cast<std::size_t>(class_size) < static_cast<std::size_t>(last_field_end)) [[unlikely]] {
            throw std::runtime_error{std::format("{} overflows by {:#x} byte(s). Its last field ends at {:#x}, but {} ends at {:#x}", class_.GetName(),
                                                 -end_pad, last_field_end, class_.GetName(), class_size)};
        }

        // The current class may be defined in multiple scopes. It doesn't matter which one we use, as all definitions are the same..
        // TODO: verify the above statement. Are static fields really shared between scopes?
        const std::string scope_name{class_.m_pTypeScope->BGetScopeName()};

        if (class_.m_pFieldMetadataOverrides && class_.m_pFieldMetadataOverrides->m_iTypeDescriptionCount > 1) {
            const auto& dm = class_.m_pFieldMetadataOverrides;

            for (std::uint64_t s = 0; s < dm->m_iTypeDescriptionCount; s++) {
                auto* t = &dm->m_pTypeDescription[s];
                if (!t)
                    continue;

                if (t->GetFieldName().empty())
                    continue;

                const auto var_info = field_parser::parse(t->m_iFieldType, t->GetFieldName(), t->m_nFieldSize);

                std::string field_type = var_info.m_type;
                if (t->m_iFieldType == fieldtype_t::FIELD_EMBEDDED) {
                    field_type = t->m_pDataMap->m_pszClassName;
                }

                std::string field_name = var_info.formatted_name();

                // @note: @og: if schema dump already has this field, then just skip it

                if (const auto it =
                        std::ranges::find_if(cached_fields, [t, field_name](const auto& f) { return f.first == field_name && f.second == t->m_iOffset; });
                    it != cached_fields.end())
                    continue;

                cached_datamap_fields.emplace_back(field_type, field_name, t->m_iOffset);
            }

            if (!cached_datamap_fields.empty()) {
                if (class_.m_nFieldSize)
                    generator.next_line();

                generator.comment("Datamap fields:");
                for (auto& [field_type, field_name, field_offset] : cached_datamap_fields) {
                    generator.comment(std::format("{} {}; // {:#x}", field_type, field_name, field_offset));
                }
            }
        }

        if (!class_.m_nFieldSize && !class_.m_nStaticMetadataSize)
            generator.comment("No schema binary for binding");

        if (is_struct) {
            generator.end_struct();
        } else {
            generator.end_class();
        }

        generator.pack_pop();
        generator.next_line();

        const bool is_standard_layout_class = IsStandardLayoutClass(cache.class_has_standard_layout, class_);

        if (options.static_assertions) {
            // TODO: when we have a CLI parser: allow users to generate assertions in non-standard-layout classes. Those assertions are
            // conditionally-supported by compilers.
            if (is_standard_layout_class) {
                for (const auto& field :
                     class_.GetFields() | std::ranges::views::filter([&](const auto& e) { return !skipped_fields.contains(e.m_pszName); })) {
                    if (field.m_pSchemaType->m_unTypeCategory == ETypeCategory::Schema_Bitfield) {
                        generator.comment(std::format("Cannot assert offset of bitfield {}::{}", class_.m_pszName, field.m_pszName));
                    } else {
                        generator.static_assert_offset(MaybeWithModuleName(generator, *class_.m_pTypeScope, class_.m_pszName), field.m_pszName,
                                                       field.m_nSingleInheritanceOffset);
                    }
                }
            } else {
                if (class_.m_nFieldSize != 0) {
                    generator.comment(std::format("Cannot assert offsets of fields in {} because it is not a standard-layout class", class_.m_pszName));
                }
            }
        }

        if (!class_is_aligned) {
            generator.end_multi_line_comment();
        }

        generator.next_line();

        if (options.static_assertions) {
            generator.static_assert_size(MaybeWithModuleName(generator, *class_.m_pTypeScope, class_.m_pszName), class_size);
        }
    }

    [[nodiscard]]
    std::filesystem::path GetFilePathForType(const codegen::IGenerator& generator, std::string_view module_name, std::string_view type_name) {
        return std::format("{}/include/{}/{}/{}.{}", kOutDirName, kIncludeDirName, module_name, EscapeTypeName(generator, DecayTypeName(type_name)),
                           generator.get_file_extension());
    }

    /// @return Path to the generated file
    std::filesystem::path GenerateEnumSdk(const source2_gen::Options& options, std::string_view module_name, const CSchemaEnumBinding& enum_) {
        // @note: @es3n1n: init codegen
        //
        auto p_generator = GetGeneratorForLanguage(options.emit_language);
        auto& generator = *p_generator;

        generator.preamble();

        // @note: @es3n1n: print banner
        //
        generator.next_line()
            .comment("/////////////////////////////////////////////////////////////")
            .comment(std::format("Module: {}", module_name))
            .comment(std::string{kCreatedBySource2genMessage})
            .comment("/////////////////////////////////////////////////////////////")
            .next_line();

        generator.begin_namespace("source2sdk");
        generator.begin_namespace(module_name);

        // @note: @es3n1n: assemble props
        //
        AssembleEnum(generator, enum_);

        generator.end_namespace();
        generator.end_namespace();

        // @note: @es3n1n: write generated data to output file
        //
        if (!std::filesystem::exists(kOutDirName))
            std::filesystem::create_directories(kOutDirName);

        const auto out_file_path = GetFilePathForType(generator, module_name, enum_.m_pszName).string();

        std::ofstream f(out_file_path, std::ios::out);
        f << generator.str();
        if (!f.good()) {
            std::cerr << std::format("Could not write to {}: {}", out_file_path, std::strerror(errno)) << std::endl;
            // This std::exit() is bad. Instead, we could return the dumped
            // header name and content to the caller in a std::expected. Let the
            // caller write the file. That would also allow the caller to choose
            // the output directory and handle errors.
            std::exit(1);
        }

        return out_file_path;
    }

    /// @return Path to the generated file
    std::filesystem::path GenerateClassSdk(const source2_gen::Options& options, sdk::GeneratorCache& cache, std::string_view module_name,
                                           const CSchemaClassBinding& class_) {
        // @note: @es3n1n: init codegen
        //
        auto p_generator = GetGeneratorForLanguage(options.emit_language);
        auto& generator = *p_generator;

        generator.preamble();

        const auto names = GetRequiredNamesForClass(class_);

        for (const auto& include : names | std::views::filter([](const auto& el) { return el.source == NameSource::include; })) {
            generator.include(std::format("{}/{}/{}", kIncludeDirName, include.module, EscapeTypeName(generator, include.type_name)),
                              codegen::IncludeOptions{
                                  .local = true,
                                  .system = false,
                              });
        }

        for (const auto& forward_declaration : names | std::views::filter([](const auto& el) { return el.source == NameSource::forward_declaration; })) {
            generator.begin_namespace("source2sdk");
            generator.begin_namespace(forward_declaration.module);
            generator.forward_declaration(forward_declaration.type_name);
            generator.end_namespace();
            generator.end_namespace();
        }

        // @note: @es3n1n: print banner
        //
        generator.next_line()
            .comment("/////////////////////////////////////////////////////////////")
            .comment(std::format("Module: {}", module_name))
            .comment(std::string{kCreatedBySource2genMessage})
            .comment("/////////////////////////////////////////////////////////////")
            .next_line();

        generator.begin_namespace("source2sdk");
        generator.begin_namespace(module_name);

        // @note: @es3n1n: assemble props
        //
        AssembleClass(options, cache, generator, class_);

        generator.end_namespace();
        generator.end_namespace();

        // @note: @es3n1n: write generated data to output file
        //
        const auto out_file_path = GetFilePathForType(generator, module_name, class_.m_pszName).string();
        std::ofstream f(out_file_path, std::ios::out);
        f << generator.str();
        if (!f.good()) {
            std::cerr << std::format("Could not write to {}: {}", out_file_path, std::strerror(errno)) << std::endl;
            // This std::exit() is bad. Instead, we could return the dumped
            // header name and content to the caller in a std::expected. Let the
            // caller write the file. That would also allow the caller to choose
            // the output directory and handle errors.
            std::exit(1);
        }

        return out_file_path;
    }
} // namespace

namespace sdk {
    GeneratorResult GenerateTypeScopeSdk(const source2_gen::Options& options, GeneratorCache& cache, std::string_view module_name,
                                         const std::unordered_set<const CSchemaEnumBinding*>& enums,
                                         const std::unordered_set<const CSchemaClassBinding*>& classes) {
        // @note: @es3n1n: print debug info
        //
        std::cout << std::format("{}: Assembling module {} with {} enum(s) and {} class(es)", __FUNCTION__, module_name, enums.size(), classes.size())
                  << std::endl;

        const std::filesystem::path out_directory_path = std::format("{}/include/{}/{}", kOutDirName, kIncludeDirName, module_name);

        if (!std::filesystem::exists(out_directory_path))
            std::filesystem::create_directories(out_directory_path);

        GeneratorResult result{};

        std::ranges::for_each(enums, [&](const auto* el) { result.generated_files.emplace(GenerateEnumSdk(options, module_name, *el)); });
        std::ranges::for_each(classes, [&](const auto* el) { result.generated_files.emplace(GenerateClassSdk(options, cache, module_name, *el)); });

        return result;
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
