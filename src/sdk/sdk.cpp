// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.

// ReSharper disable CppClangTidyClangDiagnosticLanguageExtensionToken
#include "sdk/sdk.h"
#include <filesystem>
#include <list>
#include <ranges>
#include <span>
#include <string_view>
#include <unordered_set>
#include <variant>
#include <vector>

namespace {
    using namespace std::string_view_literals;

    constexpr std::string_view kOutDirName = "sdk"sv;
    constinit std::array include_paths = {"<cstdint>"sv, "\"source2gen_user_types.hpp\""sv};

    constexpr uint32_t kMaxReferencesForClassEmbed = 2;
    constexpr std::size_t kMinFieldCountForClassEmbed = 2;
    constexpr std::size_t kMaxFieldCountForClassEmbed = 12;

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
        FNV32("MPulseProvideFeatureTag"),
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
    std::string GetMetadataValue(const SchemaMetadataEntryData_t& metadata_entry) {
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
    struct class_t {
        CSchemaClassInfo* target_{};
        std::set<CSchemaClassInfo*> refs_;
        std::uint32_t used_count_{};
        std::list<std::pair<std::string, std::ptrdiff_t>> cached_fields_;

        struct cached_datamap_t {
            std::string type_;
            std::string name_;
            std::ptrdiff_t offset_;
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

            // auto ptr = type->GetRefClass();
            // if (ptr && ptr->m_nTypeCategory == Schema_DeclaredClass)
            // {
            // 	refs_.insert(ptr->m_pClassInfo);
            // 	return;
            // }
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

    namespace {
        void PrintClassInfo(codegen::generator_t::self_ref builder, CSchemaClassBinding* class_info) {
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
                    builder.comment(std::format("static metadata: {} \"{}\"", metadata.m_szName, value));
                else
                    builder.comment(std::format("static metadata: {}", metadata.m_szName));
            }
        }

        void PrintEnumInfo(codegen::generator_t::self_ref builder, const CSchemaEnumBinding* enum_binding) {
            builder.comment(std::format("Enumerator count: {}", enum_binding->m_nEnumeratorCount))
                .comment(std::format("Alignment: {}", enum_binding->m_unAlignOf))
                .comment(std::format("Size: {:#x}", enum_binding->m_unSizeOf));

            if (enum_binding->m_nStaticMetadataSize > 0)
                builder.comment("");

            for (const auto& metadata : enum_binding->GetStaticMetadata()) {
                builder.comment(std::format("metadata: {}", metadata.m_szName));
            }
        }

        void AssembleEnums(codegen::generator_t::self_ref builder, const std::unordered_set<const CSchemaEnumBinding*>& enums) {
            for (auto schema_enum_binding : enums) {
                // @note: @es3n1n: get type name by align size
                //
                const auto get_type_name = [schema_enum_binding]() -> std::string {
                    std::string type_storage;

                    switch (schema_enum_binding->m_unAlignOf) {
                    case 1:
                        type_storage = "std::uint8_t";
                        break;
                    case 2:
                        type_storage = "std::uint16_t";
                        break;
                    case 4:
                        type_storage = "std::uint32_t";
                        break;
                    case 8:
                        type_storage = "std::uint64_t";
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
        auto parse_array(const CSchemaType& type) -> std::pair<std::string, std::vector<std::size_t>> {
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
        };

        /// @return @ref std::nullopt if the type is not contained in a module visible in @p scope
        auto get_module_of_type_in_scope(const CSchemaSystemTypeScope& scope, std::string_view type_name) -> std::optional<std::string> {
            // resolve pointers to their actual type
            while (type_name.ends_with('*')) {
                type_name.remove_suffix(1);
            }

            if (const auto* class_ = scope.FindDeclaredClass(std::string{type_name})) {
                return class_->m_pszModule;
            } else if (const auto* enum_ = scope.FindDeclaredEnum(std::string{type_name})) {
                return enum_->m_pszModule;
            } else {
                return std::nullopt;
            }
        }

        /// @return @ref std::nullopt if the type is not contained in a module visible in its scope, e.g. because it is a built-in type
        auto get_module_of_type(const CSchemaType& type) -> std::optional<std::string> {
            if (type.m_pTypeScope != nullptr) {
                return get_module_of_type_in_scope(*type.m_pTypeScope, type.m_pszName);
            } else {
                return std::nullopt;
            }
        };

        // TODO: consider moving these functions to field_parser
        /// Decomposes a templated type into its componets, keeping template syntax for later reassembly.
        /// e.g. "HashMap<int, Vector<float>>" -> ["HashMap", '<', "int", ',', "Vector", '<', "float", '>', '>']
        /// @return std::string for types, char for syntax (',', '<', '>'). Spaces are removed.
        [[nodiscard]]
        std::vector<std::variant<std::string, char>> decompose_template(std::string_view type_name) {
            // TODO: use a library for this once we have a package manager
            const auto trim = [](std::string_view str) {
                if (const auto found = str.find_first_not_of(" "); found != std::string_view::npos) {
                    str.remove_prefix(found);
                } else {
                    return std::string_view{};
                }

                if (const auto found = str.find_last_not_of(" "); found != std::string_view::npos) {
                    str.remove_suffix(str.size() - (found + 1));
                }

                return str;
            };

            // TODO: remove
            assert(trim("A") == "A");
            assert(trim(" A ") == "A");
            assert(trim(" A") == "A");
            assert(trim("A ") == "A");
            assert(trim(" ") == "");
            assert(trim("  ") == "");

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

        // TOOD: use a library
        [[nodiscard]]
        auto string_replace(std::string str, std::string_view search, std::string_view replace) -> std::string {
            const std::size_t pos = str.find(search);
            if (pos != std::string::npos) {
                str.replace(pos, search.length(), replace);
            }
            return str;
        }

        /// Adds module qualifiers and resolves built-in types
        auto reassemble_retyped_template(const CSchemaSystemTypeScope& scope,
                                         const std::vector<std::variant<std::string, char>>& decomposed) -> std::string {
            const auto maybe_with_module_name = [&scope](const std::string_view type_name) {
                // This is a hack to support nested types.
                // When we define nested types, they're not actually nested, but contain their outer class' name in their name,
                // e.g. "struct Player { struct Hand {}; };" is emitted as
                // "struct Player {}; struct Player__Hand{};".
                // But when used as a property, types expect `Hand` in `Player`, i.e. `Player::Hand m_hand;`
                // Instead of doing this hackery, we should probably declare nested classes as nested classes.
                const auto escaped_type_name = string_replace(std::string{type_name}, "::", "__");

                return get_module_of_type_in_scope(scope, type_name)
                    .transform([&](const auto module_name) { return std::format("{}::{}", module_name, escaped_type_name); })
                    .value_or(std::string{escaped_type_name});
            };

            std::string result{};

            for (const auto& el : decomposed) {
                std::visit(
                    [&](const auto& e) {
                        if constexpr (std::is_same_v<std::decay_t<decltype(e)>, char>) {
                            result += e;
                        } else {
                            if (const auto built_in = field_parser::type_name_to_cpp(e)) {
                                result += built_in.value();
                            } else {
                                result += maybe_with_module_name(e);
                            }
                        }
                    },
                    el);
            }

            // std::cout << "-> " << result << '\n';

            return result;
        }

        /// @return {type_name, array_sizes} where type_name is a fully qualified name
        auto get_type(const CSchemaType& type) -> std::pair<std::string, std::vector<std::size_t>> {
            const auto [type_name, array_sizes] = parse_array(type);

            assert(type_name.empty() == array_sizes.empty());

            const auto type_name_with_modules =
                reassemble_retyped_template(*type.m_pTypeScope, decompose_template(type_name.empty() ? type.m_pszName : type_name));

            if (!type_name.empty() && !array_sizes.empty())
                return {type_name_with_modules, array_sizes};

            return {type_name_with_modules, {}};
        };

        // TOOD: move up
        struct OrderedClasses {
            std::list<std::string> forward_declarations{};
            std::list<class_t> classes{};
        };

        /// e.g. "HashMap<int, CUtlVector<float>>" -> ["HashMap", "int", "CUtlVector", "float"]
        /// @return An empty list if @p type_name is not a template or has no template parameters
        [[nodiscard]]
        std::vector<std::string> parse_template_recursive(std::string_view type_name) {
            assert((decompose_template("vector<int>") == std::vector<std::variant<std::string, char>>{"vector", '<', "int", '>'}));
            assert((decompose_template("vector<hashmap<int, float>>") ==
                    std::vector<std::variant<std::string, char>>{"vector", '<', "hashmap", '<', "int", ',', "float", '>', '>'}));
            assert((decompose_template("int") == std::vector<std::variant<std::string, char>>{"int"}));

            std::vector<std::string> result{};

            // remove the topmost type and all syntax entries
            for (const auto& el : decompose_template(type_name)) {
                if (std::holds_alternative<std::string>(el)) {
                    result.emplace_back(std::move(std::get<std::string>(el)));
                }
            }

            return result;
        }

        // TOOD: need to sort these fuctions, they're all over the place

        /// @return All modules directly required to declare this type. Returns multiple
        /// modules for template types.
        [[nodiscard]]
        auto get_modules_of_type(const CSchemaType& type) -> std::unordered_set<std::string> {
            if (type.m_pTypeScope != nullptr) {
                std::unordered_set<std::string> result{};
                for (const auto& type_name : parse_template_recursive(type.m_pszName)) {
                    if (auto module{get_module_of_type_in_scope(*type.m_pTypeScope, type_name)}) {
                        result.emplace(std::move(module.value()));
                    }
                }
                return result;
            } else {
                return {};
            }
        }

        /**
         * Orderes classes such that dependencies are declared or defined befor dependees.
         */
        [[nodiscard]]
        OrderedClasses OrderClasses(const std::unordered_set<const CSchemaClassBinding*>& classes) {
            // TOOD: remove tests
            assert((parse_template_recursive("vector<int>") == std::vector<std::string>{"vector", "int"}));
            assert((parse_template_recursive("vector") == std::vector<std::string>{"vector"}));
            assert((parse_template_recursive("map<int, float>") == std::vector<std::string>{"map", "int", "float"}));
            assert((parse_template_recursive("A< B< C > >") == std::vector<std::string>{"A", "B", "C"}));

            /// @return Lifetime is bound to string viewed by @p type_name
            const auto decay_type_name = [](std::string_view type_name) -> std::string_view {
                if (const auto found = type_name.find('['); found != std::string_view::npos) {
                    // "array[123]" -> "array"
                    type_name = type_name.substr(0, found);
                }
                if (const auto found = type_name.find('*'); found != std::string_view::npos) {
                    // "pointer***" -> "pointer"
                    type_name = type_name.substr(0, found);
                }

                return type_name;
            };

            std::list<std::string> forward_declarations{};
            std::list<class_t> ordered_classes{};

            for (const auto* schema_class_binding : classes) {
                assert(schema_class_binding != nullptr);

                const auto class_info = schema_class_binding->m_pTypeScope->FindDeclaredClass(schema_class_binding->m_pszName);

                auto& class_dump = ordered_classes.emplace_back();
                class_dump.target_ = class_info;
            }

            for (auto& class_dump : ordered_classes) {
                const auto& class_info = *class_dump.target_;

                // forward declare all classes that are not nested. nested classes cannot be forward declared.
                // @todo: maybe we need to forward declare only pointers to classes?
                if (!class_info.GetName().contains("::")) {
                    forward_declarations.emplace_back(class_info.GetName());
                }

                for (const auto& field : std::span{class_info.m_pFields, static_cast<std::size_t>(class_info.m_nFieldSize)}) {
                    auto ref_class = field.m_pSchemaType->GetRefClass();

                    class_dump.AddRefToClass(field.m_pSchemaType);

                    const auto full_type_name = decay_type_name(std::string_view{field.m_pSchemaType->m_pszName});

                    // if `full_type_name` is `Map<int, float>`, this loop runs [`Map`, `int`, 'float`]
                    for (const auto& type_name : parse_template_recursive(full_type_name)) {
                        if (const auto found = std::ranges::find_if(classes, [=](const auto& el) { return el->m_pszName == type_name; });
                            found != classes.end()) {
                            class_dump.AddRefToClass((*found)->m_pSchemaType);
                        }
                    }

                    auto field_class = std::ranges::find_if(ordered_classes, [field](const class_t& cls) {
                        return cls.target_ == reinterpret_cast<CSchemaType_DeclaredClass*>(field.m_pSchemaType)->m_pClassInfo;
                    });
                    if (field_class != ordered_classes.end())
                        field_class->used_count_++;
                }
            }

            // to detect inderict dependency cycles
            std::set<CSchemaClassBinding*> hot_classes{};
            std::set<CSchemaClassBinding*> previous_hot_classes{};

            bool did_change = false;
            do {
                did_change = false;

                // swap until we done.
                for (auto first = ordered_classes.begin(); first != ordered_classes.end(); ++first) {
                    bool second_below_first = false;

                    for (auto second = ordered_classes.begin(); second != ordered_classes.end(); ++second) {
                        if (second == first) {
                            second_below_first = true;
                            continue;
                        }

                        // swap if second class below first, and first depends on second.
                        const bool first_depend = first->IsDependsOn(*second);

                        // swap if first class below second, and second depends on first.
                        const bool second_depend = second->IsDependsOn(*first);

                        if (first_depend && second_depend) {
                            // classes depends on each other, forward declare them.
                            // @todo: verify that cyclic dependencies is a pointers.
                            continue;
                        }

                        if (second_below_first ? first_depend : second_depend) {
                            hot_classes.emplace(first->target_);
                            hot_classes.emplace(second->target_);

                            std::iter_swap(first, second);
                            did_change = true;
                        }
                    }
                }

                if (did_change && (hot_classes == previous_hot_classes)) {
                    std::cout << "detected dependency cycle in the following classes\n";
                    for (const auto* class_ : hot_classes) {
                        std::cout << std::format("- {}::{}\n", class_->m_pszModule, class_->m_pszName);
                    }
                    std::cout << "you need to resolve the dependency cycle by hand in the generated sdk\n";
                    break;
                }
                std::swap(hot_classes, previous_hot_classes);
                hot_classes.clear();
            } while (did_change);

            return OrderedClasses{
                .forward_declarations = forward_declarations,
                .classes = ordered_classes,
            };
        }

        void AssembleClasses(codegen::generator_t::self_ref builder, const std::unordered_set<const CSchemaClassBinding*>& classes) {

            // @note: @soufiw:
            // sort all classes based on refs and inherit, and then print it.
            // ==================
            const auto ordered_classes = OrderClasses(classes);

            for (const auto& type_name : ordered_classes.forward_declarations) {
                builder.forward_declaration(type_name);
            }

            if (!ordered_classes.forward_declarations.empty()) {
                builder.next_line();
            }

            auto classes_to_dump = ordered_classes.classes;

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
                        .struct_padding(parent_class_size, expected_pad_size, false, true)
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
                    // fall back to 1 because there are no 0-sized types
                    //
                    const int field_size = field.m_pSchemaType->GetSize().value_or(1);

                    // @note: @es3n1n: parsing type
                    //
                    const auto [type_name, array_sizes] = get_type(*field.m_pSchemaType);
                    const auto var_info = field_parser::parse(type_name, field.m_pszName, array_sizes);

                    // @note: @es3n1n: insert padding if needed
                    //
                    const auto expected_offset = state.last_field_offset + state.last_field_size;
                    if (state.last_field_offset && state.last_field_size &&
                        expected_offset < static_cast<std::uint64_t>(field.m_nSingleInheritanceOffset) && !state.assembling_bitfield) {

                        builder.access_modifier("private")
                            .struct_padding(expected_offset, field.m_nSingleInheritanceOffset - expected_offset, false, true)
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
                            builder.struct_padding(std::nullopt, 0, true, false, expected_union_size_bits - actual_union_size_bits);

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
                            builder.comment(std::format("metadata: {}", field_metadata.m_szName));
                        else
                            builder.comment(std::format("metadata: {} \"{}\"", field_metadata.m_szName, data));
                    }

                    // if this prop is a non-pointer class, check if its worth directly embedding the accumulated offset of it into the metadata
                    const auto prop_class =
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
                    builder.prop(var_info.m_type, var_info.formatted_name(), false);
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
                        builder.struct_padding(std::nullopt, 0, true, false, expected_union_size_bits - actual_union_size_bits);

                    builder.end_bitfield_block(false).reset_tabs_count().comment(std::format("{:d} bits", expected_union_size_bits)).restore_tabs_count();

                    state.total_bits_count_in_union = 0;
                    state.assembling_bitfield = false;
                }

                // @note: @es3n1n: dump static fields
                //
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

                    auto [type, mod] = get_type(*static_field->m_pSchemaType);
                    const auto var_info = field_parser::parse(type, static_field->m_pszName, mod);
                    builder.static_field_getter(var_info.m_type, var_info.m_name, scope_name, class_info->m_pszName, s);
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

                builder.end_block();
            }
        }

    } // namespace

    /// @param exclude_module_name will not be contained in the return value
    /// @return All modules that are required by properties of @p classes
    std::set<std::string> find_required_modules_of_class(std::string_view exclude_module_name, const CSchemaClassBinding& class_) {
        std::set<std::string> result{};

        for (const auto& field : std::span{class_.m_pFields, static_cast<std::size_t>(class_.m_nFieldSize)}) {
            const auto modules = get_modules_of_type(*field.m_pSchemaType);
            result.insert(modules.begin(), modules.end());
        }

        for (const auto& field : std::span{class_.m_pStaticFields, static_cast<std::size_t>(class_.m_nStaticFieldsSize)}) {
            const auto modules = get_modules_of_type(*field.m_pSchemaType);
            result.insert(modules.begin(), modules.end());
        }

        if (const auto found = result.find(std::string{exclude_module_name}); found != result.end()) {
            result.erase(found);
        }

        return result;
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

    void GenerateTypeScopeSdk(std::string_view module_name, const std::unordered_set<const CSchemaEnumBinding*>& enums,
                              const std::unordered_set<const CSchemaClassBinding*>& classes) {
        // @note: @es3n1n: print debug info
        //
        std::cout << std::format("{}: Assembling {}", __FUNCTION__, module_name) << std::endl;

        // @note: @es3n1n: build file path
        //
        if (!std::filesystem::exists(kOutDirName))
            std::filesystem::create_directories(kOutDirName);
        const std::string out_file_path = std::format("{}/{}.hpp", kOutDirName, module_name);

        // @note: @es3n1n: init codegen
        //
        auto builder = codegen::get();
        builder.pragma("once");

        // @note: @es3n1n: include files
        //
        for (const auto& required_module : find_required_modules(module_name, classes)) {
            builder.include(std::format("\"{}.hpp\"", required_module));
        }

        for (auto&& include_path : include_paths)
            builder.include(include_path.data());

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
        AssembleClasses(builder, classes);

        builder.end_namespace();

        // @note: @es3n1n: write generated data to output file
        //
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
