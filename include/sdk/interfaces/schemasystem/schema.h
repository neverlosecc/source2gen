#pragma once

#include <Include.h>
#include <SDK/Interfaces/common/CUtlTSHash.h>
#include <tools/virtual.h>

#define DOTA2

#ifdef SBOX
// untested, CSchemaType::m_schema_type_ might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x5420
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x27FC
#elif defined ARTIFACT2
// untested, CSchemaType::m_schema_type_ might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x5430
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x2804
#elif defined ARTIFACT1
// untested, CSchemaType::m_schema_type_ might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x4428
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x4B8
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x2001
#elif defined UNDERLORDS
// untested, CSchemaType::m_schema_type_ might be wrong
    #define CSCHEMATYPE_GETSIZES_INDEX 5
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x5420
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x27FC
#elif defined DESKJOB
    #define CSCHEMATYPE_GETSIZES_INDEX 3
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x3A0
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x2804
#elif defined HL_ALYX
    #error "unimplemented"
#elif defined THE_LAB_ROBOT_REPAIR
    #error "unimplemented"
#elif defined(CSGO2) || defined(DOTA2)
    #define CSCHEMATYPE_GETSIZES_INDEX 3
    #define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x190
    #define SCHEMASYSTEMTYPESCOPE_OFF1 0x47E
    #define SCHEMASYSTEMTYPESCOPE_OFF2 0x2808
    #define SCHEMASYSTEM_FIND_DECLARED_CLASS_TYPE 2
#endif

class CSchemaClassInfo;
class CSchemaSystemTypeScope;
class CSchemaType;

struct SchemaMetadataEntryData_t;
struct SchemaClassInfoData_t;

using SchemaString_t = const char*;

enum SchemaClassFlags_t {
    SCHEMA_CF1_HAS_VIRTUAL_MEMBERS = 1,
    SCHEMA_CF1_IS_ABSTRACT = 2,
    SCHEMA_CF1_HAS_TRIVIAL_CONSTRUCTOR = 4,
    SCHEMA_CF1_HAS_TRIVIAL_DESTRUCTOR = 8,
    SCHEMA_CF1_HAS_NOSCHEMA_MEMBERS = 16,
    SCHEMA_CF1_IS_PARENT_CLASSES_PARSED = 32,
    SCHEMA_CF1_IS_LOCAL_TYPE_SCOPE = 64,
    SCHEMA_CF1_IS_GLOBAL_TYPE_SCOPE = 128,
    SCHEMA_CF1_UNKNOWN2 = 256, // @note: @og: idk
    SCHEMA_CF1_IS_NOSCHEMA_CLASS = 2048,
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

template <typename T>
struct SchemaArray {
public:
    T* begin() const {
        return m_data;
    }

    T* end() const {
        return m_data + m_size;
    }

    T* m_data;
    unsigned int m_size;
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

struct CSchemaClassBinding {
    CSchemaClassBinding* parent;
    const char* m_binary_name; // ex: C_World
    const char* m_module_name; // ex: libclient.so
    const char* m_class_name; // ex: client
    void* m_class_info_old_synthesized;
    void* m_class_info;
    void* m_this_module_binding_pointer;
    CSchemaType* m_schema_type;
};

struct SchemaMetadataEntryData_t {
    SchemaString_t m_name;
    CSchemaNetworkValue* m_value;
};

struct SchemaEnumeratorInfoData_t {
    SchemaString_t m_name;

    union {
        unsigned char m_value_char;
        unsigned short m_value_short;
        unsigned int m_value_int;
        unsigned long long m_value;
    };

    char pad_0x0010[0x10]; // 0x0010
};

class CSchemaEnumInfo {
public:
    SchemaEnumeratorInfoData_t m_field_;
};

class CSchemaEnumBinding {
public:
    virtual const char* GetBindingName() = 0;
    virtual CSchemaClassBinding* AsClassBinding() = 0;
    virtual CSchemaEnumBinding* AsEnumBinding() = 0;
    virtual const char* GetBinaryName() = 0;
    virtual const char* GetProjectName() = 0;
public:
    char* m_binding_name; // 0x0008
    char* m_dll_name; // 0x0010
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

class CSchemaType {
public:
    bool GetSizes(int* out_size1, uint8_t* unk_probably_not_size) {
        return reinterpret_cast<int(__thiscall*)(void*, int*, uint8_t*)>(_vtable[CSCHEMATYPE_GETSIZES_INDEX])(this, out_size1, unk_probably_not_size);
    }
public:
    bool GetSize(int* out_size) {
        uint8_t smh = 0;
        return GetSizes(out_size, &smh);
    }
public:
    uintptr_t* _vtable; // 0x0000
    const char* m_name_; // 0x0008

    CSchemaSystemTypeScope* m_type_scope_; // 0x0010
    uint8_t type_category; // ETypeCategory 0x0018
    uint8_t atomic_category; // EAtomicCategory 0x0019

    // find out to what class pointer points.
    CSchemaType* GetRefClass() {
        if (type_category != Schema_Ptr)
            return nullptr;

        auto ptr = m_schema_type_;
        while (ptr && ptr->type_category == ETypeCategory::Schema_Ptr)
            ptr = ptr->m_schema_type_;

        return ptr;
    }

    struct array_t {
        uint32_t array_size;
        uint32_t unknown;
        CSchemaType* element_type_;
    };

    struct atomic_t { // same goes for CollectionOfT
        uint64_t gap[2];
        CSchemaType* template_typename;
    };

    struct atomic_tt {
        uint64_t gap[2];
        CSchemaType* templates[2];
    };

