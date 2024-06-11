// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

#if defined(SBOX)
// untested might be wrong

constexpr auto kSchemaSystemVersion = 1;
constexpr auto kSchemaSystem_PAD0 = 0x5420;
constexpr auto kSchemaSystem_PAD1 = 0x120;
constexpr auto kSchemaSystemTypeScope_PAD0 = 0x7;

enum {
    kSchemaType_GetSizeWithAlignOf = 4,
    kSchemaSystem_GetClassInfoBinaryName = 21,
    kSchemaSystem_GetClassProjectName = 22,
    kSchemaSystem_GetEnumBinaryName = 23,
    kSchemaSystem_GetEnumProjectName = 24,
    kSchemaSystem_ValidateClasses = 34,
    kSchemaSystemTypeScope_DeclaredClass = 13,
    kSchemaSystemTypeScope_DeclaredEnum = kSchemaSystemTypeScope_DeclaredClass + 1,
    kSchemaSystemTypeScope_GetScopeName = 28,
    kSchemaSystemTypeScope_IsGlobalScope = kSchemaSystemTypeScope_GetScopeName + 1,
};

#elif defined(ARTIFACT2)

// untested

constexpr auto kSchemaSystemVersion = 1;
constexpr auto kSchemaSystem_PAD0 = 0x5430;
constexpr auto kSchemaSystem_PAD1 = 0x120;
constexpr auto kSchemaSystemTypeScope_PAD0 = 0x7;

enum {
    kSchemaType_GetSizeWithAlignOf = 4,
    kSchemaSystem_GetClassInfoBinaryName = 21,
    kSchemaSystem_GetClassProjectName = 22,
    kSchemaSystem_GetEnumBinaryName = 23,
    kSchemaSystem_GetEnumProjectName = 24,
    kSchemaSystem_ValidateClasses = 34,
    kSchemaSystemTypeScope_DeclaredClass = 13,
    kSchemaSystemTypeScope_DeclaredEnum = kSchemaSystemTypeScope_DeclaredClass + 1,
    kSchemaSystemTypeScope_GetScopeName = 28,
    kSchemaSystemTypeScope_IsGlobalScope = kSchemaSystemTypeScope_GetScopeName + 1,
};

#elif defined(ARTIFACT1)

// untested

constexpr auto kSchemaSystemVersion = 1;
constexpr auto kSchemaSystem_PAD0 = 0x4428;
constexpr auto kSchemaSystem_PAD1 = 0x120;
constexpr auto kSchemaSystemTypeScope_PAD0 = 0x7;

enum {
    kSchemaType_GetSizeWithAlignOf = 4,
    kSchemaSystem_GetClassInfoBinaryName = 21,
    kSchemaSystem_GetClassProjectName = 22,
    kSchemaSystem_GetEnumBinaryName = 23,
    kSchemaSystem_GetEnumProjectName = 24,
    kSchemaSystem_ValidateClasses = 34,
    kSchemaSystemTypeScope_DeclaredClass = 13,
    kSchemaSystemTypeScope_DeclaredEnum = kSchemaSystemTypeScope_DeclaredClass + 1,
    kSchemaSystemTypeScope_GetScopeName = 28,
    kSchemaSystemTypeScope_IsGlobalScope = kSchemaSystemTypeScope_GetScopeName + 1,
};

#elif defined(UNDERLORDS)

constexpr auto kSchemaSystemVersion = 1;
constexpr auto kSchemaSystem_PAD0 = 0x5420;
constexpr auto kSchemaSystem_PAD1 = 0x120;
constexpr auto kSchemaSystemTypeScope_PAD0 = 0x7;

enum {
    kSchemaType_GetSizeWithAlignOf = 4,
    kSchemaSystem_ValidateClasses = 34,
    kSchemaSystem_GetClassInfoBinaryName = 21,
    kSchemaSystem_GetClassProjectName = 22,
    kSchemaSystem_GetEnumBinaryName = 23,
    kSchemaSystem_GetEnumProjectName = 24,
    kSchemaSystemTypeScope_DeclaredClass = 13,
    kSchemaSystemTypeScope_DeclaredEnum = kSchemaSystemTypeScope_DeclaredClass + 1,
    kSchemaSystemTypeScope_GetScopeName = 28,
    kSchemaSystemTypeScope_IsGlobalScope = kSchemaSystemTypeScope_GetScopeName + 1,
};

#elif defined(DESKJOB)

// untested

constexpr auto kSchemaSystemVersion = 1;
constexpr auto kSchemaSystem_PAD0 = 0x3A0;
constexpr auto kSchemaSystemTypeScope_PAD0 = 0x7;

