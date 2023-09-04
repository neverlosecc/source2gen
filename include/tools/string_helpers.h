#pragma once
#include <string>

static void replace_all(std::string& text, const std::string& to_remove, const std::string& to_insert) {
    auto pos = text.find(to_remove);
    while (pos != std::string::npos) {
        text.replace(pos, to_remove.length(), to_insert);
        pos = text.find(to_remove, pos);
    }
}
