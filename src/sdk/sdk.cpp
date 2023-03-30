#include "sdk/sdk.h"
#include <filesystem>
#include <set>
#include <string_view>

namespace {
    using namespace std::string_view_literals;

    constexpr std::string_view kOutDirName = "sdk"sv;
    constexpr std::initializer_list<std::string_view> kIncludePaths = {"<cstdint>"sv, "\"!GlobalTypes.hpp\""sv};

    constexpr std::initializer_list<fnv32::hash> kStringMetadataEntries = {
        FNV32("MNetworkChangeCallback"),  FNV32("MPropertyFriendlyName"), FNV32("MPropertyDescription"),
        FNV32("MPropertyAttributeRange"), FNV32("MPropertyStartGroup"),   FNV32("MPropertyAttributeChoiceName"),
        FNV32("MPropertyGroupName"),      FNV32("MNetworkUserGroup"),     FNV32("MNetworkAlias"),
        FNV32("MNetworkTypeAlias"),       FNV32("MNetworkSerializer"),    FNV32("MPropertyAttributeEditor"),
        FNV32("MPropertySuppressExpr"),   FNV32("MKV3TransferName"),
    };

    constexpr std::initializer_list<fnv32::hash> kIntegerMetadataEntries = {
        FNV32("MNetworkVarEmbeddedFieldOffsetDelta"),
        FNV32("MNetworkBitCount"),
        FNV32("MNetworkPriority"),
        FNV32("MPropertySortPriority"),
    };

    constexpr std::initializer_list<fnv32::hash> kFloatMetadataEntries = {
        FNV32("MNetworkMinValue"),
        FNV32("MNetworkMaxValue"),
    };

    inline bool ends_with(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
    }
} // namespace

namespace sdk {
    std::unordered_map<int, std::set<std::string>> type_sizes;

    namespace {
        __forceinline void PrintClassInfo(codegen::generator_t::self_ref builder, std::int16_t alignment, std::int16_t size) {
            builder.comment(std::format("Alignment: {}", alignment)).comment(std::format("Size: {:#x}", size));
        }

        void PrintEnum(codegen::generator_t::self_ref builder, CSchemaEnumBinding* schema_enum_binding, std::set<CSchemaEnumBinding*>& parsed_enums) {
            // @note: @es3n1n: get type name by align size
            //
            const auto get_type_name = [schema_enum_binding]() [[msvc::forceinline]] {
                std::string type_storage;

                switch (schema_enum_binding->m_align_) {
                case 1:
                    type_storage = "uint8_t";
                    break;
                case 2:
                    type_storage = "uint16_t";
                    break;
                case 4:
                    type_storage = "uint32_t";
                    break;
                case 8:
                    type_storage = "uint64_t";
                    break;
                default:
                    type_storage = "uINVALID_TYPE";
                }

                return type_storage;
            };

            parsed_enums.emplace(schema_enum_binding);

            // @todo: @es3n1n: assemble flags
            //
            // if (schema_enum_binding->m_flags_) out.print("// Flags: MEnumFlagsWithOverlappingBits\n");

            // @note: @es3n1n: print meta info
            //
            PrintClassInfo(builder, schema_enum_binding->m_align_, schema_enum_binding->m_size_);

            // @note: @es3n1n: begin enum class
            //
            builder.begin_enum_class(schema_enum_binding->m_binding_name_, get_type_name());

            // @note: @es3n1n: assemble enum items
            //
            for (auto l = 0; l < schema_enum_binding->m_size_; l++) {
                auto& field = schema_enum_binding->m_enum_info_[l];
                switch (schema_enum_binding->m_align_) {
                case 1:
                    builder.enum_item(field.m_name, field.m_value == std::numeric_limits<uint8_t>::max() ? -1 : (uint8_t)field.m_value);
                    break;
                case 2:
                    builder.enum_item(field.m_name, field.m_value == std::numeric_limits<uint16_t>::max() ? -1 : (uint16_t)field.m_value);
                    break;
                case 4:
                    builder.enum_item(field.m_name, field.m_value == std::numeric_limits<uint32_t>::max() ? -1 : (uint32_t)field.m_value);
                    break;
                case 8:
                    builder.enum_item(field.m_name, field.m_value == std::numeric_limits<uint64_t>::max() ? -1 : (uint64_t)field.m_value);
                    break;
                default:
                    continue;
                }
            }

            // @note: @es3n1n: we are done with this enum
            //
            builder.end_enum_class();
        }

