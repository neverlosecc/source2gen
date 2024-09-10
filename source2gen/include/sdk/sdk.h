// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <Include.h>

#include <sdk/interfaces/tier0/IMemAlloc.h>

#include <sdk/interfaces/common/CBufferString.h>
#include <sdk/interfaces/common/CInterlockedInt.h>
#include <sdk/interfaces/common/CThreadMutex.h>
#include <sdk/interfaces/common/CThreadSpinMutex.h>
#include <sdk/interfaces/common/CThreadSpinRWLock.h>
#include <sdk/interfaces/common/CTSList.h>
#include <sdk/interfaces/common/CUtlMap.h>
#include <sdk/interfaces/common/CUtlMemory.h>
#include <sdk/interfaces/common/CUtlMemoryPoolBase.h>
#include <sdk/interfaces/common/CUtlRBTree.h>
#include <sdk/interfaces/common/CUtlString.h>
#include <sdk/interfaces/common/CUtlTSHash.h>
#include <sdk/interfaces/common/CUtlVector.h>

#include <map>
#include <sdk/interfaceregs.h>
#include <sdk/interfaces/client/game/datamap_t.h>
#include <sdk/interfaces/schemasystem/schema.h>
#include <unordered_set>

namespace sdk {
    inline CSchemaSystem* g_schema = nullptr;

    /// Unique identifier for a type in the source2 engine
    struct TypeIdentifier {
        std::string module{};
        std::string name{};

        auto operator<=>(const TypeIdentifier&) const = default;
    };

    /// Stores results of expensive function calls, like those that recurse through classes.
    struct GeneratorCache {
        /// Key is {module,class}
        /// If an entry exists for a class, but its value is @ref std::nullopt, we have already tried finding its alignment but couldn't figure it out.
        std::map<TypeIdentifier, std::optional<int>> class_alignment{};
        /// Key is {module,class}
        std::map<TypeIdentifier, bool> class_has_standard_layout{};
    };

    void GenerateTypeScopeSdk(GeneratorCache& cache, std::string_view module_name, const std::unordered_set<const CSchemaEnumBinding*>& enums,
                              const std::unordered_set<const CSchemaClassBinding*>& classes);
} // namespace sdk

// source2gen - Source2 games SDK generator
// Copyright 2024 neverlosecc
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
