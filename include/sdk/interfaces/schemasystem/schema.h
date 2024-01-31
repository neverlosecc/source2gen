// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
/**
 * =============================================================================
 * Source2Gen
 * Copyright (C) 2023 neverlose (https://github.com/neverlosecc/source2gen)
 * =============================================================================
 **/

#pragma once

#ifdef SBOX
// untested, CSchemaType::m_pSchemaType might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define CSCHEMASYSTEM_VALIDATECLASSES 34
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x5420
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x27FC
#elif defined ARTIFACT2
// untested, CSchemaType::m_pSchemaType might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define CSCHEMASYSTEM_VALIDATECLASSES 34
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x5430
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x2804
#elif defined ARTIFACT1
// untested, CSchemaType::m_pSchemaType might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define CSCHEMASYSTEM_VALIDATECLASSES 34
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x4428
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x4B8
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x2001
#elif defined UNDERLORDS
// untested, CSchemaType::m_pSchemaType might be wrong
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
#else
    #define CSCHEMATYPE_GETSIZES_INDEX 0
    #define CSCHEMASYSTEM_VALIDATECLASSES 0
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0
    #define SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE 0
#endif
#include "SDK/SDK.h"

class ISaveRestoreOps;
class CSchemaEnumInfo;
class CSchemaClassInfo;
class CSchemaSystemTypeScope;
class CSchemaType;
class CSchemaSystem;

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
    const char* m_pszName;
    const char* m_pszType;
};

struct CSchemaNetworkValue {
    union {
        const char* m_pszValue;
        int m_nValue;
        float m_fValue;
        std::uintptr_t m_pPointer;
        CSchemaVarName m_VarValue;
        std::array<char, 32> m_szValue;
    };
};

struct SchemaMetadataEntryData_t {
    const char* m_szName;
    CSchemaNetworkValue* m_pNetworkValue;
};

struct SchemaEnumeratorInfoData_t {
    const char* m_szName;

    union {
        unsigned char m_Char;
        unsigned short m_Short;
        unsigned int m_Int;
        unsigned long long m_Uint;
    };

    std::int32_t m_nMetadataSize;
    SchemaMetadataEntryData_t* m_pMetadata;
};

class SchemaEnumInfoData_t {
public:
    SchemaEnumInfoData_t* m_pSelf; // 0x0000
    const char* m_pszName; // 0x0008
    const char* m_pszModule; // 0x0010
    std::int8_t m_nAlingOf; // 0x0018
private:
    char pad_0x0019[0x3] = {}; // 0x0019
public:
    std::int16_t m_nSize; // 0x001C
    std::int16_t m_nStaticMetadataSize; // 0x001E
    SchemaEnumeratorInfoData_t* m_pEnumInfo;
    SchemaMetadataEntryData_t* m_pStaticMetadata;
    CSchemaSystemTypeScope* m_pTypeScope; // 0x0030
private:
    char pad_0x0038[0x8] = {}; // 0x0038
    std::int32_t m_unknown = 0; // 0x0040
};

class CSchemaEnumInfo : public SchemaEnumInfoData_t {
public:
    std::vector<SchemaEnumeratorInfoData_t> GetEnumeratorValues() {
        return {m_pEnumInfo, m_pEnumInfo + m_nSize};
    }

    std::vector<SchemaMetadataEntryData_t> GetStaticMetadata() {
        return {m_pStaticMetadata, m_pStaticMetadata + m_nStaticMetadataSize};
    }
};

class CSchemaType {
public:
    bool IsValid(void) {
        return Virtual::Get<bool (*)(void*)>(this, 0)(this);
    }

    // @note: @og: actually its formattes string to CBufferString (E.g. <%s, cat %d>) but for some reason I cant get it work, so need to look in to it in
    // future
    std::string_view ToString() {
        static CBufferStringGrowable<1024> szBuf;
        auto res = Virtual::Get<const char*(__thiscall*)(void*, CBufferString*, bool)>(this, 1)(this, &szBuf, false);
        return szBuf.Get();
    }

    void SpewDescription(std::uint32_t unLogChannel, const char* szName) {
        Virtual::Get<void (*)(void*, std::uint32_t, const char*)>(this, 2)(this, unLogChannel, szName);
    }

