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
#pragma endregion Tools

namespace source2_gen {
    void WINAPI main(HMODULE h_module);
}

constexpr std::string_view kPoweredByMessage = {"Powered by github.com/neverlosecc/source2gen"};
constexpr std::string_view kCreatedBySource2genMessage = {"Created using source2gen - github.com/neverlosecc/source2gen"};