        void AssembleEnums(codegen::generator_t::self_ref builder, std::set<CSchemaEnumBinding*>& parsed_enums, CUtlTSHash<CSchemaEnumBinding*> enums) {
            for (auto schema_enum_binding : enums.GetElements()) {
                PrintEnum(builder, schema_enum_binding, parsed_enums);
            }
        }

        struct class_t {
            // all classes that will be dumped.
            inline static std::list<class_t> classes_to_dump;

            // fast access by class info pointer.
            inline static std::unordered_map<CSchemaClassInfo*, class_t*> class_info_pointers;

            // fast access by name.
            inline static std::unordered_map<std::string_view, class_t*> name_to_class;

            CSchemaClassInfo* target_;
            std::set<CSchemaClassInfo*> refs_;
            std::vector<SchemaClassFieldData_t*> fields_;

            class_t() = default;

            class_t(CSchemaClassInfo* info): target_(info) { }

            void Parse(codegen::generator_t::self_ref builder, std::set<CSchemaEnumBinding*>& parsed_enums, bool& did_forward_decls);

            CSchemaClassInfo* GetParent() {
                if (!target_->m_schema_parent)
                    return nullptr;

                return target_->m_schema_parent->m_class;
            }

            void AddRefToClass(CSchemaType* type) {
                if (type->type_category == Schema_DeclaredClass) {
                    refs_.insert(type->m_class_info);
                    return;
                }

                auto ptr = type->GetArrayType();
                if (ptr && ptr->type_category == Schema_DeclaredClass) {
                    refs_.insert(ptr->m_class_info);
                    return;
                }
            }

            bool IsDependsOn(const class_t& other) {
                // if current class inherit other.
                auto parent = this->GetParent();
                if (parent == other.target_)
                    return true;

                // if current class contains ref to other.
                if (this->refs_.contains(other.target_))
                    return true;

                // otherwise, order doesn`t matter.
                return false;
            }

            SchemaClassFieldData_t* GetFirstField() {
                if (!fields_.empty())
                    return fields_.front();
                return nullptr;
            }

            // @note: @es3n1n: Returns the struct size without its parent's size
            //
            std::ptrdiff_t ClassSizeWithoutParent() {
                if (CSchemaClassInfo* class_parent = this->GetParent(); class_parent)
                    return this->target_->m_size - class_parent->m_size;
                return this->target_->m_size;
            }

            // @note: if class is derived, get offset where its starts
            //
            std::intptr_t GetStartOffset() {
                auto parent = this->GetParent();
                if (!parent)
                    return 0;

                // expect that we start derived class where parent ends.
                return parent->m_size;
            }

            static void SortClasses();
        };

        void class_t::SortClasses() {
            bool did_change = false;
            do {
                did_change = false;

                // swap until we done.
                for (auto first = class_t::classes_to_dump.begin(); first != class_t::classes_to_dump.end(); ++first) {
                    bool second_below_first = false;

                    for (auto second = class_t::classes_to_dump.begin(); second != class_t::classes_to_dump.end(); ++second) {
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

                        bool swap = second_below_first ? first_depend : second_depend;
                        if (swap) {
                            std::iter_swap(first, second);
                            did_change = true;
                        }
                    }
                }
            } while (did_change);
        }

