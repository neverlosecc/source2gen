#pragma once

// template <class>
// using CAnimValue = char[0x08];
typedef char CAnimVariant[0x11];
// size is a guess
// template <class T>
// typedef  char CAnimScriptParam [0x08];
typedef char CBufferString[0x10];
typedef char CColorGradient[0x18];
// size doesn't mapper. only used as a pointer
// template <class>
// typedef  char CCompressor [0x01];
typedef char CEntityHandle[0x04];
typedef char CEntityIndex[0x04];
typedef char CGlobalSymbol[0x08];
typedef char CKV3MemberNameWithStorage[0x38];
typedef char CNetworkedQuantizedFloat[0x08];
typedef char CParticleNamedValueRef[0x40];
typedef char CPiecewiseCurve[0x40];
typedef char CPlayerSlot[0x04];
typedef char CPulseValueFullType[0x10];
typedef char CResourceName[0xe0];
typedef char CSplitScreenSlot[0x04];
typedef char CTransform[0x20];
typedef char CUtlBinaryBlock[0x18];
// template <class, class>
// typedef  char CUtlHashtable [0x20];
typedef char CUtlStringTokenWithStorage[0x18];
typedef char CUtlStringToken[0x04];
typedef char CUtlString[0x08];
typedef char CUtlSymbolLarge[0x08];
typedef char CUtlSymbol[0x02];
// template <class>
// typedef  char CAnimGraphParamOptionalRef [0x20];
// template <class>
// typedef  char CAnimGraphParamRef [0x20];
// template <int N>
// typedef  char CBitVec [(N + 7) / 8];
// template <class>
// // typedef  char CEntityOutputTemplate [0x28];
// // template <class>
// typedef  char CHandle [0x04];
// template <class>
// typedef  char C_NetworkUtlVectorBase [0x18];
// template <class>
// typedef  char CNetworkUtlVectorBase [0x18];
// size unknown. only used in dynamic containers.
typedef char CSoundEventName[0x01];
// template <class>
// typedef  char CUtlLeanVector [0x10];
// template <class, class>
// typedef  char CUtlOrderedMap [0x28];
// size doesn't mapper. only used as a pointer
// template <class, class>
// typedef  char CUtlPair [0x01];
// template <class>
// typedef  char CUtlVector [0x18];
// size is a guess that fits both occurences of this type in CS2
// template <class T>
// typedef  char CUtlVectorFixedGrowable [0x18 + ((sizeof(T) < 4) ? 4 : sizeof(T))];
// template <class T>
// typedef  char CUtlLeanVectorFixedGrowable [0x10 + ((sizeof(T) < 4) ? 4 : sizeof(T))];
// template <class>
// typedef  char C_UtlVectorEmbeddedNetworkVar [0x50];
// template <class>
// typedef  char CUtlVectorEmbeddedNetworkVar [0x50];
typedef char CUtlVectorSIMDPaddedVector[0x18];
// template <class>
// typedef  char CSmartPtr [0x08];
// template <class>
// typedef  char CResourceArray [0x08];
// size unknown
typedef char CResourceString[0x08];
// template <class>
// typedef  char CResourcePointer [0x08];
// template <class>
// typedef  char CResourceNameTyped [0xe0];
// template <class>
// typedef  char CStrongHandle [0x08];
// template <class>
// typedef  char CStrongHandleCopyable [0x08];
// size doesn't mapper. only used as a pointer
typedef char CStrongHandleVoid[0x08];
// template <class>
// typedef  char CVariantBase [0x10];
// template <class>
// typedef  char CWeakHandle [0x18];
typedef char Color[0x04];
typedef char DegreeEuler[0x0c];
typedef char FourVectors[0x30];
typedef char HSCRIPT[0x08];
typedef char KeyValues3[0x10];
// size doesn't mapper. only used as a pointer
typedef char KeyValues[0x01];
typedef char QAngle[0x0c];
typedef char QuaternionStorage[0x10];
typedef char Quaternion[0x10];
typedef char RadianEuler[0x0c];
typedef char RenderInputLayoutField_t[0x04];
// we don't have a field size for this type. uses the fallback of 1.
enum RenderPrimitiveType_t : char {
    RenderPrimitiveType_t_unused,
};
typedef char RotationVector[0x0c];
// template <class>
// typedef  char SphereBase_t [0x10];
typedef char Vector2D[0x08];
typedef char Vector4D[0x10];
typedef char VectorAligned[0x10];
typedef char Vector[0x0c];
typedef char WorldGroupId_t[0x04];
typedef char float32[0x04];
typedef char fltx4[0x10];
typedef char matrix3x4_t[0x30];
typedef char matrix3x4a_t[0x30];