    // @note: @og: gets size with align
    bool GetSizeWithAlignOf(int* nOutSize, std::uint8_t* unOutAlign) {
        return reinterpret_cast<int (*)(void*, int*, std::uint8_t*)>(vftable[CSCHEMATYPE_GETSIZES_INDEX])(this, nOutSize, unOutAlign);
    }

    // @note: @og: Can be used on CSchemaClassInfo. (Uses multiple inheritance depth verify that current CSchemaType->m_pClassInfo is inherits from pType)
    bool IsA(CSchemaType* pType) {
        return Virtual::Get<bool (*)(void*, CSchemaType*)>(this, 7)(this, pType);
    }
public:
    // @note: @og: wrapper around GetSizes, this one gets CSchemaClassInfo->m_nSize
    bool GetSize(int* nOutSize) {
        std::uint8_t align_of = 0;
        return GetSizeWithAlignOf(nOutSize, &align_of);
    }
public:
    std::uintptr_t* vftable; // 0x0000
    const char* m_pszName; // 0x0008

    CSchemaSystemTypeScope* m_pTypeScope; // 0x0010
    std::uint8_t m_unTypeCategory; // ETypeCategory 0x0018
    std::uint8_t m_unAtomicCategory; // EAtomicCategory 0x0019

    // find out to what class pointer points.
    CSchemaType* GetRefClass() const {
        if (m_unTypeCategory != Schema_Ptr)
            return nullptr;

        auto ptr = m_pSchemaType;
        while (ptr && ptr->m_unTypeCategory == ETypeCategory::Schema_Ptr)
            ptr = ptr->m_pSchemaType;

        return ptr;
    }

    struct array_t {
        std::uint32_t m_nArraySize;
    private:
        std::uint32_t m_unknown = 0;
    public:
        CSchemaType* m_pElementType;
    };

    struct atomic_t { // same goes for CollectionOfT
    private:
        std::uint64_t pad0x0000[2] = {};
    public:
        CSchemaType* m_pTemplateTypeName;
    };

    struct atomic_tt {
    private:
        std::uint64_t pad0x0000[2] = {};
    public:
        CSchemaType* m_pTemplates[2];
    };

    struct atomic_i {
    private:
        std::uint64_t pad0x0000[2] = {};
    public:
        std::uint64_t m_nInteger;
    };

    // this union depends on CSchema implementation, all members above
    // is from base class ( CSchemaType )
    union // 0x020
    {
        CSchemaType* m_pSchemaType;
        CSchemaClassInfo* m_pClassInfo;
        CSchemaEnumBinding* m_pEnumBinding;
        array_t m_Array;
        atomic_t m_Atomic_t;
        atomic_tt m_Atomic_tt;
        atomic_i m_Atomic_i;
    };
};
static_assert(offsetof(CSchemaType, m_pSchemaType) == 0x20);

using CSchemaType_DeclaredClass = CSchemaType;
using CSchemaType_DeclaredEnum = CSchemaType;
using CSchemaType_Builtin = CSchemaType;

struct SchemaClassFieldData_t {
    const char* m_pszName; // 0x0000
    CSchemaType* m_pSchemaType; // 0x0008
    std::int32_t m_nSingleInheritanceOffset; // 0x0010
    std::int32_t m_nMetadataSize; // 0x0014
    SchemaMetadataEntryData_t* m_pMetadata; // 0x0018
};

struct SchemaStaticFieldData_t {
    const char* m_pszName; // 0x0000
    CSchemaType* m_pSchemaType; // 0x0008
    void* m_pInstance; // 0x0010
    char pad_0x0018[0x10]; // 0x0018
};

struct SchemaBaseClassInfoData_t {
    unsigned int m_unOffset; // 0x0000
    CSchemaClassInfo* m_pPrevByClass; // 0x0008
};

using SchemaFieldMetadataOverrideSetData_t = datamap_t;
using SchemaFieldMetadataOverrideData_t = typedescription_t;