enum {
    kSchemaType_GetSizeWithAlignOf = 3,
    kSchemaSystem_ValidateClasses = 34,
    kSchemaSystem_GetClassInfoBinaryName = 21,
    kSchemaSystem_GetClassProjectName = 22,
    kSchemaSystem_GetEnumBinaryName = 23,
    kSchemaSystem_GetEnumProjectName = 24,
    kSchemaSystemTypeScope_DeclaredClass = 13,
    kSchemaSystemTypeScope_DeclaredEnum = kSchemaSystemTypeScope_DeclaredClass + 1,
    kSchemaSystemTypeScope_GetScopeName = 30,
    kSchemaSystemTypeScope_IsGlobalScope = kSchemaSystemTypeScope_GetScopeName + 1,
};

#elif defined(HL_ALYX)
    #error unimplemented
#elif defined(THE_LAB_ROBOT_REPAIR)
    #error unimplemented
#elif defined(DOTA2) || defined(CS2)

constexpr auto kSchemaSystemVersion = 2;
constexpr auto kSchemaSystem_PAD0 = 0x188;
constexpr auto kSchemaSystem_PAD1 = 0x120;
constexpr auto kSchemaSystemTypeScope_PAD0 = 0x7;

enum {
    kSchemaType_GetSizeWithAlignOf = 3,
    kSchemaSystem_ValidateClasses = 35,
    kSchemaSystem_GetClassInfoBinaryName = 22,
    kSchemaSystem_GetClassProjectName = kSchemaSystem_GetClassInfoBinaryName + 1,
    kSchemaSystem_GetEnumBinaryName = kSchemaSystem_GetClassProjectName + 1,
    kSchemaSystem_GetEnumProjectName = kSchemaSystem_GetEnumBinaryName + 1,
    kSchemaSystemTypeScope_DeclaredClass = 14,
    kSchemaSystemTypeScope_DeclaredEnum = kSchemaSystemTypeScope_DeclaredClass + 1,
    kSchemaSystemTypeScope_GetScopeName = 28,
    kSchemaSystemTypeScope_IsGlobalScope = kSchemaSystemTypeScope_GetScopeName + 1,
};

#elif defined(CS2_OLD)

constexpr auto kSchemaSystemVersion = 2;
constexpr auto kSchemaSystem_PAD0 = 0x190;
constexpr auto kSchemaSystem_PAD1 = 0x120;
constexpr auto kSchemaSystemTypeScope_PAD0 = 0x7;

enum {
    kSchemaType_GetSizeWithAlignOf = 3,
    kSchemaSystem_ValidateClasses = 35,
    kSchemaSystem_GetClassInfoBinaryName = 22,
    kSchemaSystem_GetClassProjectName = 23,
    kSchemaSystem_GetEnumBinaryName = 24,
    kSchemaSystem_GetEnumProjectName = 25,
    kSchemaSystemTypeScope_DeclaredClass = 14,
    kSchemaSystemTypeScope_DeclaredEnum = kSchemaSystemTypeScope_DeclaredClass + 1,
    kSchemaSystemTypeScope_GetScopeName = 32,
    kSchemaSystemTypeScope_IsGlobalScope = kSchemaSystemTypeScope_GetScopeName + 1,
};

#else
    #error No implementation defined, please re-generate project with premake5
#endif

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

#if defined(CS2) || defined(DOTA2)
enum SchemaClassFlags_t {
    SCHEMA_CF1_HAS_VIRTUAL_MEMBERS = (1 << 0),
    SCHEMA_CF1_IS_ABSTRACT = (1 << 1),
    SCHEMA_CF1_HAS_TRIVIAL_CONSTRUCTOR = (1 << 2),
    SCHEMA_CF1_HAS_TRIVIAL_DESTRUCTOR = (1 << 3),
    SCHEMA_CF1_LIMITED_METADATA = (1 << 4),
    SCHEMA_CF1_INHERITANCE_DEPTH_CALCULATED = (1 << 5),
    SCHEMA_CF1_MODULE_LOCAL_TYPE_SCOPE = (1 << 6),
    SCHEMA_CF1_GLOBAL_TYPE_SCOPE = (1 << 7),
    SCHEMA_CF1_CONSTRUCT_ALLOWED = (1 << 8),
    SCHEMA_CF1_CONSTRUCT_DISALLOWED = (1 << 9),
    SCHEMA_CF1_INFO_TAG_MNetworkAssumeNotNetworkable = (1 << 10),
    SCHEMA_CF1_INFO_TAG_MNetworkNoBase = (1 << 11),
    SCHEMA_CF1_INFO_TAG_MIgnoreTypeScopeMetaChecks = (1 << 12),
    SCHEMA_CF1_INFO_TAG_MDisableDataDescValidation = (1 << 13),
    SCHEMA_CF1_INFO_TAG_MClassHasEntityLimitedDataDesc = (1 << 14),
    SCHEMA_CF1_INFO_TAG_MClassHasCustomAlignedNewDelete = (1 << 15),
    SCHEMA_CF1_INFO_TAG_MNonConstructibleClassBase = (1 << 16),
    SCHEMA_CF1_INFO_TAG_MConstructibleClassBase = (1 << 17),
    SCHEMA_CF1_INFO_TAG_MHasKV3TransferPolymorphicClassname = (1 << 18),
};
#else
enum SchemaClassFlags_t {
    SCHEMA_CF1_HAS_VIRTUAL_MEMBERS = 1,
    SCHEMA_CF1_IS_ABSTRACT = 2,
    SCHEMA_CF1_HAS_TRIVIAL_CONSTRUCTOR = 4,
    SCHEMA_CF1_HAS_TRIVIAL_DESTRUCTOR = 8,
    SCHEMA_CF1_HAS_NOSCHEMA_MEMBERS = 16,
    SCHEMA_CF1_IS_PARENT_CLASSES_PARSED = 32,
    SCHEMA_CF1_IS_LOCAL_TYPE_SCOPE = 64,
    SCHEMA_CF1_IS_GLOBAL_TYPE_SCOPE = 128,
    SCHEMA_CF1_IS_SCHEMA_VALIDATED = 1024,
};
#endif

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

