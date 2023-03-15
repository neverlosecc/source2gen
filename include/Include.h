#ifndef INCLUDE_H__
#define INCLUDE_H__

#include <windows.h>
#include <chrono>
#include <thread>
#include <filesystem>
#include <cassert>
#include <array>
#include <unordered_map>

using namespace std::chrono_literals;
using namespace std::this_thread;

#pragma region FMT
#include <fmt/core.h>
#include <fmt/printf.h>
#include <fmt/os.h>
#include <fmt/color.h>
#pragma endregion FMT

#pragma region Source Engine 2
#include <SDK/SDK.h>
#pragma endregion Source Engine 2

#pragma region Tools
#include "tools/fnv.h"
#pragma endregion Tools

namespace source2_gen
{
    void WINAPI main(HMODULE h_module);
}

#endif