struct SchemaClassInfoData_t {
public:
    enum class SchemaClassInfoFunctionIndex : std::int32_t {
        kRegisterClassSchema = 0,
        kFillBaseClassName = 1,
        kCopyInstance = 2,
        kCreateInstance = 3,
        kDestroyInstance = 4,
        kCreateInstanceWithMemory = 5,
        kDestroyInstanceWithMemory = 6,
        kSchemaDynamicBinding = 7
    };
public:
    SchemaClassInfoData_t* m_pSelf; // 0x0000
    const char* m_pszName; // 0x0008
    const char* m_pszModule; // 0x0010
    int m_nSize; // 0x0018
    std::int16_t m_nFieldSize; // 0x001C
    std::int16_t m_nStaticFieldsSize; // 0x001E
    std::int16_t m_nStaticMetadataSize; // 0x0020
    std::uint8_t m_unAlignOf; // 0x0022
    std::uint8_t m_bHasBaseClass; // 0x0023
    std::int16_t m_nTotalClassSize; // 0x0024 // @note: @og: if there no derived or base class then it will be 1 otherwise derived class size + 1.
    std::int16_t m_nDerivedClassSize; // 0x0026
    SchemaClassFieldData_t* m_pFields; // 0x0028
    SchemaStaticFieldData_t* m_pStaticFields; // 0x0030
    SchemaBaseClassInfoData_t* m_pBaseClassses; // 0x0038
    SchemaFieldMetadataOverrideSetData_t* m_pFieldMetadataOverrides; // 0x0040
    SchemaMetadataEntryData_t* m_pStaticMetadata; // 0x0048
    CSchemaSystemTypeScope* m_pTypeScope; // 0x0050
    CSchemaType* m_pSchemaType; // 0x0058
    SchemaClassFlags_t m_nClassFlags:8; // 0x0060
    std::uint32_t m_unSequence; // 0x0064 // @note: @og: idk
    void* m_pFn; // 0x0068
};

class CSchemaClassInfo : public SchemaClassInfoData_t {
public:
    [[nodiscard]] std::string_view GetName() {
        if (m_pszName)
            return {m_pszName};
        return {};
    }

    [[nodiscard]] std::string_view GetModule() {
        if (m_pszModule)
            return {m_pszModule};
        return {};
    }

    [[nodiscard]] std::optional<CSchemaClassInfo*> GetBaseClass() const {
        if (m_bHasBaseClass && m_pBaseClassses)
            return m_pBaseClassses->m_pPrevByClass;
        return std::nullopt;
    }

    std::vector<SchemaClassFieldData_t> GetFields() {
        return {m_pFields, m_pFields + m_nFieldSize};
    }

    std::vector<SchemaStaticFieldData_t> GetStaticFields() {
        return {m_pStaticFields, m_pStaticFields + m_nStaticFieldsSize};
    }

    std::vector<SchemaMetadataEntryData_t> GetStaticMetadata() {
        return {m_pStaticMetadata, m_pStaticMetadata + m_nStaticMetadataSize};
    }

    [[nodiscard]] std::string_view GetPrevClassName() const {
        if (!m_pBaseClassses || !m_pBaseClassses->m_pPrevByClass)
            return {};
        return m_pBaseClassses->m_pPrevByClass->GetName();
    }

    [[nodiscard]] bool IsA(CSchemaType* pInheritance) const {
        if (!m_pSchemaType)
            return false;

        return m_pSchemaType->IsA(pInheritance);
    }

    [[nodiscard]] bool HasVirtualTable() const {
        return (m_nClassFlags & SCHEMA_CF1_HAS_VIRTUAL_MEMBERS) != 0;
    }

    [[nodiscard]] bool RecursiveHasVirtualTable() const {
        return HasVirtualTable() ? true :
                                   (m_pBaseClassses && m_pBaseClassses->m_pPrevByClass ? m_pBaseClassses->m_pPrevByClass->HasVirtualTable() : false);
    }

    [[nodiscard]] bool IsInherits(const std::string_view from) const {
        if (!m_bHasBaseClass || !m_pBaseClassses || !m_pBaseClassses->m_pPrevByClass)
            return false;
        if (m_pBaseClassses->m_pPrevByClass->GetName() == from)
            return true;
        return false;
    }

    [[nodiscard]] bool IsRecursiveInherits(const std::string_view from) const {
        return IsInherits(from) ?
                   true :
                   (m_pBaseClassses && m_pBaseClassses->m_pPrevByClass ? m_pBaseClassses->m_pPrevByClass->IsRecursiveInherits(from) : false);
    }