    struct atomic_i {
        uint64_t gap[2];
        uint64_t integer;
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
    SchemaString_t m_name; // 0x0000
    CSchemaType* m_type; // 0x0008
    std::int32_t m_single_inheritance_offset; // 0x0010
    std::int32_t m_metadata_size; // 0x0014
    SchemaMetadataEntryData_t* m_metadata; // 0x0018
};

struct CSchemaClassInfoField_t {
    char pad_0000[8]; // 0x0000
    SchemaString_t m_name; // 0x0008
    std::uint32_t m_single_inheritance_offset; // 0x0010
    char pad_0014[84]; // 0x0014
}; // Size: 0x0068
static_assert(sizeof(CSchemaClassInfoField_t) == 0x68);

struct SchemaStaticFieldData_t {
    const char* name; // 0x0000
    CSchemaType* m_type; // 0x0008
    void* m_instance; // 0x0010
    char pad_0x0018[0x10]; // 0x0018
};

struct SchemaBaseClassInfoData_t {
    unsigned int m_offset;
    CSchemaClassInfo* m_class;
};

struct CSchemaClassInfoFieldsData_t {
    CSchemaClassInfoField_t* m_field;
    std::int32_t m_size;
};

struct SchemaClassInfoData_t {
private:
    enum class SchemaClassInitialization_t : std::int32_t {
        kInitialize = 0,
        kNone,
        kMetadataInitialize,
        kCreateSomeStruct,
        kReInitialize,
        kInitializeSomeStruct,
        kDestroy,
        kGetSomePtr
    };
    using InitializationFn = void(*)(SchemaClassInitialization_t, SchemaClassInfoData_t*, SchemaClassInfoData_t*);

public:
    SchemaClassInfoData_t* m_self; // 0x0000
    const char* m_name; // 0x0008
    const char* m_module; // 0x0010
    int m_size; // 0x0018
    std::int16_t m_fields_size; // 0x001C
    std::int16_t m_static_size; // 0x001E
    std::int16_t m_metadata_size; // 0x0020
    std::uint8_t m_align_of; // 0x0022
    std::uint8_t m_has_base_class; // 0x0023
    std::int16_t m_total_class_size; // 0x0024 // @note: @og: if there no derived or base class then it will be 1 otherwise derived class size + 1.
    std::int16_t m_derived_class_size; // 0x0026
    SchemaClassFieldData_t* m_fields; // 0x0028
    SchemaStaticFieldData_t* m_static_fields; // 0x0030
    SchemaBaseClassInfoData_t* m_schema_parent; // 0x0038
    CSchemaClassInfoFieldsData_t* m_fields_backup; // 0x0040 // @note: @og: sometimes it duplicates m_fields, cant find where it being used
    SchemaMetadataEntryData_t* m_metadata; // 0x0048
    CSchemaSystemTypeScope* m_type_scope; // 0x0050
    CSchemaType* m_shema_type; // 0x0058
    SchemaClassFlags_t m_class_flags; // 0x0060
    std::uint32_t m_sequence; // 0x0064 // @note: @og: idk
    InitializationFn initialization_fn; // 0x0068

public:
    [[nodiscard]] std::uint8_t GetAligment() const {
        return m_align_of == std::numeric_limits<std::uint8_t>::max() ? 8 : m_align_of;
    }
};

class CSchemaClassInfo : public SchemaClassInfoData_t {
public:
    bool GetMetaStrings(const char* metaName, std::vector<const char**>& strings);

    unsigned int CalculateInheritanceDataSize(bool ignoreVirtuals = false);

    bool DependsOn(CSchemaClassInfo* other);
    bool InheritsFrom(CSchemaClassInfo* other);
    bool UsesClass(CSchemaClassInfo* other);
    bool InheritsVirtuals();

    void FillClassFieldsList(std::vector<SchemaClassFieldData_t*>& fields);
    void FillInheritanceList(std::vector<const char*>& inheritance);
private:
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
    char pad_0x0000[0x8]; // 0x0000
    std::array<char, 256> m_name_ = {};
    char pad_0x0108[SCHEMASYSTEMTYPESCOPE_OFF1]; // 0x0108
    CUtlTSHash<CSchemaClassBinding*> m_classes_; // 0x0588
    char pad_0x0594[SCHEMASYSTEMTYPESCOPE_OFF2]; // 0x05C8
    CUtlTSHash<CSchemaEnumBinding*> m_enumes_; // 0x2DD0
private:
    static constexpr unsigned int s_class_list = 0x580;
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

    const char* GetClassInfoBinaryName(SchemaClassInfoData_t* class_info) {
        return Virtual::Get<const char*(__thiscall*)(void*, SchemaClassInfoData_t*)>(this, 22)(this, class_info);
    }

    const char* GetClassProjectName(SchemaClassInfoData_t* class_info) {
        return Virtual::Get<const char*(__thiscall*)(void*, SchemaClassInfoData_t*)>(this, 23)(this, class_info);
    }

    const char* GetEnumBinaryName(CSchemaEnumBinding* class_info) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaEnumBinding*)>(this, 24)(this, class_info);
    }

    const char* GetEnumProjectName(CSchemaEnumBinding* class_info) {
        return Virtual::Get<const char*(__thiscall*)(void*, CSchemaEnumBinding*)>(this, 25)(this, class_info);
    }

    bool SchemaSystemIsReady()
    {
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
    char pad_01A0[288]; // 0x01A0
    std::int32_t m_registrations_; // 0x02C0
    std::int32_t m_ignored_; // 0x02C4
    std::int32_t m_redundant_; // 0x02C8
    char pad_02CC[4]; // 0x02CC
    std::int32_t m_ignored_bytes_; // 0x02D0

public:
    static CSchemaSystem* GetInstance(void) {
        return sdk::GetInterface<CSchemaSystem>("schemasystem.dll", "SchemaSystem_0");
    }
};
