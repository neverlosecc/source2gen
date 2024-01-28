// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
/**
 * =============================================================================
 * Source2Gen
 * Copyright (C) 2023 neverlose (https://github.com/neverlosecc/source2gen)
 * =============================================================================
 **/

#pragma once

#include <Include.h>
#include <SDK/Interfaces/common/CUtlTSHash.h>
#include <tools/virtual.h>

#define CS2

#ifdef SBOX
// untested, CSchemaType::m_schema_type_ might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define CSCHEMASYSTEM_VALIDATECLASSES 34
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x5420
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x27FC
#elif defined ARTIFACT2
// untested, CSchemaType::m_schema_type_ might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define CSCHEMASYSTEM_VALIDATECLASSES 34
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x5430
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x2804
#elif defined ARTIFACT1
// untested, CSchemaType::m_schema_type_ might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define CSCHEMASYSTEM_VALIDATECLASSES 34
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x4428
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x4B8
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x2001
#elif defined UNDERLORDS
// untested, CSchemaType::m_schema_type_ might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define CSCHEMASYSTEM_VALIDATECLASSES 34
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x5420
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x27FC
#elif defined DESKJOB
    #define CSCHEMATYPE_GETSIZES_INDEX 3
    #define CSCHEMASYSTEM_VALIDATECLASSES 34
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x3A0
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x2804
#elif defined HL_ALYX
    #error "unimplemented"
#elif defined THE_LAB_ROBOT_REPAIR
    #error "unimplemented"
#elif defined(CS2) || defined(DOTA2)
    #define CSCHEMATYPE_GETSIZES_INDEX 3
    #define CSCHEMASYSTEM_VALIDATECLASSES 35
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x190
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x47E
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x2808
    #define SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE 2
#endif

class ISaveRestoreOps;
class CSchemaEnumInfo;
class CSchemaClassInfo;
class CSchemaSystemTypeScope;
class CSchemaType;

struct SchemaMetadataEntryData_t;
struct SchemaClassInfoData_t;

// @note: @og: now CSchemaClassBinding is the same class\structure as CSchemaClassBinding\SchemaClassInfoData_t
using CSchemaClassBinding = CSchemaClassInfo;

// @note: @og: now CSchemaEnumBinding is the same class\structure as CSchemaEnumInfoData\SchemaEnumInfoData_t
using CSchemaEnumBinding = CSchemaEnumInfo;

enum SchemaClassFlags_t {
    SCHEMA_CF1_HAS_VIRTUAL_MEMBERS = 1,
    SCHEMA_CF1_IS_ABSTRACT = 2,
    SCHEMA_CF1_HAS_TRIVIAL_CONSTRUCTOR = 4,
    SCHEMA_CF1_HAS_TRIVIAL_DESTRUCTOR = 8,
    SCHEMA_CF1_HAS_NOSCHEMA_MEMBERS = 16,
    SCHEMA_CF1_IS_PARENT_CLASSES_PARSED = 32,
    SCHEMA_CF1_IS_LOCAL_TYPE_SCOPE = 64,
    SCHEMA_CF1_IS_GLOBAL_TYPE_SCOPE = 128,

#if defined(CS2) || defined(DOTA2)
    SCHEMA_CF1_IS_SCHEMA_VALIDATED = 2048,
#else
    SCHEMA_CF1_IS_SCHEMA_VALIDATED = 1024,
#endif
};

enum ETypeCategory {
    Schema_Builtin = 0,
    Schema_Ptr = 1,
    Schema_Bitfield = 2,
    Schema_FixedArray = 3,
    Schema_Atomic = 4,
    Schema_DeclaredClass = 5,
    Schema_DeclaredEnum = 6,
    Schema_None = 7
};

enum EAtomicCategory {
    Atomic_Basic,
    Atomic_T,
    Atomic_CollectionOfT,
    Atomic_TT,
    Atomic_I,
    Atomic_None
};

struct CSchemaVarName {
    const char* m_name;
    const char* m_type;
};

struct CSchemaNetworkValue {
    union {
        const char* m_p_sz_value;
        int m_n_value;
        float m_f_value;
        std::uintptr_t m_p_value;
        CSchemaVarName m_var_value;
        std::array<char, 32> m_sz_value;
    };
};