    [[nodiscard]] int GetSize() const {
        return m_nSize;
    }

    [[nodiscard]] std::uint8_t GetAligment() const {
        return m_unAlignOf == std::numeric_limits<std::uint8_t>::max() ? 8 : m_unAlignOf;
    }

    // @note: @og: Copy instance from original to new created with all data from original, returns new_instance
    void* CopyInstance(void* instance, void* new_instance) const {
        using Fn = void* (*)(SchemaClassInfoFunctionIndex, void*, void*);
        return reinterpret_cast<Fn>(m_pFn)(SchemaClassInfoFunctionIndex::kCreateInstance, instance, new_instance);
    }

    // @note: @og: Creates default instance with engine allocated memory (e.g. if SchemaClassInfoData_t is C_BaseEntity, then Instance will be
    // C_BaseEntity)
    [[nodiscard]] void* CreateInstance() const {
        using Fn = void* (*)(SchemaClassInfoFunctionIndex);
        return reinterpret_cast<Fn>(m_pFn)(SchemaClassInfoFunctionIndex::kCreateInstance);
    }

    // @note: @og: Creates default instance with your own allocated memory (e.g. if SchemaClassInfoData_t is C_BaseEntity, then Instance will be
    // C_BaseEntity)
    void* CreateInstance(void* memory) const {
        using Fn = void* (*)(SchemaClassInfoFunctionIndex, void*);
        return reinterpret_cast<Fn>(m_pFn)(SchemaClassInfoFunctionIndex::kCreateInstanceWithMemory, memory);
    }

    // @note: @og: Destroy instance (e.g.: C_BaseInstance 1st VT fn with 0 flag)
    void* DestroyInstance(void* instance) const {
        using Fn = void* (*)(SchemaClassInfoFunctionIndex, void*);
        return reinterpret_cast<Fn>(m_pFn)(SchemaClassInfoFunctionIndex::kDestroyInstanceWithMemory, instance);
    }

    // @note: @og: Destroy instance with de-allocating memory (e.g.: C_BaseInstance 1st VT fn with 1 flag)
    void* DestroyInstanceWithMemory(void* instance) const {
        using Fn = void* (*)(SchemaClassInfoFunctionIndex, void*);
        return reinterpret_cast<Fn>(m_pFn)(SchemaClassInfoFunctionIndex::kDestroyInstanceWithMemory, instance);
    }

    CSchemaClassBinding* SchemaClassBinding(void* entity) const {
        using Fn = CSchemaClassBinding* (*)(SchemaClassInfoFunctionIndex, void*);
        return reinterpret_cast<Fn>(m_pFn)(SchemaClassInfoFunctionIndex::kSchemaDynamicBinding, entity);
    }
};

enum class SchemaBuiltinType_t : std::uint32_t {
    kInvalid = 0,
    kVoid,
    kChar,
    kInt8,
    kUint8,
    kInt16,
    kUint16,
    kInt32,
    kUint32,
    kInt64,
    kUint64,
    kFloat32,
    kFloat64,
    kBool
};

class CSchemaSystemTypeScope {
public:
    void* InsertNewClassBinding(const std::string_view szName, void* a2) {
        return Virtual::Get<void* (*)(CSchemaSystemTypeScope*, const char*, void*)>(this, 0)(this, szName.data(), a2);
    }

    void* InsertNewEnumBinding(const std::string_view szName, void* a2) {
        return Virtual::Get<void* (*)(CSchemaSystemTypeScope*, const char*, void*)>(this, 1)(this, szName.data(), a2);
    }

    CSchemaClassInfo* FindDeclaredClass(const std::string_view szName) {
#if defined(SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE) && SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE == 2
        CSchemaClassInfo* class_info;

        Virtual::Get<void(__thiscall*)(void*, CSchemaClassInfo**, const char*)>(this, 2)(this, &class_info, szName.data());
        return class_info;
#else
        return Virtual::Get<CSchemaClassInfo*(__thiscall*)(void*, const char*)>(this, 2)(this, szName.data());
#endif
    }

