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

#pragma region FMT
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/os.h>
#include <fmt/printf.h>
#pragma endregion FMT

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