enum class SchemaEnumFlags_t : std::uint16_t {
    SCHEMA_EF_IS_REGISTERED = (1 << 0),
    SCHEMA_EF_MODULE_LOCAL_TYPE_SCOPE = (1 << 1),
    SCHEMA_EF_GLOBAL_TYPE_SCOPE = (1 << 2),
};

class SchemaEnumInfoData_t {
public:
    SchemaEnumInfoData_t* m_pSelf; // 0x0000
    const char* m_pszName; // 0x0008
    const char* m_pszModule; // 0x0010
    std::uint8_t m_unSize; // 0x0018
    std::uint8_t m_unAlignOf; // 0x0019
    SchemaEnumFlags_t m_unFlags; // 0x001A
    std::int16_t m_nEnumeratorCount; // 0x001C
    std::int16_t m_nStaticMetadataSize; // 0x001E
    SchemaEnumeratorInfoData_t* m_pEnumerators;
    SchemaMetadataEntryData_t* m_pStaticMetadata;
    CSchemaSystemTypeScope* m_pTypeScope; // 0x0030
    std::int64_t m_nMinEnumeratorValue; // 0x0038
    std::int64_t m_nMaxEnumeratorValue; // 0x0040
};
static_assert(sizeof(SchemaEnumInfoData_t) == 0x48);

class CSchemaEnumInfo : public SchemaEnumInfoData_t {
public:
    [[nodiscard]] std::vector<SchemaEnumeratorInfoData_t> GetEnumeratorValues() const {
        return {m_pEnumerators, m_pEnumerators + m_nEnumeratorCount};
    }

    [[nodiscard]] std::vector<SchemaMetadataEntryData_t> GetStaticMetadata() const {
        return {m_pStaticMetadata, m_pStaticMetadata + m_nStaticMetadataSize};
    }
};

enum class ETypeCategory : std::uint8_t {
    Schema_Builtin = 0,
    Schema_Ptr,
    Schema_Bitfield,
    Schema_FixedArray,
    Schema_Atomic,
    Schema_DeclaredClass,
    Schema_DeclaredEnum,
    Schema_None
};

#if defined(CS2)
enum class EAtomicCategory : std::uint8_t {
    Atomic_Basic = 0,
    Atomic_T,
    Atomic_CollectionOfT,
    Atomic_TF,
    Atomic_TT,
    Atomic_TTF,
    Atomic_I,
    Atomic_None
};
#else
enum class EAtomicCategory : std::uint8_t {
    Atomic_Basic = 0,
    Atomic_T,
    Atomic_CollectionOfT,
    Atomic_TT,
    Atomic_I,
    Atomic_None
};
#endif

enum class SchemaBuiltinType_t : std::uint32_t {
    Schema_Builtin_none = 0,
    Schema_Builtin_void,
    Schema_Builtin_char,
    Schema_Builtin_int8,
    Schema_Builtin_uint8,
    Schema_Builtin_int16,
    Schema_Builtin_uint16,
    Schema_Builtin_int32,
    Schema_Builtin_uint32,
    Schema_Builtin_int64,
    Schema_Builtin_uint64,
    Schema_Builtin_float32,
    Schema_Builtin_float64,
    Schema_Builtin_bool,
    Schema_Builtin_count
};

constexpr auto kSchemaBuiltinTypeCount = static_cast<std::size_t>(SchemaBuiltinType_t::Schema_Builtin_count);

class CSchemaType {
public:
    [[nodiscard]] bool IsValid(void) {
        return Virtual::Get<bool (*)(void*)>(this, 0)(this);
    }

    [[nodiscard]] std::string ToString() {
        static CBufferStringGrowable<1024> szBuf;
        (void)Virtual::Get<const char*(__thiscall*)(void*, CBufferString&, bool bDontClearBuff)>(this, 1)(this, szBuf, false);
        return szBuf.Get();
    }