        void class_t::Parse(codegen::generator_t::self_ref builder, std::set<CSchemaEnumBinding*>& parsed_enums, bool& did_forward_decls) {
            class_info_pointers[target_] = this;
            name_to_class[target_->m_name] = this;

            // @note: parse parent at first.
            auto p = GetParent();
            class_t* parent_dump = nullptr;

            if (p) {
                if (!class_info_pointers.contains(p)) {
                    if (!name_to_class.contains(p->m_name)) {
                        auto& class_dump = classes_to_dump.emplace_back(p);
                        class_dump.Parse(builder, parsed_enums, did_forward_decls);
                    } else {
                        std::cout << "\n";
                    }
                }

                auto it = class_info_pointers.find(p);
                if (it != class_info_pointers.end()) {
                    parent_dump = it->second;
                } else {
                    auto it = name_to_class.find(p->m_name);
                    if (it != name_to_class.end())
                        parent_dump = it->second;
                }
            }

            auto start_offset = this->GetStartOffset();

            for (auto k = 0; k < target_->m_align; k++) {
                const auto field = &target_->m_fields[k];
                if (!field)
                    continue;

                auto actual_type = field->m_type->GetRefClass()->GetArrayType()->GetRefClass();
                if (actual_type->type_category == Schema_Bitfield)
                    continue;

                if (actual_type->type_category == Schema_DeclaredEnum && !parsed_enums.contains(actual_type->m_enum_binding_)) {
                    parsed_enums.emplace(actual_type->m_enum_binding_);
                    PrintEnum(builder, actual_type->m_enum_binding_, parsed_enums);
                }

                // @note: for some reason, derived classes sometimes describes field of
                // base classes, so check if offset of this field is lower.
                auto target_dump = this;

                if (parent_dump && start_offset > field->m_single_inheritance_offset) {
                    std::cout << std::format("skipped collision: {}->{}\n", target_->m_name, field->m_name);
                    continue;
#if 0
                    // @note: verify that base class have enough space.
                    // @todo: fix this for bitfield!
                    int field_size = 0;
                    if (field->m_type->GetSize(&field_size)) {
                        auto field_end = field->m_single_inheritance_offset + field_size;
                        if (field_end > p->m_size)
                            continue;
                    
                        target_dump = parent_dump;
                    
                        // @note: check if some of derived classes already pushed field.
                        if (std::find(target_dump->fields_.begin(), target_dump->fields_.end(), field) != target_dump->fields_.end()) {
                            continue;
                        }
                    }
#endif
                }

                // forward declare all classes.
                // @todo: maybe we need to forward declare only pointers to classes?

                if (actual_type->type_category == Schema_DeclaredClass) {
                    builder.forward_declartion(actual_type->m_name_);
                    did_forward_decls = true;

                    auto cl = actual_type->m_class_info;
                    int f_size = 0;
                    if (!actual_type->GetSize(&f_size))
                        continue;

                    if (cl) {
                        if (!class_info_pointers.contains(cl)) {
                            if (!name_to_class.contains(cl->m_name)) {
                                auto& class_dump = classes_to_dump.emplace_back(cl);
                                class_dump.Parse(builder, parsed_enums, did_forward_decls);
                            } else {
                                std::cout << "\n";
                            }
                        }
                    }
                }

                target_dump->fields_.push_back(field);
                target_dump->AddRefToClass(field->m_type);
            }
        }

