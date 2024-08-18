// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include <filesystem>
#include <optional>

namespace steam_resolver {
    [[nodiscard]] inline std::optional<std::filesystem::path> find_game(const std::size_t app_id) {
        return std::nullopt;
    }
} // namespace steam_resolver

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
