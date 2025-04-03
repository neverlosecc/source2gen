#pragma once
#include <cstdint>
#include <string_view>

template <typename>
using CAnimValue = char[0x08];
using CAnimVariant = char[0x11];
// size is a guess
template <typename T>
using CAnimScriptParam = char[0x08];
using CBufferString = char[0x10];
using CColorGradient = char[0x18];
// size doesn't matter. only used as a pointer
template <typename>
using CCompressor = char[0x01];
using CEntityHandle = char[0x04];
using CEntityIndex = char[0x04];
using CGlobalSymbol = char[0x08];
using CKV3MemberNameWithStorage = char[0x38];
using CNetworkedQuantizedFloat = char[0x08];
using CParticleNamedValueRef = char[0x40];
using CPiecewiseCurve = char[0x40];
using CPlayerSlot = char[0x04];
// size is 8 bytes bigger in Deadlock
using CPulseValueFullType = char[0x10];
using CResourceName = char[0xe0];
using CSplitScreenSlot = char[0x04];
using CTransform = char[0x20];
using CUtlBinaryBlock = char[0x18];
template <typename, typename>
using CUtlHashtable = char[0x20];
using CUtlStringTokenWithStorage = char[0x18];
using CUtlStringToken = char[0x04];
using CUtlString = char[0x08];
using CUtlSymbolLarge = char[0x08];
using CUtlSymbol = char[0x02];
template <typename>
using CAnimGraphParamOptionalRef = char[0x20];
template <typename>
using CAnimGraphParamRef = char[0x20];
template <int N>
using CBitVec = char[(N + 7) / 8];
template <typename>
using CEntityOutputTemplate = char[0x28];
template <typename>
using CHandle = char[0x04];
template <typename>
using C_NetworkUtlVectorBase = char[0x18];
template <typename>
using CNetworkUtlVectorBase = char[0x18];
using CSoundEventName = char[0x10];
template <typename>
using CUtlLeanVector = char[0x10];
template <typename, class>
using CUtlOrderedMap = char[0x28];
// size doesn't matter. only used as a pointer
template <typename, class>
using CUtlPair = char[0x01];
template <typename>
using CUtlVector = char[0x18];
// size is a guess that fits both occurences of this type in CS2
template <typename Ty>
using CUtlVectorFixedGrowable = char[0x18 + ((sizeof(Ty) < 4) ? 4 : sizeof(Ty))];
template <typename Ty>
using CUtlLeanVectorFixedGrowable = char[0x10 + ((sizeof(Ty) < 4) ? 4 : sizeof(Ty))];
template <typename>
using C_UtlVectorEmbeddedNetworkVar = char[0x50];
template <typename>
using CUtlVectorEmbeddedNetworkVar = char[0x50];
using CUtlVectorSIMDPaddedVector = char[0x18];
template <typename>
using CSmartPtr = char[0x08];
template <typename>
using CResourceArray = char[0x08];
// size unknown
using CResourceString = char[0x08];
template <typename>
using CResourcePointer = char[0x08];
template <typename>
using CResourceNameTyped = char[0xe0];
template <typename>
using CStrongHandle = char[0x08];
template <typename>
using CStrongHandleCopyable = char[0x08];
// size doesn't matter. only used as a pointer
using CStrongHandleVoid = char[0x08];
template <typename>
using CVariantBase = char[0x10];
template <typename>
using CWeakHandle = char[0x18];
using CSmartPropAttributeVector = char[0x40];
using CSmartPropAttributeFloat = char[0x40];
using CSmartPropAttributeBool = char[0x40];
using CSmartPropAttributeColor = char[0x40];
using CSmartPropAttributeInt = char[0x40];
using CSmartPropAttributeModelName = char[0x40];
using CSmartPropAttributeMaterialGroup = char[0x40];
using CSmartPropAttributeVector2D = char[0x40];
using CSmartPropVariableComparison = char[0x20];
using CSmartPropAttributeAngles = char[0x40];
using CSmartPropAttributeStateName = char[0x40];
using CSmartPropAttributeVariableValue = char[0x40];
using Range_t = char[0x8];
using V_uuid_t = char[0x10];
using CAnimGraphTagOptionalRef = char[0x18];
using CPanoramaImageName = char[0x10];
using CAnimGraphTagRef = char[0x18];
using CAttachmentNameSymbolWithStorage = char[0x20];
using CGlobalSymbolCaseSensitive = char[0x8];
using CKV3MemberNameSet = char[0x10];
using CSmartPropAttributeMaterialName = char[0x40];
using PulseSymbol_t = char[0x10];
using Color = char[0x04];
using DegreeEuler = char[0x0c];
using FourVectors = char[0x30];
using HSCRIPT = char[0x08];
using KeyValues3 = char[0x10];
// size doesn't matter. only used as a pointer
using KeyValues = char[0x01];
using QAngle = char[0x0c];
using QuaternionStorage = char[0x10];
using Quaternion = char[0x10];
using RadianEuler = char[0x0c];
using RotationVector = char[0x0c];
template <typename>
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
