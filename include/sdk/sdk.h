// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <Include.h>

#include <sdk/interfaces/tier0/IMemAlloc.h>

#include <sdk/interfaces/common/CUtlMap.h>
#include <sdk/interfaces/common/CUtlMemory.h>
#include <sdk/interfaces/common/CUtlString.h>
#include <sdk/interfaces/common/CUtlVector.h>
#include <sdk/interfaces/common/CUtlTSHash.h>
#include <sdk/interfaces/common/CBufferString.h>

#include <sdk/interfaceregs.h>
#include <sdk/interfaces/client/game/datamap_t.h>
#include <sdk/interfaces/schemasystem/Schema.h>

namespace sdk {
    inline CSchemaSystem* g_schema = nullptr;

    void GenerateTypeScopeSdk(CSchemaSystemTypeScope* current);
} // namespace sdk


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