    CSchemaEnumInfo* FindDeclaredEnum(const std::string_view szName) {
#if defined(SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE) && SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE == 2
        CSchemaEnumInfo* enum_info;

        Virtual::Get<void(__thiscall*)(void*, CSchemaEnumInfo**, const char*)>(this, 3)(this, &enum_info, szName.data());
        return enum_info;
#else
        return Virtual::Get<CSchemaEnumInfo*(__thiscall*)(void*, const char*)>(this, 3)(this, szName.data());
#endif
    }

    CSchemaType* FindSchemaTypeByName(const std::string_view szName) {
#if defined(SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE) && SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE == 2
        CSchemaType* schema_type;

        Virtual::Get<void(__thiscall*)(void*, CSchemaType**, const char*)>(this, 4)(this, &schema_type, szName.data());
        return schema_type;
#else
        return Virtual::Get<CSchemaType*(__thiscall*)(void*, const char*)>(this, 4)(this, szName.data());
#endif
    }

    CSchemaType_Builtin* Type_Builtin(const SchemaBuiltinType_t type) {
#if defined(SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE) && SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE == 2
        CSchemaType_Builtin* type_builtin;

        Virtual::Get<void(__thiscall*)(void*, CSchemaType_Builtin**, SchemaBuiltinType_t)>(this, 5)(this, &type_builtin, type);
        return type_builtin;
#else
        return Virtual::Get<CSchemaType_Builtin*(__thiscall*)(void*, SchemaBuiltinType_t)>(this, 5)(this, type);
#endif
    }

    CSchemaType_DeclaredClass* Type_DeclaredClass(const std::string_view szName) {
#if defined(SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE) && SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE == 2
        CSchemaType_DeclaredClass* declared_class;

        Virtual::Get<void(__thiscall*)(void*, CSchemaType_DeclaredClass**, const char*)>(this, 13)(this, &declared_class, szName.data());
        return declared_class;
#else
        return Virtual::Get<CSchemaType_DeclaredClass*(__thiscall*)(void*, const char*)>(this, 13)(this, szName.data());
#endif
    }

    CSchemaType_DeclaredEnum* Type_DeclaredEnum(const std::string_view szName) {
#if defined(SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE) && SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE == 2
        CSchemaType_DeclaredEnum* declared_class;

        Virtual::Get<void(__thiscall*)(void*, CSchemaType_DeclaredEnum**, const char*)>(this, 14)(this, &declared_class, szName.data());
        return declared_class;
#else
        return Virtual::Get<CSchemaType_DeclaredEnum*(__thiscall*)(void*, const char*)>(this, 14)(this, szName.data());
#endif
    }

    const char* GetScopeName() {
        return Virtual::Get<const char*(__thiscall*)(void*)>(this, 30)(this);
    }

    bool IsGlobalScope() {
        return Virtual::Get<bool(__thiscall*)(void*)>(this, 31)(this);
    }

    std::string_view BGetScopeName() {
        return {m_szName.data()};
    }

    [[nodiscard]] CUtlTSHash<CSchemaClassBinding*> GetClassBindings() const {
        return m_ClassBindings;
    }

    [[nodiscard]] CUtlTSHash<CSchemaEnumBinding*> GetEnumBindings() const {
        return m_EnumBindings;
    }
private:
    void* vftable = nullptr;
    std::array<char, 256> m_szName = {};
    char pad_0x0108[SCHEMASYSTEMTYPESCOPE_OFF1] = {}; // 0x0108
    CUtlTSHash<CSchemaClassBinding*> m_ClassBindings; // 0x0588
    char pad_0x0594[SCHEMASYSTEMTYPESCOPE_OFF2] = {}; // 0x05C8
    CUtlTSHash<CSchemaEnumBinding*> m_EnumBindings; // 0x2DD0
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

    CSchemaSystemTypeScope* FindTypeScopeForModule(const std::string_view pszModuleName) {
        return Virtual::Get<CSchemaSystemTypeScope*(__thiscall*)(void*, const char*, void*)>(this, 13)(this, pszModuleName.data(), nullptr);
    }

    CSchemaSystemTypeScope* GetTypeScopeForBinding(const SchemaTypeScope_t nType, const std::string_view pszBinding) {
        return Virtual::Get<CSchemaSystemTypeScope*(__thiscall*)(void*, SchemaTypeScope_t, const char*)>(this, 14)(this, nType, pszBinding.data());
    }