    void SpewDescription(std::uint32_t unLogChannel, const char* szName) {
        Virtual::Get<void (*)(void*, std::uint32_t, const char*)>(this, 2)(this, unLogChannel, szName);
    }

    // @note: @og: gets size with align
    [[nodiscard]] bool GetSizeWithAlignOf(int* nOutSize, std::uint8_t* unOutAlign) {
        return reinterpret_cast<int (*)(void*, int*, std::uint8_t*)>(vftable[kSchemaType_GetSizeWithAlignOf])(this, nOutSize, unOutAlign);
    }

    [[nodiscard]] bool CanReinterpretAs(CSchemaType* pType) {
        return reinterpret_cast<bool (*)(void*, CSchemaType*)>(vftable[kSchemaType_GetSizeWithAlignOf + 1])(this, pType);
    }

    // @note: @og: Can be used on CSchemaClassInfo. (Uses multiple inheritance depth verify that current CSchemaType->m_pClassInfo is inherits from pType)
    [[nodiscard]] bool IsA(CSchemaType* pType) {
        return Virtual::Get<bool (*)(void*, CSchemaType*)>(this, 7)(this, pType);
    }

public:
    // @note: @og: wrapper around GetSizes, this one gets CSchemaClassInfo->m_nSize
    [[nodiscard]] std::optional<int> GetSize() {
        std::uint8_t align_of = 0;
        int result = 0;
        return GetSizeWithAlignOf(&result, &align_of) ? std::make_optional(result) : std::nullopt;
    }

    // @todo: @og: find out to what class pointer points.
    [[nodiscard]] CSchemaType* GetRefClass();

    [[nodiscard]] ETypeCategory GetTypeCategory() {
#if defined(CS2) || defined(DOTA2)
        return m_unTypeCategory;
#else
        return Virtual::Get<ETypeCategory (*)(CSchemaType*)>(this, 0)(this);
#endif
    }

    [[nodiscard]] EAtomicCategory GetAtomicCategory() {
#if defined(CS2) || defined(DOTA2)
        return m_unAtomicCategory;
#else
        return Virtual::Get<EAtomicCategory (*)(CSchemaType*)>(this, 1)(this);
#endif
    }

public:
    std::uintptr_t* vftable; // 0x0000
    const char* m_pszName; // 0x0008

    CSchemaSystemTypeScope* m_pTypeScope; // 0x0010

#if defined(CS2) || defined(DOTA2)
    ETypeCategory m_unTypeCategory; // 0x0018
    EAtomicCategory m_unAtomicCategory; // 0x0019
#endif
};

class CSchemaType_Ptr : public CSchemaType {
public:
    CSchemaType* m_pObjectType;
};

[[nodiscard]] inline CSchemaType* CSchemaType::GetRefClass() {
    if (GetTypeCategory() != ETypeCategory::Schema_Ptr)
        return nullptr;

    auto ptr = reinterpret_cast<CSchemaType_Ptr*>(this)->m_pObjectType;
    while (ptr && ptr->GetTypeCategory() == ETypeCategory::Schema_Ptr)
        ptr = reinterpret_cast<CSchemaType_Ptr*>(ptr)->m_pObjectType;

    return ptr;
}

using CSchemaType_NoschemaType = CSchemaType;

class CSchemaType_Builtin : public CSchemaType {
public:
    SchemaBuiltinType_t m_eBuiltinType;
    std::uint8_t m_unSize;
};

class CSchemaType_DeclaredClass : public CSchemaType {
public:
    CSchemaClassInfo* m_pClassInfo;
    bool m_bGlobalPromotionRequired;
};

class CSchemaType_DeclaredEnum : public CSchemaType {
public:
    CSchemaEnumBinding* m_pClassInfo;
    bool m_bGlobalPromotionRequired;
};

struct SchemaAtomicTypeInfo_t {
    const char* m_pszName1;
    const char* m_pszName2;

    int m_nAtomicID;

    int m_nStaticMetadataCount;
    SchemaMetadataEntryData_t* m_pStaticMetadata;
};

class CSchemaType_Atomic : public CSchemaType {
public:
    SchemaAtomicTypeInfo_t* m_pAtomicInfo;
    int m_nAtomicID;
    std::uint16_t m_unSize;
    std::uint8_t m_unAlignment;
};

class CSchemaType_Atomic_T : public CSchemaType_Atomic {
public:
    CSchemaType* m_pTemplateType;
};

enum class SchemaAtomicFunctionIndex : std::int32_t {
    Schema_Atomic_Get_Count = 0,
    Schema_Atomic_Get_Element_Const,
    Schema_Atomic_Get_Element,
    Schema_Atomic_Swap_Elements,
    Schema_Atomic_Insert_Before,
    Schema_Atomic_Remove_Multiple,
    Schema_Atomic_Set_Count,
};