        void AssembleClasses(CSchemaSystemTypeScope* current, codegen::generator_t::self_ref builder, std::set<CSchemaEnumBinding*>& parsed_enums,
                             CUtlTSHash<CSchemaClassBinding*> classes) {
            // @note: @soufiw:
            // sort all classes based on refs and inherit, and then print it.
            // ==================
            class_t::classes_to_dump.clear();
            class_t::class_info_pointers.clear();
            class_t::name_to_class.clear();

            bool did_forward_decls = false;
            for (const auto schema_class_binding : classes.GetElements()) {
                const auto class_info = current->FindDeclaredClass(schema_class_binding->m_binary_name);
                if (class_t::class_info_pointers.contains(class_info))
                    continue;

                if (class_t::name_to_class.contains(schema_class_binding->m_binary_name))
                    continue;

                auto& class_dump = class_t::classes_to_dump.emplace_back(class_info);
                class_dump.Parse(builder, parsed_enums, did_forward_decls);
            }

            if (did_forward_decls)
                builder.next_line();

            class_t::SortClasses();
            // ==================

            // returns {type_name, array_sizes}
            auto parse_array = [&](CSchemaType* type) -> std::pair<std::string, std::vector<std::size_t>> {
                auto ptr = type->GetRefClass();
                auto actual_type = ptr ? ptr : type;

                std::string base_type;
                std::vector<std::size_t> sizes;

                if (actual_type->type_category == Schema_FixedArray) {
                    // dump all sizes.
                    auto schema = actual_type;
                    while (true) {
                        sizes.emplace_back(schema->m_array_.array_size);
                        schema = schema->m_array_.element_type_;

                        if (schema->type_category != Schema_FixedArray) {
                            base_type = schema->m_name_;
                            break;
                        }
                    }
                }

                return {base_type, sizes};
            };

            // returns {type_name, array_sizes}
            auto get_type = [&](CSchemaType* type) -> std::pair<std::string, std::vector<std::size_t>> {
                auto [type_name, mods] = parse_array(type);

                assert((!type_name.empty() && !mods.empty()) || (type_name.empty() && mods.empty()));

                if (!type_name.empty() && !mods.empty())
                    return {type_name, mods};

                auto name_wt = std::string_view(type->m_name_);

                auto start = name_wt.find('<');
                auto end = name_wt.find_last_of('>');

                if (start != end && start != std::string::npos) {
                    name_wt = std::string_view(type->m_name_, start);
                }

                int size = 0;
                type->GetSize(&size);

                if (name_wt.contains("CBitVec")) {
                    int size = 0;
                    if (type->GetSize(&size)) {
                        switch (size) {
                        case 1:
                            return {"uint8_t", {}};
                        case 2:
                            return {"uint16_t", {}};
                        case 4:
                            return {"uint32_t", {}};
                        case 8:
                            return {"uint64_t", {}};
                        default:
                            return {"uint8_t", {(size_t)size}};
                        }
                    }
                }

                if (name_wt == ("CUtlVectorEmbeddedNetworkVar")) {
                    if (size == 0x50)
                        return {type->m_name_, {}};

                    return {"uint8_t", {(size_t)size}};
                }

                if (name_wt == ("C_UtlVectorEmbeddedNetworkVar")) {
                    if (size == 0x50)
                        return {type->m_name_, {}};

                    return {"uint8_t", {(size_t)size}};
                }

                if (name_wt.contains("CUtlVector")) {
                    int size = 0;
                    if (type->GetSize(&size)) {
                        switch (size) {
                        case 8:
                            return {"uintptr_t", {}};
                        case 0x10:
                            return {"CUtlVectorBasic", {}};
                        case 0x14:
                            return {"CUtlVector", {}};
                        default:
                            return {"uint8_t", {(size_t)size}};
                        }
                    }
                }

                if (name_wt.contains("CNetworkUtlVectorBase")) {
                    int size = 0;
                    if (type->GetSize(&size)) {
                        switch (size) {
                        case 8:
                            return {"uintptr_t", {}};
                        case 0x10:
                            return {"CUtlVectorBasic", {}};
                        case 0x18:
                            return {"CNetworkUtlVectorBase", {}};
                        default:
                            return {"uint8_t", {(size_t)size}};
                        }
                    }
                }
                if (name_wt.contains("C_NetworkUtlVectorBase")) {
                    int size = 0;
                    if (type->GetSize(&size)) {
                        switch (size) {
                        case 8:
                            return {"uintptr_t", {}};
                        case 0x10:
                            return {"CUtlVectorBasic", {}};
                        case 0x18:
                            return {"C_NetworkUtlVectorBase", {}};
                        default:
                            return {"uint8_t", {(size_t)size}};
                        }
                    }
                }

                return {type->m_name_, {}};
            };

            for (auto& class_dump : class_t::classes_to_dump) {
                // @note: @es3n1n: get class info, assemble it
                //
                const auto class_parent = class_dump.GetParent();
                const auto class_info = class_dump.target_;
                const auto is_struct = true; // ends_with(class_info->m_name, "_t");
                PrintClassInfo(builder, class_info->m_align, class_info->m_size);

                // if (std::string_view(class_dump.target_->m_name).contains("CParticleFunctionInitializer")) {
                //     std::cout << "debugme\n";
                // }

                auto cl_align = class_info->m_class_alignment;
                if (cl_align == -1) // @use default align then
                    cl_align = sizeof(uintptr_t);

                if (cl_align != sizeof(uintptr_t))
                    builder.pragma(std::format("pack(push, {})", cl_align));

                // @note: @es3n1n: get parent name
                //
                std::string parent_cls_name;
                if (auto parent = class_info->m_schema_parent ? class_info->m_schema_parent->m_class : nullptr; parent)
                    parent_cls_name = parent->m_name;

                // @note: @es3n1n: start class
                //
                if (is_struct)
                    builder.begin_struct_with_base_type(class_info->m_name, parent_cls_name, "");
                else
                    builder.begin_class_with_base_type(class_info->m_name, parent_cls_name, "");

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
                    first_field_offset = first_field->m_single_inheritance_offset;

                const std::ptrdiff_t parent_class_size = class_parent ? class_parent->m_size : 0;

                std::ptrdiff_t expected_pad_size = first_field_offset.value_or(class_dump.target_->m_size);
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

                // @note: @es3n1n: begin public members
                //
                builder.access_modifier("public");

                bool did_init = false;

                std::vector<std::string> offset_asserts;

                for (auto field : class_dump.fields_) {
                    // @note: @es3n1n: some more utils
                    //
                    auto get_metadata_type = [&](SchemaMetadataEntryData_t metadata_entry) -> std::string {
                        std::string value;

                        const auto value_hash_name = fnv32::hash_runtime(metadata_entry.m_name);

                        // clang-format off
                        if (std::find(kStringMetadataEntries.begin(), kStringMetadataEntries.end(), value_hash_name) != kStringMetadataEntries.end())
                            value = metadata_entry.m_value->m_sz_value;
                        else if (std::find(kIntegerMetadataEntries.begin(), kIntegerMetadataEntries.end(), value_hash_name) != kIntegerMetadataEntries.end())
                            value = std::to_string(metadata_entry.m_value->m_n_value);
                        else if (std::find(kFloatMetadataEntries.begin(), kFloatMetadataEntries.end(), value_hash_name) != kFloatMetadataEntries.end())
                            value = std::to_string(metadata_entry.m_value->m_f_value);
                        // clang-format on

                        return value;
                    };

                    // @note: @es3n1n: obtaining size
                    //
                    int field_size = 0;
                    if (!field->m_type->GetSize(&field_size)) // @note: @es3n1n: should happen if we are attempting to get a size of the bitfield
                        field_size = 0;

                    // @note: @es3n1n: parsing type
                    //
                    auto [type, mod] = get_type(field->m_type);
                    {
                        auto start = type.find('<');
                        auto end = type.find_last_of('>');

                        if (start != end && start != std::string::npos) {
                            type.erase(start, end);
                        }
                    }

                    const auto var_info = field_parser::parse(type, field->m_name, mod);

                    // @note: @es3n1n: insert padding if needed
                    //
                    const auto expected_offset = state.last_field_offset + state.last_field_size;
                    if (did_init && expected_offset < field->m_single_inheritance_offset && !state.assembling_bitfield) {

                        builder.access_modifier("private")
                            .struct_padding(expected_offset, field->m_single_inheritance_offset - expected_offset, false, true)
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
                        const auto expected_union_size_bytes = field->m_single_inheritance_offset - state.last_field_offset;
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
                    for (auto j = 0; j < field->m_metadata_size; j++) {
                        auto field_metadata = field->m_metadata[j];

                        if (auto data = get_metadata_type(field_metadata); data.empty())
                            builder.comment(field_metadata.m_name);
                        else
                            builder.comment(std::format("{} \"{}\"", field_metadata.m_name, data));
                    }

                    // @note: @es3n1n: update state
                    //
                    if (field->m_type->type_category != Schema_Bitfield && field_size) {
                        state.last_field_offset = field->m_single_inheritance_offset;
                        state.last_field_size = static_cast<std::size_t>(field_size);

                        did_init = true;
                    }

                    if (var_info.is_bitfield())
                        state.total_bits_count_in_union += var_info.m_bitfield_size;

                    // @note: @es3n1n: push prop
                    //
                    builder.prop(var_info.m_type, var_info.formatted_name(), false);
                    if (!var_info.is_bitfield()) {
                        offset_asserts.push_back(std::format(" offsetof( {}, {} ) == {:#x} ", builder.escape_name(class_info->m_name), var_info.m_name,
                                                             field->m_single_inheritance_offset));

                        builder.reset_tabs_count().comment(std::format("{:#x}", field->m_single_inheritance_offset), false).restore_tabs_count();
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

                if (did_init) {
                    auto end_gap_start = state.last_field_size + state.last_field_offset;

                    if (class_info->m_size > end_gap_start) {
                        auto end_gap_size = class_info->m_size - end_gap_start;
                        builder.struct_padding(end_gap_start, end_gap_size, true, false);
                    }
                }

#if 0
                // @note: @es3n1n: dump static fields
                //
                if (class_info->m_static_size) {
                    if (class_info->m_align)
                        builder.next_line();
                    builder.comment("Static fields:");
                }
                for (auto s = 0; s < class_info->m_static_size; s++) {
                    auto static_field = &class_info->m_static_fields[s];

                    auto [type, mod] = get_type(static_field->m_type);
                    const auto var_info = field_parser::parse(type, static_field->name, mod);
                    builder.static_field_getter(var_info.m_type, var_info.m_name, current->GetScopeName().data(), class_info->m_name, s);
                }
#endif

                if (!class_info->m_align && !class_info->m_static_size)
                    builder.comment("No members available");

                builder.end_block();
                for (const auto& off : offset_asserts)
                    builder.push_static_assert(off);

                builder.push_static_assert(std::format(" sizeof( {} ) == {:#x} ", builder.escape_name(class_info->m_name), class_info->m_size));

                if (cl_align != sizeof(uintptr_t))
                    builder.pragma(std::format("pack(pop)"));

                builder.next_line();
            }
        }
    } // namespace

    void GenerateTypeScopeSdk(CSchemaSystemTypeScope* current) {
        // @note: @es3n1n: getting current scope name & formatting it
        //
        constexpr std::string_view dll_extension = ".dll";
        auto scope_name = current->GetScopeName();
        if (ends_with(scope_name.data(), dll_extension.data()))
            scope_name.remove_suffix(dll_extension.size());

        // @note: @es3n1n: print debug info
        //
        std::cout << std::format("{}: Assembling {}", __FUNCTION__, scope_name) << std::endl;

        // @note: @es3n1n: build file path
        //
        if (!std::filesystem::exists(kOutDirName))
            std::filesystem::create_directories(kOutDirName);
        const std::string out_file_path = std::format("{}\\{}.hpp", kOutDirName, scope_name);

        // @note: @es3n1n: init codegen
        //
        auto builder = codegen::get();
        builder.pragma("once");

        // @note: @es3n1n: include files
        //
        // for (auto&& include_path : kIncludePaths)
        //     builder.include(include_path.data());

        // @note: @es3n1n: get stuff from schema that we'll use later
        //
        const auto current_classes = current->GetClasses();
        const auto current_enums = current->GetEnums();

        // @note: @es3n1n: print banner
        //
        builder.next_line()
            .comment("/////////////////////////////////////////////////////////////")
            .comment(std::format("Binary: {}", current->GetScopeName()))
            .comment(std::format("Classes count: {}", current_classes.Count()))
            .comment(std::format("Enums count: {}", current_enums.Count()))
            .comment(kCreatedBySource2genMessage.data())
            .comment("/////////////////////////////////////////////////////////////")
            .next_line();

        // @note: @es3n1n: assemble props
        //
        std::set<CSchemaEnumBinding*> parsed_enums;
        AssembleEnums(builder, parsed_enums, current_enums);
        AssembleClasses(current, builder, parsed_enums, current_classes);

        // @note: lmao.
        if (scope_name == "!GlobalTypes") {
            for (auto& [size, types] : type_sizes) {
                std::cout << std::format("{}: current_size: ", size) << std::endl;

                for (auto& type : types) {
                    std::cout << std::format("\"{}\"", type) << std::endl;
                }

                std::cout << "\n";
            }
        }

        // @note: @es3n1n: write generated data to output file
        //
        std::ofstream f(out_file_path, std::ios::out);
        f << builder.str();
        f.close();
    }
} // namespace sdk
