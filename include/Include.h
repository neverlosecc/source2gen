// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <array>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <windows.h>

using namespace std::chrono_literals;
using namespace std::this_thread;

#pragma region Format
#include <format>
#include <iostream>
#pragma endregion Format

#pragma region Source Engine 2
#include <SDK/SDK.h>
#pragma endregion Source Engine 2

#pragma region Tools
#include "tools/codegen.h"
#include "tools/field_parser.h"
#include "tools/fnv.h"
#include "tools/util.h"
#pragma endregion Tools

namespace source2_gen {
    void main(HMODULE h_module);
}

constexpr std::string_view kConsoleTitleMessage = {"source2gen :: github.com/neverlosecc/source2gen"};
constexpr std::string_view kPoweredByMessage = {"Powered by github.com/neverlosecc/source2gen"};
constexpr std::string_view kCreatedBySource2genMessage = {"Created using source2gen - github.com/neverlosecc/source2gen"};

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