    // @note: @og: E.g: engine2.dll!CEntityComponent
    CSchemaClassBinding* FindClassByScopedName(const std::string_view pszScopedName) {
#if defined(SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE) && SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE == 2
        CSchemaClassBinding* binding;

        Virtual::Get<void(__thiscall*)(void*, CSchemaClassBinding**, const char*)>(this, 16)(this, &binding, pszScopedName.data());
        return binding;
#else
        return Virtual::Get<CSchemaClassBinding*(__thiscall*)(void*, const char*)>(this, 16)(this, pszScopedName.data());
#endif
    }

    std::string_view ScopedNameForClass(CSchemaClassBinding* pBinding) {
        static CBufferStringGrowable<1024> szBuf;
        auto res = Virtual::Get<const char*(__thiscall*)(void*, CSchemaClassBinding*, CBufferString*)>(this, 17)(this, pBinding, &szBuf);
        return szBuf.Get();
    }

    // @note: @og: E.g: engine2.dll!SpawnDebugRestrictionOverrideState_t
    CSchemaEnumBinding* FindEnumByScopedName(const std::string_view pszScopedName) {
#if defined(SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE) && SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE == 2
        CSchemaEnumBinding* binding;

        Virtual::Get<void(__thiscall*)(void*, CSchemaEnumBinding**, const char*)>(this, 18)(this, &binding, pszScopedName.data());
        return binding;
#else
        return Virtual::Get<CSchemaEnumBinding*(__thiscall*)(void*, const char*)>(this, 18)(this, pszScopedName.data());
#endif
    }

    std::string_view ScopedNameForEnum(CSchemaEnumBinding* pBinding) {
        static CBufferStringGrowable<1024> szBuf;
        auto res = Virtual::Get<const char*(__thiscall*)(void*, CSchemaEnumBinding*, CBufferString*)>(this, 19)(this, pBinding, &szBuf);
        return szBuf.Get();
    }

    const char* GetClassInfoBinaryName(CSchemaClassBinding* pBinding) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaClassBinding*)>(this, 22)(this, pBinding);
    }

    const char* GetClassProjectName(CSchemaClassBinding* pBinding) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaClassBinding*)>(this, 23)(this, pBinding);
    }

    const char* GetEnumBinaryName(CSchemaEnumBinding* pBinding) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaEnumBinding*)>(this, 24)(this, pBinding);
    }

    const char* GetEnumProjectName(CSchemaEnumBinding* pBinding) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaEnumBinding*)>(this, 25)(this, pBinding);
    }

    CSchemaClassBinding* ValidateClasses(CSchemaClassBinding** ppBinding) {
        return Virtual::Get<CSchemaClassBinding*(__thiscall*)(void*, CSchemaClassBinding**)>(this, CSCHEMASYSTEM_VALIDATECLASSES)(this, ppBinding);
    }

    bool SchemaSystemIsReady() {
        return Virtual::Get<bool(__thiscall*)(void*)>(this, 26)(this);
    }

    [[nodiscard]] CUtlVector<CSchemaSystemTypeScope*> GetTypeScopes(void) const {
        return m_TypeScopes;
    }

    [[nodiscard]] std::int32_t GetRegistration() const {
        return m_nRegistrations;
    }

    [[nodiscard]] std::int32_t GetIgnored() const {
        return m_nIgnored;
    }

    [[nodiscard]] std::int32_t GetRedundant() const {
        return m_nRedundant;
    }

    [[nodiscard]] std::int32_t GetIgnoredBytes() const {
        return m_nIgnoredBytes;
    }
private:
    char pad_0x0000[SCHEMASYSTEM_TYPE_SCOPES_OFFSET] = {}; // 0x0000
    CUtlVector<CSchemaSystemTypeScope*> m_TypeScopes = {}; // SCHEMASYSTEM_TYPE_SCOPES_OFFSET
    char pad_01A0[288] = {}; // 0x01A0
    std::int32_t m_nRegistrations = 0; // 0x02C0
    std::int32_t m_nIgnored = 0; // 0x02C4
    std::int32_t m_nRedundant = 0; // 0x02C8
    char pad_02CC[4] = {}; // 0x02CC
    std::int32_t m_nIgnoredBytes = 0; // 0x02D0
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
