#pragma once

namespace util { 
	inline std::string_view PrettifyNum(int num) {
        static const auto fn = reinterpret_cast<const char* (*)(int)>(GetProcAddress(GetModuleHandleA("tier0.dll"), "V_PrettifyNum"));

        if (fn) {
            std::string_view res = fn(num);
            if (!res.empty()) {
                return res;
            }
        }

        return std::to_string(num);
    }
};