using SchemaAtomicFunction = void (*)(SchemaAtomicFunctionIndex, void*, void*, void*);

class CSchemaType_Atomic_CollectionOfT : public CSchemaType_Atomic_T {
public:
    SchemaAtomicFunction m_pFn;
    std::uint16_t m_unElementSize;
};

class CSchemaType_Atomic_TF : public CSchemaType_Atomic_T {
public:
    int m_nFuncPtrSize;
};

class CSchemaType_Atomic_TT : public CSchemaType_Atomic_T {
public:
    CSchemaType* m_pTemplateType2;
};

class CSchemaType_Atomic_TTF : public CSchemaType_Atomic_TT {
public:
    int m_nFuncPtrSize;
};

class CSchemaType_Atomic_I : public CSchemaType_Atomic {
public:
    int m_nInteger;
};

class CSchemaType_Bitfield : public CSchemaType {
public:
    int m_nSize;
};

class CSchemaType_FixedArray : public CSchemaType {
public:
    int m_nElementCount;
    std::uint16_t m_unElementSize;
    std::uint8_t m_unElementAlignment;
    CSchemaType* m_pElementType;
};

struct AtomicTypeInfo_T_t {
    int m_nAtomicID;
    CSchemaType* m_pTemplateType;
    SchemaAtomicFunction m_pfnManipulator;
};

struct AtomicTypeInfo_TF_t {
    int m_nAtomicID;
    CSchemaType* m_pTemplateType;
    int m_nFuncPtrSize;
};

struct AtomicTypeInfo_TT_t {
    int m_nAtomicID;
    CSchemaType* m_pTemplateType;
    CSchemaType* m_pTemplateType2;
};

struct AtomicTypeInfo_TTF_t {
    int m_nAtomicID;
    CSchemaType* m_pTemplateType;
    CSchemaType* m_pTemplateType2;
    int m_nFuncPtrSize;
};

struct AtomicTypeInfo_I_t {
    int m_nAtomicID;
    int m_nInteger;
};

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
    int m_nStaticMetadataCount; // 0x0018
    SchemaMetadataEntryData_t* m_pStaticMetadata; // 0x0020
};

struct SchemaBaseClassInfoData_t {
    std::uint32_t m_unOffset; // 0x0000
    CSchemaClassInfo* m_pPrevByClass; // 0x0008
};

using SchemaFieldMetadataOverrideSetData_t = datamap_t;
using SchemaFieldMetadataOverrideData_t = typedescription_t;

struct SchemaClassInfoData_t {
public:
    enum class SchemaClassInfoFunctionIndex : std::int32_t {
        kRegisterClassSchema = 0,
        kPreRegisterClassSchema = 1,
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
    std::int16_t m_nTotalClassSize; // 0x0024 // @note: @og: if there is no derived or base class, then it will be 1 otherwise derived class size + 1.
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

public:
    template <typename RetTy = void*, typename... Ty>
    RetTy CallFunction(SchemaClassInfoFunctionIndex index, Ty... args) const {
        return reinterpret_cast<RetTy (*)(SchemaClassInfoFunctionIndex, Ty...)>(m_pFn)(index, std::forward<Ty>(args)...);
    }
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

    [[nodiscard]] std::vector<SchemaClassFieldData_t> GetFields() {
        return {m_pFields, m_pFields + m_nFieldSize};
    }

    [[nodiscard]] std::vector<SchemaStaticFieldData_t> GetStaticFields() {
        return {m_pStaticFields, m_pStaticFields + m_nStaticFieldsSize};
    }

    [[nodiscard]] std::vector<SchemaMetadataEntryData_t> GetStaticMetadata() {
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
    auto CopyInstance(void* instance, void* new_instance) const {
        return CallFunction<void*>(SchemaClassInfoFunctionIndex::kCreateInstance, instance, new_instance);
    }

    // @note: @og: Creates default instance with engine allocated memory (e.g. if SchemaClassInfoData_t is C_BaseEntity, then Instance will be
    // C_BaseEntity)
    [[nodiscard]] auto CreateInstance() const {
        return CallFunction<void*>(SchemaClassInfoFunctionIndex::kCreateInstance);
    }

    // @note: @og: Creates default instance with your own allocated memory (e.g. if SchemaClassInfoData_t is C_BaseEntity, then Instance will be
    // C_BaseEntity)
    auto CreateInstance(void* memory) const {
        return CallFunction<void*>(SchemaClassInfoFunctionIndex::kCreateInstanceWithMemory, memory);
    }

    // @note: @og: Destroy instance (e.g.: C_BaseEntity 1st VT fn with 0 flag)
    auto DestroyInstance(void* instance) const {
        return CallFunction<void*>(SchemaClassInfoFunctionIndex::kDestroyInstanceWithMemory, instance);
    }

    // @note: @og: Destroy instance with de-allocating memory (e.g.: C_BaseEntity 1st VT fn with 1 flag)
    auto DestroyInstanceWithMemory(void* instance) const {
        return CallFunction<void*>(SchemaClassInfoFunctionIndex::kDestroyInstanceWithMemory, instance);
    }

    [[nodiscard]] auto SchemaClassBinding(void* entity) const {
        return CallFunction<CSchemaClassBinding*>(SchemaClassInfoFunctionIndex::kSchemaDynamicBinding, entity);
    }
};

struct TypeAndCountInfo_t {
    int m_nElementCount;
    CSchemaType* m_pElementType;
};

template <class K, class V>
class CSchemaPtrMap {
public:
    CUtlMap<K, V> m_Map;

#if !defined(DOTA2) && !defined(CS2)
    CThreadFastMutex m_Mutex;
#endif
};

class CSchemaSystemTypeScope {
public:
    void* InsertNewClassBinding(const std::string_view szName, void* a2) {
        return Virtual::Get<void* (*)(CSchemaSystemTypeScope*, const char*, void*)>(this, 0)(this, szName.data(), a2);
    }