struct SchemaMetadataEntryData_t {
    const char* m_name;
    CSchemaNetworkValue* m_value;
};

struct SchemaEnumeratorInfoData_t {
    const char* m_name;

    union {
        unsigned char m_value_char;
        unsigned short m_value_short;
        unsigned int m_value_int;
        unsigned long long m_value;
    };

    std::int32_t m_metadata_size;
    SchemaMetadataEntryData_t* m_metadata;
};

class SchemaEnumInfoData_t {
public:
    SchemaEnumInfoData_t* m_self; // 0x0000
    const char* m_name; // 0x0008
    const char* m_module; // 0x0010
    std::int8_t m_align; // 0x0018
    char pad_0x0019[0x3]; // 0x0019
    std::int16_t m_size; // 0x001C
    std::int16_t m_static_metadata_size; // 0x001E
    SchemaEnumeratorInfoData_t* m_enum_info;
    SchemaMetadataEntryData_t* m_static_metadata;
    CSchemaSystemTypeScope* m_type_scope; // 0x0030
    char pad_0x0038[0x8]; // 0x0038
    std::int32_t m_i_unk1; // 0x0040
};

class CSchemaEnumInfo : public SchemaEnumInfoData_t {
public:
    std::vector<SchemaEnumeratorInfoData_t> GetEnumeratorValues() {
        return {m_enum_info, m_enum_info + m_size};
    }

    std::vector<SchemaMetadataEntryData_t> GetStaticMetadata() {
        return {m_static_metadata, m_static_metadata + m_static_metadata_size};
    }
};

class CSchemaType {
public:
    bool GetSizes(int* out_size1, uint8_t* unk_probably_not_size) {
        return reinterpret_cast<int(__thiscall*)(void*, int*, uint8_t*)>(vftable_[CSCHEMATYPE_GETSIZES_INDEX])(this, out_size1, unk_probably_not_size);
    }
public:
    bool GetSize(int* out_size) {
        uint8_t smh = 0;
        return GetSizes(out_size, &smh);
    }
public:
    std::uintptr_t* vftable_; // 0x0000
    const char* m_name_; // 0x0008

    CSchemaSystemTypeScope* m_type_scope_; // 0x0010
    std::uint8_t type_category; // ETypeCategory 0x0018
    std::uint8_t atomic_category; // EAtomicCategory 0x0019

    // find out to what class pointer points.
    CSchemaType* GetRefClass() const {
        if (type_category != Schema_Ptr)
            return nullptr;

        auto ptr = m_schema_type_;
        while (ptr && ptr->type_category == ETypeCategory::Schema_Ptr)
            ptr = ptr->m_schema_type_;

        return ptr;
    }

    struct array_t {
        std::uint32_t array_size;
        std::uint32_t unknown;
        CSchemaType* element_type_;
    };

    struct atomic_t { // same goes for CollectionOfT
        std::uint64_t gap[2];
        CSchemaType* template_typename;
    };

    struct atomic_tt {
        std::uint64_t gap[2];
        CSchemaType* templates[2];
    };

    struct atomic_i {
        std::uint64_t gap[2];
        std::uint64_t integer;
    };

    // this union depends on CSchema implementation, all members above
    // is from base class ( CSchemaType )
    union // 0x020
    {
        CSchemaType* m_schema_type_;
        CSchemaClassInfo* m_class_info;
        CSchemaEnumBinding* m_enum_binding_;
        array_t m_array_;
        atomic_t m_atomic_t_;
        atomic_tt m_atomic_tt_;
        atomic_i m_atomic_i_;
    };
};
static_assert(offsetof(CSchemaType, m_schema_type_) == 0x20);

struct SchemaClassFieldData_t {
    const char* m_name; // 0x0000
    CSchemaType* m_type; // 0x0008
    std::int32_t m_single_inheritance_offset; // 0x0010
    std::int32_t m_metadata_size; // 0x0014
    SchemaMetadataEntryData_t* m_metadata; // 0x0018
};

struct SchemaStaticFieldData_t {
    const char* m_name; // 0x0000
    CSchemaType* m_type; // 0x0008
    void* m_instance; // 0x0010
    char pad_0x0018[0x10]; // 0x0018
};

struct SchemaBaseClassInfoData_t {
    unsigned int m_offset; // 0x0000
    CSchemaClassInfo* m_prev_by_class; // 0x0008
};

