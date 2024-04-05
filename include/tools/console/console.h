// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include "../../Include.h"

#include <chrono>
#include <iostream>

class DebugConsole {
public:
    DebugConsole() { }

    static constexpr std::uint8_t character_sequence_[] = {'|', '/', '-', '\\'};

    void start(const std::string& title) {
        auto status{false};

        {
            if (this->m_active_)
                throw std::runtime_error("cannot initialize console object more than once!");

            this->sz_title_ += title;
            this->m_active_ = true;
        }

        this->h_console_window_ = GetConsoleWindow();

        this->old_out_ = GetStdHandle(STD_OUTPUT_HANDLE);
        this->old_err_ = GetStdHandle(STD_ERROR_HANDLE);
        this->old_in_ = GetStdHandle(STD_INPUT_HANDLE);

        if (this->h_console_window_ == nullptr) {
            if (!AllocConsole())
                throw std::runtime_error("cannot initialise console object more than once!");

            this->h_console_window_ = GetConsoleWindow();
            if (this->h_console_window_ == nullptr)
                throw std::runtime_error("cannot get console window!");
        }

        {
            FILE* stream;
            freopen_s(&stream, "CONIN$", "r", stdin);
            freopen_s(&stream, "CONOUT$", "w", stdout);
            freopen_s(&stream, "CONOUT$", "w", stderr);
        }

        ///
        {
            std::cout.clear();
            std::clog.clear();
            std::cerr.clear();
            std::cin.clear();
        }

        const auto h_con_out = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                                           FILE_ATTRIBUTE_NORMAL, nullptr);

        const auto h_con_in = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                                          FILE_ATTRIBUTE_NORMAL, nullptr);

        status = SetStdHandle(STD_OUTPUT_HANDLE, h_con_out);
        if (!status) {
            throw std::runtime_error("cannot set std output handle!");
        }

        status = SetStdHandle(STD_ERROR_HANDLE, h_con_out);
        if (!status) {
            throw std::runtime_error("cannot set std error handle!");
        }

        status = SetStdHandle(STD_INPUT_HANDLE, h_con_in);
        if (!status) {
            throw std::runtime_error("cannot set std input handle!");
        }

        ///
        {
            std::wcout.clear();
            std::wclog.clear();
            std::wcerr.clear();
            std::wcin.clear();
        }

        status = SetStdHandle(STD_OUTPUT_HANDLE, h_con_out);
        if (!status) {
            throw std::runtime_error("cannot set std output handle!");
        }

        status = SetStdHandle(STD_ERROR_HANDLE, h_con_out);
        if (!status) {
            throw std::runtime_error("cannot set std error handle!");
        }

        status = SetStdHandle(STD_INPUT_HANDLE, h_con_in);
        if (!status) {
            throw std::runtime_error("cannot set std input handle!");
        }
    }

    void update() {
        if (!this->m_active_)
            return;

        using clock_t = std::chrono::high_resolution_clock;
        const auto time = clock_t::now();
        const auto time_delta = time - m_last_update_time_;
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_delta);

        if (constexpr auto update_time = 150; duration.count() <= update_time)
            return;

        this->sz_title_.at(1) = static_cast<char>(character_sequence_[this->m_seq_index_]);

        if (const auto status = SetConsoleTitleA(sz_title_.c_str()); !status) {
            throw std::runtime_error("cannot set console title!");
        }

        this->m_seq_index_++;
        this->m_seq_index_ %= sizeof(character_sequence_);

        this->m_last_update_time_ = time;
    }

    void stop(void) {
        std::fclose(stdin);
        std::fclose(stdout);
        std::fclose(stderr);

        if (const auto status = FreeConsole(); !status) {
            throw std::runtime_error("cannot free console!");
        }

        if (old_out_)
            SetStdHandle(STD_OUTPUT_HANDLE, old_out_);
        if (old_err_)
            SetStdHandle(STD_ERROR_HANDLE, old_err_);
        if (old_in_)
            SetStdHandle(STD_INPUT_HANDLE, old_in_);

        if (const auto h_console_wnd = GetConsoleWindow(); h_console_wnd != nullptr)
            PostMessageA(h_console_wnd, WM_CLOSE, 0, 0);

        if (this->m_active_)
            this->m_active_ = false;
    }

private:
    HWND h_console_window_ = nullptr;
    bool m_active_ = false;
    std::string sz_title_ = {"[-] "};
    std::size_t m_seq_index_ = 0;
    std::chrono::steady_clock::time_point m_last_update_time_;

    HANDLE old_out_ = nullptr;
    HANDLE old_err_ = nullptr;
    HANDLE old_in_ = nullptr;
};

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
