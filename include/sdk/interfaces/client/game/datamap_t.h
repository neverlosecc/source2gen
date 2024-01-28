// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

class ISaveRestoreOps;
class typedescription_t;
class IPredictionCopyOps;

enum {
    FTYPEDESC_NONE = 0
};

enum DatamapFlags : int
{
    // This field is masked for global entity save/restore
    FTYPEDESC_GLOBAL = (1 << 0),
    // This field is saved to disk
    FTYPEDESC_SAVE = (1 << 1),
    // This field can be requested and written to by string name at load time
    FTYPEDESC_KEY = (1 << 2),
    // This field can be written to by string name at run time, and a function called
    FTYPEDESC_INPUT = (1 << 3),
    // This field propogates it's value to all targets whenever it changes
    FTYPEDESC_OUTPUT = (1 << 4),
    // This is a table entry for a member function pointer
    FTYPEDESC_FUNCTIONTABLE = (1 << 5),
    // This field is a pointer, not an embedded object
    FTYPEDESC_PTR = (1 << 6),
    // The field is an override for one in a base class (only used by prediction system for now)
    FTYPEDESC_OVERRIDE = (1 << 7),
    // Flags used by other systems (e.g., prediction system)
    FTYPEDESC_INSENDTABLE = (1 << 8),
    // This field is present in a network SendTable
    // The field is local to the client or server only (not referenced by prediction code and not replicated by
    // networking
    FTYPEDESC_PRIVATE = (1 << 9),
    // The field is part of the prediction typedescription, but doesn't get compared when checking for errors
    FTYPEDESC_NOERRORCHECK = (1 << 10),
    // The field is a model index (used for debugging output)
    FTYPEDESC_MODELINDEX = (1 << 11),
    // The field is an index into file data, used for byteswapping.
    FTYPEDESC_INDEX = (1 << 12),
    FTYPEDESC_OVERRIDE_RECURSIVE = (1 << 13),
    FTYPEDESC_SCHEMA_INITIALIZED = (1 << 14),
    FTYPEDESC_GEN_ARRAY_KEYNAMES_0 = (1 << 15),
    FTYPEDESC_GEN_ARRAY_KEYNAMES_1 = (1 << 16),
    FTYPEDESC_ADDITIONAL_FIELDS = (1 << 17),
    FTYPEDESC_EXPLICIT_BASE = (1 << 18),
    FTYPEDESC_PROCEDURAL_KEYFIELD = (1 << 19),
    // Used if the typedesc is enum, no datamap_t info would be available
    FTYPEDESC_ENUM = (1 << 20),
    FTYPEDESC_REMOVED_KEYFIELD = (1 << 21),
    FTYPEDESC_WAS_INPUT = (1 << 22),
    FTYPEDESC_WAS_OUTPUT = (1 << 23)
};

enum {
    TD_OFFSET_NORMAL = 0,
    TD_OFFSET_PACKED = 1,

    // Must be last
    TD_OFFSET_COUNT,
};

enum PredictionCopyType_t {
    PC_NON_NETWORKED_ONLY = 0,
    PC_NETWORKED_ONLY,

    PC_COPYTYPE_COUNT,
    PC_EVERYTHING = PC_COPYTYPE_COUNT,
};