using SchemaFieldMetadataOverrideSetData_t = datamap_t;
using SchemaFieldMetadataOverrideData_t = typedescription_t;

struct SchemaClassInfoData_t {
public:
    enum class SchemaClassInfoFunctionIndex : std::int32_t {
        kRegisterClassSchema = 0,
        kUnknown = 1, // @note: @og: Can't find fn with such index
        kCopyInstance = 2,
        kCreateInstance = 3,
        kDestroyInstance = 4,
        kCreateInstanceWithMemory = 5,
        kDestroyInstanceWithMemory = 6,
        kSchemaDynamicBinding = 7
    };
public:
    SchemaClassInfoData_t* m_self; // 0x0000
    const char* m_name; // 0x0008
    const char* m_module; // 0x0010
    int m_size; // 0x0018
    std::int16_t m_fields_size; // 0x001C
    std::int16_t m_static_fields_size; // 0x001E
    std::int16_t m_static_metadata_size; // 0x0020
    std::uint8_t m_align_of; // 0x0022
    std::uint8_t m_has_base_class; // 0x0023
    std::int16_t m_total_class_size; // 0x0024 // @note: @og: if there no derived or base class then it will be 1 otherwise derived class size + 1.
    std::int16_t m_derived_class_size; // 0x0026
    SchemaClassFieldData_t* m_fields; // 0x0028
    SchemaStaticFieldData_t* m_static_fields; // 0x0030
    SchemaBaseClassInfoData_t* m_base_classes; // 0x0038
    SchemaFieldMetadataOverrideSetData_t* m_field_metadata_overrides; // 0x0040
    SchemaMetadataEntryData_t* m_static_metadata; // 0x0048
    CSchemaSystemTypeScope* m_type_scope; // 0x0050
    CSchemaType* m_schema_type; // 0x0058
    SchemaClassFlags_t m_class_flags:8; // 0x0060
    std::uint32_t m_sequence; // 0x0064 // @note: @og: idk
    void* m_fn; // 0x0068
};

class CSchemaClassInfo : public SchemaClassInfoData_t {
public:
    [[nodiscard]] std::string_view GetName() {
        if (m_name)
            return {m_name};
        return {};
    }

    [[nodiscard]] std::string_view GetModule() {
        if (m_module)
            return {m_module};
        return {};
    }

    std::optional<CSchemaClassInfo*> GetBaseClass() const {
        if (m_has_base_class && m_base_classes)
            return m_base_classes->m_prev_by_class;
        return std::nullopt;
    }

    std::vector<SchemaClassFieldData_t> GetFields() {
        return {m_fields, m_fields + m_fields_size};
    }

    std::vector<SchemaStaticFieldData_t> GetStaticFields() {
        return {m_static_fields, m_static_fields + m_static_fields_size};
    }

    std::vector<SchemaMetadataEntryData_t> GetStaticMetadata() {
        return {m_static_metadata, m_static_metadata + m_static_metadata_size};
    }

    [[nodiscard]] std::string_view GetPrevClassName() const {
        if (!m_base_classes || !m_base_classes->m_prev_by_class)
            return {};
        return m_base_classes->m_prev_by_class->GetName();
    }

    [[nodiscard]] bool HasVirtualTable() const {
        return (m_class_flags & SCHEMA_CF1_HAS_VIRTUAL_MEMBERS) != 0;
    }

    [[nodiscard]] bool RecursiveHasVirtualTable() const {
        return HasVirtualTable() ? true : (m_base_classes && m_base_classes->m_prev_by_class ? m_base_classes->m_prev_by_class->HasVirtualTable() : false);
    }

    [[nodiscard]] bool IsInherits(const std::string_view from) const {
        if (!m_has_base_class || !m_base_classes || !m_base_classes->m_prev_by_class)
            return false;
        if (m_base_classes->m_prev_by_class->GetName() == from)
            return true;
        return false;
    }

    [[nodiscard]] bool IsRecursiveInherits(const std::string_view from) const {
        return IsInherits(from) ? true :
                                  (m_base_classes && m_base_classes->m_prev_by_class ? m_base_classes->m_prev_by_class->IsRecursiveInherits(from) : false);
    }

    [[nodiscard]] int GetSize() const {
        return m_size;
    }

