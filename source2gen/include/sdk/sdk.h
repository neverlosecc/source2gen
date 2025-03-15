// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <sdk/interfaces/schemasystem/schema.h>

#include "options.hpp"
#include <filesystem>
#include <map>
#include <sdk/interfaceregs.h>
#include <sdk/interfaces/client/game/datamap_t.h>
#include <sdk/interfaces/schemasystem/schema.h>
#include <string>
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

    // Wrapping the file list in a struct in case we need to return more properties in the future
    struct GeneratorResult {
        /// All generated files
        std::unordered_set<std::filesystem::path> generated_files{};
    };

    GeneratorResult GenerateTypeScopeSdk(source2_gen::Options options, GeneratorCache& cache, std::string_view module_name,
                                         const std::unordered_set<const CSchemaEnumBinding*>& enums,
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
