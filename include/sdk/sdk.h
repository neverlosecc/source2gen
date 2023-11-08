#pragma once

#include <Include.h>

#include <sdk/interfaces/tier0/IMemAlloc.h>

#include <sdk/interfaces/common/CUtlMap.h>
#include <sdk/interfaces/common/CUtlMemory.h>
#include <sdk/interfaces/common/CUtlString.h>
#include <sdk/interfaces/common/CUtlVector.h>
#include <sdk/interfaces/common/CUtlTSHash.h>

#include <sdk/interfaceregs.h>
#include <sdk/interfaces/schemasystem/Schema.h>

namespace sdk {
    inline CSchemaSystem* g_schema = nullptr;

    void GenerateTypeScopeSdk(CSchemaSystemTypeScope* current);
} // namespace sdk