    [[nodiscard]] std::uint8_t GetAligment() const {
        return m_align_of == std::numeric_limits<std::uint8_t>::max() ? 8 : m_align_of;
    }

    // @note: @og: Copy instance from original to new created with all data from original, returns new_instance
    void* CopyInstance(void* instance, void* new_instance) const {
        using Fn = void* (*)(SchemaClassInfoFunctionIndex, void*, void*);
        return reinterpret_cast<Fn>(m_fn)(SchemaClassInfoFunctionIndex::kCreateInstance, instance, new_instance);
    }

    // @note: @og: Creates default instance with engine allocated memory (e.g. if SchemaClassInfoData_t is C_BaseEntity, then Instance will be
    // C_BaseEntity)
    void* CreateInstance() const {
        using Fn = void* (*)(SchemaClassInfoFunctionIndex);
        return reinterpret_cast<Fn>(m_fn)(SchemaClassInfoFunctionIndex::kCreateInstance);
    }

    // @note: @og: Creates default instance with your own allocated memory (e.g. if SchemaClassInfoData_t is C_BaseEntity, then Instance will be
    // C_BaseEntity)
    void* CreateInstance(void* memory) const {
        using Fn = void* (*)(SchemaClassInfoFunctionIndex, void*);
        return reinterpret_cast<Fn>(m_fn)(SchemaClassInfoFunctionIndex::kCreateInstanceWithMemory, memory);
    }

    // @note: @og: Destroy instance (e.g.: C_BaseInstance 1st VT fn with 0 flag)
    void* DestroyInstance(void* instance) const {
        using Fn = void* (*)(SchemaClassInfoFunctionIndex, void*);
        return reinterpret_cast<Fn>(m_fn)(SchemaClassInfoFunctionIndex::kDestroyInstanceWithMemory, instance);
    }

    // @note: @og: Destroy instance with de-allocating memory (e.g.: C_BaseInstance 1st VT fn with 1 flag)
    void* DestroyInstanceWithMemory(void* instance) const {
        using Fn = void* (*)(SchemaClassInfoFunctionIndex, void*);
        return reinterpret_cast<Fn>(m_fn)(SchemaClassInfoFunctionIndex::kDestroyInstanceWithMemory, instance);
    }

    CSchemaClassBinding* SchemaClassBinding(void* entity) const {
        using Fn = CSchemaClassBinding* (*)(SchemaClassInfoFunctionIndex, void*);
        return reinterpret_cast<Fn>(m_fn)(SchemaClassInfoFunctionIndex::kSchemaDynamicBinding, entity);
    }
};

class CSchemaSystemTypeScope {
public:
    CSchemaClassInfo* FindDeclaredClass(const char* class_name) {
#if defined(SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE) && SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE == 2
        CSchemaClassInfo* class_info;

        Virtual::Get<void(__thiscall*)(void*, CSchemaClassInfo**, const char*)>(this, 2)(this, &class_info, class_name);
        return class_info;
#else
        return Virtual::Get<CSchemaClassInfo*(__thiscall*)(void*, const char*)>(this, 2)(this, class_name);
#endif
    }

    CSchemaEnumBinding* FindDeclaredEnum(const char* name) {
        return Virtual::Get<CSchemaEnumBinding*(__thiscall*)(void*, const char*)>(this, 3)(this, name);
    }

    CSchemaType* FindSchemaTypeByName(const char* name, std::uintptr_t* schema) {
        return Virtual::Get<CSchemaType*(__thiscall*)(void*, const char*, std::uintptr_t*)>(this, 4)(this, name, schema);
    }

    CSchemaType* FindTypeDeclaredClass(const char* name) {
        return Virtual::Get<CSchemaType*(__thiscall*)(void*, const char*)>(this, 5)(this, name);
    }

    CSchemaType* FindTypeDeclaredEnum(const char* name) {
        return Virtual::Get<CSchemaType*(__thiscall*)(void*, const char*)>(this, 6)(this, name);
    }

    CSchemaClassBinding* FindRawClassBinding(const char* name) {
        return Virtual::Get<CSchemaClassBinding*(__thiscall*)(void*, const char*)>(this, 7)(this, name);
    }

    CSchemaEnumBinding* FindRawEnumBinding(const char* name) {
        return Virtual::Get<CSchemaEnumBinding*(__thiscall*)(void*, const char*)>(this, 8)(this, name);
    }