    void* InsertNewEnumBinding(const std::string_view szName, void* a2) {
        return Virtual::Get<void* (*)(CSchemaSystemTypeScope*, const char*, void*)>(this, 1)(this, szName.data(), a2);
    }

    [[nodiscard]] CSchemaClassInfo* FindDeclaredClass(const std::string_view szName) {
        if constexpr (kSchemaSystemVersion == 2) {
            CSchemaClassInfo* class_info;

            Virtual::Get<void(__thiscall*)(void*, CSchemaClassInfo**, const char*)>(this, 2)(this, &class_info, szName.data());
            return class_info;
        } else {
            return Virtual::Get<CSchemaClassInfo*(__thiscall*)(void*, const char*)>(this, 2)(this, szName.data());
        }
    }

    [[nodiscard]] CSchemaEnumInfo* FindDeclaredEnum(const std::string_view szName) {
        if constexpr (kSchemaSystemVersion == 2) {
            CSchemaEnumInfo* enum_info;

            Virtual::Get<void(__thiscall*)(void*, CSchemaEnumInfo**, const char*)>(this, 3)(this, &enum_info, szName.data());
            return enum_info;
        } else {
            return Virtual::Get<CSchemaEnumInfo*(__thiscall*)(void*, const char*)>(this, 3)(this, szName.data());
        }
    }

    [[nodiscard]] CSchemaType* FindSchemaTypeByName(const std::string_view szName) {
        if constexpr (kSchemaSystemVersion == 2) {
            CSchemaType* schema_type;

            Virtual::Get<void(__thiscall*)(void*, CSchemaType**, const char*)>(this, 4)(this, &schema_type, szName.data());
            return schema_type;
        } else {
            return Virtual::Get<CSchemaType*(__thiscall*)(void*, const char*)>(this, 4)(this, szName.data());
        }
    }

    [[nodiscard]] CSchemaType_Builtin* Type_Builtin(const SchemaBuiltinType_t type) {
        if constexpr (kSchemaSystemVersion == 2) {
            CSchemaType_Builtin* type_builtin;

            Virtual::Get<void(__thiscall*)(void*, CSchemaType_Builtin**, SchemaBuiltinType_t)>(this, 5)(this, &type_builtin, type);
            return type_builtin;
        } else {
            return Virtual::Get<CSchemaType_Builtin*(__thiscall*)(void*, SchemaBuiltinType_t)>(this, 5)(this, type);
        }
    }

    [[nodiscard]] CSchemaType_DeclaredClass* Type_DeclaredClass(const std::string_view szName) {
        if constexpr (kSchemaSystemVersion == 2) {
            CSchemaType_DeclaredClass* declared_class;

            Virtual::Get<void(__thiscall*)(void*, CSchemaType_DeclaredClass**, const char*)>(this, kSchemaSystemTypeScope_DeclaredClass)(
                this, &declared_class, szName.data());

            return declared_class;
        } else {
            return Virtual::Get<CSchemaType_DeclaredClass*(__thiscall*)(void*, const char*)>(this, kSchemaSystemTypeScope_DeclaredClass)(this,
                                                                                                                                         szName.data());
        }
    }

    [[nodiscard]] CSchemaType_DeclaredEnum* Type_DeclaredEnum(const std::string_view szName) {
        if constexpr (kSchemaSystemVersion == 2) {
            CSchemaType_DeclaredEnum* declared_class;

            Virtual::Get<void(__thiscall*)(void*, CSchemaType_DeclaredEnum**, const char*)>(this, kSchemaSystemTypeScope_DeclaredEnum)(
                this, &declared_class, szName.data());

            return declared_class;
        } else {
            return Virtual::Get<CSchemaType_DeclaredEnum*(__thiscall*)(void*, const char*)>(this, kSchemaSystemTypeScope_DeclaredEnum)(this,
                                                                                                                                       szName.data());
        }
    }