// Registered binary: schemasystem.dll (project 'schemasystem')
// Alignment: 1
// Size: 0x50
enum class fieldtype_t : uint8_t {
    FIELD_VOID = 0x0,
    FIELD_FLOAT32 = 0x1,
    FIELD_STRING = 0x2,
    FIELD_VECTOR = 0x3,
    FIELD_QUATERNION = 0x4,
    FIELD_INT32 = 0x5,
    FIELD_BOOLEAN = 0x6,
    FIELD_INT16 = 0x7,
    FIELD_CHARACTER = 0x8,
    FIELD_COLOR32 = 0x9,
    FIELD_EMBEDDED = 0xa,
    FIELD_CUSTOM = 0xb,
    FIELD_CLASSPTR = 0xc,
    FIELD_EHANDLE = 0xd,
    FIELD_POSITION_VECTOR = 0xe,
    FIELD_TIME = 0xf,
    FIELD_TICK = 0x10,
    FIELD_SOUNDNAME = 0x11,
    FIELD_INPUT = 0x12,
    FIELD_FUNCTION = 0x13,
    FIELD_VMATRIX = 0x14,
    FIELD_VMATRIX_WORLDSPACE = 0x15,
    FIELD_MATRIX3X4_WORLDSPACE = 0x16,
    FIELD_INTERVAL = 0x17,
    FIELD_UNUSED = 0x18,
    FIELD_VECTOR2D = 0x19,
    FIELD_INT64 = 0x1a,
    FIELD_VECTOR4D = 0x1b,
    FIELD_RESOURCE = 0x1c,
    FIELD_TYPEUNKNOWN = 0x1d,
    FIELD_CSTRING = 0x1e,
    FIELD_HSCRIPT = 0x1f,
    FIELD_VARIANT = 0x20,
    FIELD_UINT64 = 0x21,
    FIELD_FLOAT64 = 0x22,
    FIELD_POSITIVEINTEGER_OR_NULL = 0x23,
    FIELD_HSCRIPT_NEW_INSTANCE = 0x24,
    FIELD_UINT32 = 0x25,
    FIELD_UTLSTRINGTOKEN = 0x26,
    FIELD_QANGLE = 0x27,
    FIELD_NETWORK_ORIGIN_CELL_QUANTIZED_VECTOR = 0x28,
    FIELD_HMATERIAL = 0x29,
    FIELD_HMODEL = 0x2a,
    FIELD_NETWORK_QUANTIZED_VECTOR = 0x2b,
    FIELD_NETWORK_QUANTIZED_FLOAT = 0x2c,
    FIELD_DIRECTION_VECTOR_WORLDSPACE = 0x2d,
    FIELD_QANGLE_WORLDSPACE = 0x2e,
    FIELD_QUATERNION_WORLDSPACE = 0x2f,
    FIELD_HSCRIPT_LIGHTBINDING = 0x30,
    FIELD_V8_VALUE = 0x31,
    FIELD_V8_OBJECT = 0x32,
    FIELD_V8_ARRAY = 0x33,
    FIELD_V8_CALLBACK_INFO = 0x34,
    FIELD_UTLSTRING = 0x35,
    FIELD_NETWORK_ORIGIN_CELL_QUANTIZED_POSITION_VECTOR = 0x36,
    FIELD_HRENDERTEXTURE = 0x37,
    FIELD_HPARTICLESYSTEMDEFINITION = 0x38,
    FIELD_UINT8 = 0x39,
    FIELD_UINT16 = 0x3a,
    FIELD_CTRANSFORM = 0x3b,
    FIELD_CTRANSFORM_WORLDSPACE = 0x3c,
    FIELD_HPOSTPROCESSING = 0x3d,
    FIELD_MATRIX3X4 = 0x3e,
    FIELD_SHIM = 0x3f,
    FIELD_CMOTIONTRANSFORM = 0x40,
    FIELD_CMOTIONTRANSFORM_WORLDSPACE = 0x41,
    FIELD_ATTACHMENT_HANDLE = 0x42,
    FIELD_AMMO_INDEX = 0x43,
    FIELD_CONDITION_ID = 0x44,
    FIELD_AI_SCHEDULE_BITS = 0x45,
    FIELD_MODIFIER_HANDLE = 0x46,
    FIELD_ROTATION_VECTOR = 0x47,
    FIELD_ROTATION_VECTOR_WORLDSPACE = 0x48,
    FIELD_HVDATA = 0x49,
    FIELD_SCALE32 = 0x4a,
    FIELD_STRING_AND_TOKEN = 0x4b,
    FIELD_ENGINE_TIME = 0x4c,
    FIELD_ENGINE_TICK = 0x4d,
    FIELD_WORLD_GROUP_ID = 0x4e,
    FIELD_TYPECOUNT = 0x4f,
};