    std::string_view GetScopeName() {
        return {m_name_.data()};
    }

    [[nodiscard]] CUtlTSHash<CSchemaClassBinding*> GetClasses() const {
        return m_classes_;
    }

    [[nodiscard]] CUtlTSHash<CSchemaEnumBinding*> GetEnums() const {
        return m_enumes_;
    }
private:
    void* vftable_ = nullptr;
    std::array<char, 256> m_name_ = {};
    char pad_0x0108[SCHEMASYSTEMTYPESCOPE_OFF1] = {}; // 0x0108
    CUtlTSHash<CSchemaClassBinding*> m_classes_; // 0x0588
    char pad_0x0594[SCHEMASYSTEMTYPESCOPE_OFF2] = {}; // 0x05C8
    CUtlTSHash<CSchemaEnumBinding*> m_enumes_; // 0x2DD0
};

class CSchemaSystem {
private:
    /**
     * \brief (class_info->m_class_flags & 64) != 0;
     */
    using SchemaTypeScope_t = std::int32_t;
public:
    CSchemaSystemTypeScope* GlobalTypeScope(void) {
        return Virtual::Get<CSchemaSystemTypeScope*(__thiscall*)(void*)>(this, 11)(this);
    }

    CSchemaSystemTypeScope* FindTypeScopeForModule(const char* module_name) {
        return Virtual::Get<CSchemaSystemTypeScope*(__thiscall*)(void*, const char*, void*)>(this, 13)(this, module_name, nullptr);
    }

    CSchemaSystemTypeScope* GetTypeScopeForBinding(const SchemaTypeScope_t type, const char* binding) {
        return Virtual::Get<CSchemaSystemTypeScope*(__thiscall*)(void*, SchemaTypeScope_t, const char*)>(this, 14)(this, type, binding);
    }

    const char* GetClassInfoBinaryName(CSchemaClassBinding* class_info) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaClassBinding*)>(this, 22)(this, class_info);
    }

    const char* GetClassProjectName(CSchemaClassBinding* class_info) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaClassBinding*)>(this, 23)(this, class_info);
    }

    const char* GetEnumBinaryName(CSchemaEnumBinding* enum_info) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaEnumBinding*)>(this, 24)(this, enum_info);
    }

    const char* GetEnumProjectName(CSchemaEnumBinding* enum_info) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaEnumBinding*)>(this, 25)(this, enum_info);
    }

    CSchemaClassBinding* ValidateClasses(CSchemaClassBinding** class_info) {
        return Virtual::Get<CSchemaClassBinding*(__thiscall*)(void*, CSchemaClassBinding**)>(this, CSCHEMASYSTEM_VALIDATECLASSES)(this, class_info);
    }

    bool SchemaSystemIsReady() {
        return Virtual::Get<bool(__thiscall*)(void*)>(this, 26)(this);
    }

    [[nodiscard]] CUtlVector<CSchemaSystemTypeScope*> GetTypeScopes(void) const {
        return m_type_scopes_;
    }

    [[nodiscard]] std::int32_t GetRegistration() const {
        return m_registrations_;
    }

    [[nodiscard]] std::int32_t GetIgnored() const {
        return m_ignored_;
    }

    [[nodiscard]] std::int32_t GetRedundant() const {
        return m_redundant_;
    }

    [[nodiscard]] std::int32_t GetIgnoredBytes() const {
        return m_ignored_bytes_;
    }
private:
    char pad_0x0000[SCHEMASYSTEM_TYPE_SCOPES_OFFSET]; // 0x0000
    CUtlVector<CSchemaSystemTypeScope*> m_type_scopes_ = {}; // SCHEMASYSTEM_TYPE_SCOPES_OFFSET
    char pad_01A0[288] = {}; // 0x01A0
    std::int32_t m_registrations_ = 0; // 0x02C0
    std::int32_t m_ignored_ = 0; // 0x02C4
    std::int32_t m_redundant_ = 0; // 0x02C8
    char pad_02CC[4] = {}; // 0x02CC
    std::int32_t m_ignored_bytes_ = 0; // 0x02D0
public:
    static CSchemaSystem* GetInstance(void) {
        return sdk::GetInterface<CSchemaSystem>("schemasystem.dll", "SchemaSystem_0");
    }
};

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