    [[nodiscard]] const char* GetScopeName() {
        return Virtual::Get<const char*(__thiscall*)(void*)>(this, kSchemaSystemTypeScope_GetScopeName)(this);
    }

    [[nodiscard]] bool IsGlobalScope() {
        return Virtual::Get<bool(__thiscall*)(void*)>(this, kSchemaSystemTypeScope_IsGlobalScope)(this);
    }

    [[nodiscard]] std::string BGetScopeName() const {
        return m_szName.data();
    }

    [[nodiscard]] CUtlTSHash<CSchemaClassBinding*> GetClassBindings() const {
        return m_ClassBindings;
    }

    [[nodiscard]] CUtlTSHash<CSchemaEnumBinding*> GetEnumBindings() const {
        return m_EnumBindings;
    }

    [[nodiscard]] CUtlMap<std::uint16_t, CSchemaType_DeclaredClass*>& GetDeclaredClasses() {
        return m_DeclaredClasses.m_Map;
    }

    [[nodiscard]] CUtlMap<std::uint16_t, CSchemaType_DeclaredEnum*>& GetDeclaredEnums() {
        return m_DeclaredEnums.m_Map;
    }

private:
    void* vftable = nullptr;
    std::array<char, 256> m_szName = {}; // 0x0008
    CSchemaSystemTypeScope* m_pGlobalTypeScope = nullptr; // 0x0108
    bool m_bBuiltinTypesInitialized = false; // 0x0110
    std::array<CSchemaType_Builtin, kSchemaBuiltinTypeCount> m_BuiltinTypes = {}; // 0x0118
    CSchemaPtrMap<CSchemaType*, CSchemaType_Ptr*> m_Ptrs; // 0x0348
    CSchemaPtrMap<int, CSchemaType_Atomic*> m_Atomics; // 0x0378
    CSchemaPtrMap<AtomicTypeInfo_T_t, CSchemaType_Atomic_T*> m_AtomicsT; // 0x03A8
    CSchemaPtrMap<AtomicTypeInfo_T_t, CSchemaType_Atomic_CollectionOfT*> m_AtomicsCollectionOfT; // 0x03D8

#if defined(CS2_OLD)
    CSchemaPtrMap<AtomicTypeInfo_TF_t, CSchemaType_Atomic_TF*> m_AtomicsTF; // 0x0408
#endif

    CSchemaPtrMap<AtomicTypeInfo_TT_t, CSchemaType_Atomic_TT*> m_AtomicsTT; // 0x0438

#if defined(CS2_OLD)
    CSchemaPtrMap<AtomicTypeInfo_TTF_t, CSchemaType_Atomic_TTF*> m_AtomicsTTF; // 0x0468
#endif

    CSchemaPtrMap<AtomicTypeInfo_I_t, CSchemaType_Atomic_I*> m_AtomicsI; // 0x0498
    CSchemaPtrMap<std::uint16_t, CSchemaType_DeclaredClass*> m_DeclaredClasses; // 0x04C8
    CSchemaPtrMap<std::uint16_t, CSchemaType_DeclaredEnum*> m_DeclaredEnums; // 0x04F8
    CSchemaPtrMap<int, const SchemaAtomicTypeInfo_t*> m_AtomicInfos; // 0x0528
    CSchemaPtrMap<TypeAndCountInfo_t, CSchemaType_FixedArray*> m_FixedArrays; // 0x0558
    CSchemaPtrMap<int, CSchemaType_Bitfield*> m_Bitfields; // 0x0588

#if !defined(DOTA2) && !defined(CS2)
    CSchemaType_NoschemaType m_pNoschemaType = {};
#endif

    CUtlTSHash<CSchemaClassBinding*> m_ClassBindings = {}; // 0x05C0
    CUtlTSHash<CSchemaEnumBinding*> m_EnumBindings = {}; // 0x2E50
};

enum SchemaTypeScope_t : std::uint8_t {
    SCHEMA_TYPESCOPE_GLOBAL = 0,
    SCHEMA_TYPESCOPE_LOCAL,
    SCHEMA_TYPESCOPE_DEFAULT,
};

class CSchemaSystem {
public:
    [[nodiscard]] CSchemaSystemTypeScope* GlobalTypeScope(void) {
        return Virtual::Get<CSchemaSystemTypeScope*(__thiscall*)(void*)>(this, 11)(this);
    }

    [[nodiscard]] CSchemaSystemTypeScope* FindTypeScopeForModule(const std::string_view pszModuleName) {
        return Virtual::Get<CSchemaSystemTypeScope*(__thiscall*)(void*, const char*, void*)>(this, 13)(this, pszModuleName.data(), nullptr);
    }