// Each datamap_t is broken down into two flattened arrays of fields,
//  one for PC_NETWORKED_DATA and one for PC_NON_NETWORKED_ONLY (optimized_datamap_t::datamapinfo_t::flattenedoffsets_t)
// Each flattened array is sorted by offset for better cache performance
// Finally, contiguous "runs" off offsets are precomputed (optimized_datamap_t::datamapinfo_t::datacopyruns_t) for fast
// copy operations

// A data run is a set of DEFINE_PRED_FIELD fields in a c++ object which are contiguous and can be processing
//  using a single memcpy operation
struct datarun_t {
    datarun_t(): m_nStartFlatField(0), m_nEndFlatField(0), m_nLength(0) {
        for (int i = 0; i < TD_OFFSET_COUNT; ++i) {
            m_nStartOffset[i] = 0;
#ifdef _X360
            // These are the offsets of the next run, for priming the L1 cache
            m_nPrefetchOffset[i] = 0;
#endif
        }
    }

    // Indices of start/end fields in the flattened typedescription_t list
    int m_nStartFlatField;
    int m_nEndFlatField;

    // Offsets for run in the packed/unpacked data (I think the run starts need to be properly aligned)
    int m_nStartOffset[TD_OFFSET_COUNT];
#ifdef _X360
    // These are the offsets of the next run, for priming the L1 cache
    int m_nPrefetchOffset[TD_OFFSET_COUNT];
#endif
    int m_nLength;
};

struct datacopyruns_t {
public:
    CUtlVector<datarun_t> m_vecRuns;
};

struct flattenedoffsets_t {
    CUtlVector<typedescription_t> m_Flattened;
    int m_nPackedSize; // Contiguous memory to pack all of these together for TD_OFFSET_PACKED
    int m_nPackedStartOffset;
};

struct datamapinfo_t {
    // Flattened list, with FIELD_EMBEDDED, FTYPEDESC_PRIVATE,
    //  and FTYPEDESC_OVERRIDE (overridden) fields removed
    flattenedoffsets_t m_Flat;
    datacopyruns_t m_CopyRuns;
};

struct optimized_datamap_t {
    // Optimized info for PC_NON_NETWORKED and PC_NETWORKED data
    datamapinfo_t m_Info[PC_COPYTYPE_COUNT];
};

class datamap_t {
public:
    typedescription_t* m_pTypeDescription;
    std::uint64_t m_iTypeDescriptionCount;
    const char* m_pszClassName; // Ex: C_DOTAPlayer
    datamap_t* m_pBaseDatamap; // Ex: For C_DOTAPlayer it would be next baseclass C_BasePlayer, can be NULL
    int m_nPackedSize;
    optimized_datamap_t* m_pOptimizedDataMap;
};

class typedescription_t {
public:
    std::string_view GetFieldName() {
        if (m_pszFieldName)
            return {m_pszFieldName};

        return {};
    }

    std::string_view GetExternalFieldName() {
        if (m_pszExternalName)
            return {m_pszExternalName};

        return {};
    }

public:
    fieldtype_t m_iFieldType;
    const char* m_pszFieldName;
    int m_iOffset; // Local offset value
    std::uint16_t m_nFieldSize;
    DatamapFlags m_nFlags;
    // the name of the variable in the map/fgd data, or the name of the action
    const char* m_pszExternalName;
    // pointer to the function set for save/restoring of custom data types
    ISaveRestoreOps* m_pSaveRestoreOps;
    // for associating function with string names
    void* m_pInputFn;

    // For embedding additional datatables inside this one
    union {
        datamap_t* m_pDataMap;
        const char* m_pszEnumName;
    };

    // Stores the actual member variable size in bytes
    int m_iFieldSizeInBytes;

    // Tolerance for field errors for float fields
    float m_flFieldTolerance;

    // For raw fields (including children of embedded stuff) this is the flattened offset
    int m_flFlatOffset[TD_OFFSET_COUNT];
    std::uint16_t m_uFlatGroup;

    IPredictionCopyOps* pPredictionCopyOps;
    datamap_t* m_pPredictionCopyDataMap;

    ~typedescription_t();
};
static_assert(sizeof(typedescription_t) == 0x68);

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
