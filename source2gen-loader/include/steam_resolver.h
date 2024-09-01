// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#pragma once
#include <windows.h>

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

#include "vdf_parser.hpp"

namespace steam_resolver {
    [[nodiscard]] inline std::optional<std::filesystem::path> find_game(const std::size_t app_id) {
        HKEY reg_tree;
        if (RegOpenKeyW(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam", &reg_tree)) {
            return std::nullopt;
        }

        DWORD steam_path_size = MAX_PATH;
        std::wstring steam_path_str;
        steam_path_str.resize(steam_path_size);

        const auto query_stat =
            RegQueryValueExW(reg_tree, L"SteamPath", nullptr, nullptr, reinterpret_cast<LPBYTE>(steam_path_str.data()), &steam_path_size);
        RegCloseKey(reg_tree);

        if (query_stat) {
            return std::nullopt;
        }

        steam_path_str.resize(steam_path_size / sizeof(wchar_t) - 1);

        const auto library_folders_path = std::filesystem::path{steam_path_str} / "steamapps" / "libraryfolders.vdf";
        if (!exists(library_folders_path)) {
            return std::nullopt;
        }

        std::ifstream library_folders_stream(library_folders_path);
        const auto library_folders = tyti::vdf::read(library_folders_stream);

        const auto app_id_str = std::to_string(app_id);

        for (const auto& [index, info] : library_folders.childs) {
            const auto steam_apps_path = std::filesystem::path(info->attribs.at("path")) / "steamapps";
            const auto apps_it = info->childs.at("apps");

            const auto app_it = apps_it->attribs.find(app_id_str);
            if (app_it == std::end(apps_it->attribs)) {
                continue;
            }

            std::filesystem::path manifest_path = steam_apps_path / std::format("appmanifest_{}.acf", app_id_str);
            if (!exists(manifest_path)) [[unlikely]] {
                continue;
            }

            std::ifstream manifest_stream(manifest_path);
            const auto manifest = tyti::vdf::read(manifest_stream);
            const auto install_dir = manifest.attribs.at("installdir");

            return steam_apps_path / "common" / install_dir;
        }

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