    [[nodiscard]] CSchemaSystemTypeScope* GetTypeScopeForBinding(const SchemaTypeScope_t nType, const std::string_view pszBinding) {
        return Virtual::Get<CSchemaSystemTypeScope*(__thiscall*)(void*, SchemaTypeScope_t, const char*)>(this, 14)(this, nType, pszBinding.data());
    }

    // @note: @og: E.g: engine2.dll!CEntityComponent
    [[nodiscard]] CSchemaClassBinding* FindClassByScopedName(const std::string_view pszScopedName) {
        if constexpr (kSchemaSystemVersion == 2) {
            CSchemaClassBinding* binding;

            Virtual::Get<void(__thiscall*)(void*, CSchemaClassBinding**, const char*)>(this, 16)(this, &binding, pszScopedName.data());
            return binding;
        } else {
            return Virtual::Get<CSchemaClassBinding*(__thiscall*)(void*, const char*)>(this, 16)(this, pszScopedName.data());
        }
    }

    [[nodiscard]] std::string ScopedNameForClass(CSchemaClassBinding* pBinding) {
        static CBufferStringGrowable<1024> szBuf;
        Virtual::Get<const char*(__thiscall*)(void*, CSchemaClassBinding*, CBufferString*)>(this, 17)(this, pBinding, &szBuf);
        return szBuf.Get();
    }

    // @note: @og: E.g: engine2.dll!SpawnDebugRestrictionOverrideState_t
    [[nodiscard]] CSchemaEnumBinding* FindEnumByScopedName(const std::string_view pszScopedName) {
        if constexpr (kSchemaSystemVersion == 2) {
            CSchemaEnumBinding* binding;

            Virtual::Get<void(__thiscall*)(void*, CSchemaEnumBinding**, const char*)>(this, 18)(this, &binding, pszScopedName.data());
            return binding;
        } else {
            return Virtual::Get<CSchemaEnumBinding*(__thiscall*)(void*, const char*)>(this, 18)(this, pszScopedName.data());
        }
    }

    [[nodiscard]] std::string GetScopedNameForEnum(CSchemaEnumBinding* pBinding) {
        static CBufferStringGrowable<1024> szBuf;
        Virtual::Get<const char*(__thiscall*)(void*, CSchemaEnumBinding*, CBufferString*)>(this, 19)(this, pBinding, &szBuf);
        return szBuf.Get();
    }

    [[nodiscard]] const char* GetClassInfoBinaryName(CSchemaClassBinding* pBinding) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaClassBinding*)>(this, kSchemaSystem_GetClassInfoBinaryName)(this, pBinding);
    }

    [[nodiscard]] const char* GetClassProjectName(CSchemaClassBinding* pBinding) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaClassBinding*)>(this, kSchemaSystem_GetClassProjectName)(this, pBinding);
    }

    [[nodiscard]] const char* GetEnumBinaryName(CSchemaEnumBinding* pBinding) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaEnumBinding*)>(this, kSchemaSystem_GetEnumBinaryName)(this, pBinding);
    }

    [[nodiscard]] const char* GetEnumProjectName(CSchemaEnumBinding* pBinding) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaEnumBinding*)>(this, kSchemaSystem_GetEnumProjectName)(this, pBinding);
    }

    CSchemaClassBinding* ValidateClasses(CSchemaClassBinding** ppBinding) {
        return Virtual::Get<CSchemaClassBinding*(__thiscall*)(void*, CSchemaClassBinding**)>(this, kSchemaSystem_ValidateClasses)(this, ppBinding);
    }

    [[nodiscard]] bool IsSchemaSystemReady() {
        return Virtual::Get<bool(__thiscall*)(void*)>(this, 26)(this);
    }

    [[nodiscard]] void PrintSchemaStats() {
        Virtual::Get<void(__thiscall*)(void*)>(this, 30)(this);
    }

    // @note: @og: there 2 options, "enum" or "class"
    [[nodiscard]] void PrintSchemaMetaStats(const char* pszOptions) {
        Virtual::Get<void(__thiscall*)(void*, const char*)>(this, 31)(this, pszOptions);
    }

    [[nodiscard]] CUtlVector<CSchemaSystemTypeScope*> GetTypeScopes() const {
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
    char pad_0x0000[kSchemaSystem_PAD0] = {}; // 0x0000
    CUtlVector<CSchemaSystemTypeScope*> m_TypeScopes = {}; // SCHEMASYSTEM_TYPE_SCOPES_OFFSET
    char pad_01A0[kSchemaSystem_PAD1] = {}; // 0x01A0
    std::int32_t m_nRegistrations = 0; // 0x02C0
    std::int32_t m_nIgnored = 0; // 0x02C4
    std::int32_t m_nRedundant = 0; // 0x02C8
    std::size_t m_nIgnoredBytes = 0; // 0x02CC

public:
    [[nodiscard]] static CSchemaSystem* GetInstance(void) {
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
