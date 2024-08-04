#pragma once

#include <cstdint>
#include <string_view>

template <class>
using CAnimValue = char[0x08];
using CAnimVariant = char[0x14];
// size is a guess
template <class T>
using CAnimScriptParam = char[0x08];
using CBufferString = char[0x10];
using CColorGradient = char[0x18];
// size doesn't mapper. only used as a pointer
template <class>
using CCompressor = char[0x01];
using CEntityHandle = char[0x04];
using CEntityIndex = char[0x04];
using CGlobalSymbol = char[0x08];
using CKV3MemberNameWithStorage = char[0x38];
using CNetworkedQuantizedFloat = char[0x08];
using CParticleNamedValueRef = char[0x40];
using CPiecewiseCurve = char[0x40];
using CPlayerSlot = char[0x04];
using CPulseValueFullType = char[0x10];
using CResourceName = char[0xe0];
using CSplitScreenSlot = char[0x04];
using CTransform = char[0x20];
using CUtlBinaryBlock = char[0x18];
template <class, class>
using CUtlHashtable = char[0x20];
using CUtlStringTokenWithStorage = char[0x18];
using CUtlStringToken = char[0x04];
using CUtlString = char[0x08];
using CUtlSymbolLarge = char[0x08];
using CUtlSymbol = char[0x02];
template <class>
using CAnimGraphParamOptionalRef = char[0x20];
template <class>
using CAnimGraphParamRef = char[0x20];
template <int N>
using CBitVec = char[(N + 7) / 8];
template <class>
using CEntityOutputTemplate = char[0x28];
template <class>
using CHandle = char[0x04];
template <class>
using C_NetworkUtlVectorBase = char[0x18];
// size unknown. only used in dynamic containers.
using CSoundEventName = char[0x01];
template <class>
using CUtlLeanVector = char[0x10];
template <class, class>
using CUtlOrderedMap = char[0x28];
// size doesn't mapper. only used as a pointer
template <class, class>
using CUtlPair = char[0x01];
template <class>
using CUtlVector = char[0x18];
// size is a guess that fits both occurences of this type in CS2
template <class T>
using CUtlVectorFixedGrowable = char[0x18 + ((sizeof(T) < 4) ? 4 : sizeof(T))];
template <class T>
using CUtlLeanVectorFixedGrowable = char[0x18 + ((sizeof(T) < 4) ? 4 : sizeof(T))];
template <class>
using C_UtlVectorEmbeddedNetworkVar = char[0x50];
using CUtlVectorSIMDPaddedVector = char[0x18];
template <class>
using CSmartPtr = char[0x08];
template <class>
using CResourceNameTyped = char[0xe0];
template <class>
using CStrongHandle = char[0x08];
template <class>
using CStrongHandleCopyable = char[0x08];
// size doesn't mapper. only used as a pointer
using CStrongHandleVoid = char[0x08];
template <class>
using CVariantBase = char[0x10];
template <class>
using CWeakHandle = char[0x18];
using Color = char[0x04];
using DegreeEuler = char[0x0c];
using FourVectors = char[0x30];
using HSCRIPT = char[0x08];
using KeyValues3 = char[0x10];
// size doesn't mapper. only used as a pointer
using KeyValues = char[0x01];
using QAngle = char[0x0c];
using QuaternionStorage = char[0x10];
using Quaternion = char[0x10];
using RadianEuler = char[0x14];
// we don't have a field size for this type. uses the fallback of 1.
using RenderPrimitiveType_t = char[0x01];
using RotationVector = char[0x0c];
template <class>
using SphereBase_t = char[0x10];
using Vector2D = char[0x08];
using Vector4D = char[0x10];
using VectorAligned = char[0x10];
using Vector = char[0x0c];
using WorldGroupId_t = char[0x04];
using float32 = char[0x04];
using fltx4 = char[0x10];
using matrix3x4_t = char[0x30];
using matrix3x4a_t = char[0x30];
using uint256_t = char[0x20];

using uint8 = std::uint8_t; // TOOD: happens in CUtlVector<unit8>. how to fix?

// intentionally left undefined. if you want to access static fields, add your own sdk.
namespace interfaces {
    struct SchemaStaticFieldData_t {
        void* m_pInstance{};
    };

    struct CSchemaClassInfo {
        auto GetStaticFields() -> SchemaStaticFieldData_t**;
    };

    struct CSchemaSystemTypeScope {
        auto FindDeclaredClass(std::string_view) -> CSchemaClassInfo*;
    };

    struct schema_t {
        auto FindTypeScopeForModule(std::string_view) -> CSchemaSystemTypeScope*;
    };

    extern schema_t* g_schema;
} // namespace interfaces
