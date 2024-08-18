#include <iostream>

#include "steam_resolver/steam_resolver.h"

#include <shared/loader/loader.h>
#include <shared/platform.h>

namespace {
    constexpr std::size_t kGameId =
#if defined(CS2)
        730
#elif defined(DOTA2)
        570
#elif defined(SBOX)
        590830
#elif defined(ARTIFACT2)
        2082270
#elif defined(ARTIFACT1)
        583950
#elif defined(UNDERLORDS)
        1046930
#elif defined(DESKJOB)
        1902490
#elif defined(HL_ALYX)
        546560
#elif defined(THE_LAB_ROBOT_REPAIR)
        450390
#else
    #error
#endif
        ;

    constexpr auto kEnvVarName = IF_LINUX("LD_LIBRARY_PATH") IF_WINDOWS("PATH");
    constexpr auto kEnvVarPathSep = IF_LINUX(":") IF_WINDOWS(";");

    [[nodiscard]] std::optional<std::string> getenv_impl(const std::string& key) {
        const char* val = std::getenv(key.c_str());
        if (val == nullptr) {
            return std::nullopt;
        }
        return std::string(val);
    }
} // namespace

int main(const int argc, char* argv[])  try {
    std::cout << std::format("*** loading for game with app_id={:d}", kGameId) << std::endl;

    auto path = steam_resolver::find_game(kGameId);
    if (!path.has_value() || argc >= 2) {
        if (argc < 2) {
            std::cerr << "game directory not found!" << std::endl;
            std::cerr << "please specify it via the command line option like this:" << std::endl;
            std::cerr << std::format("\t* {} game_path", argv[0]) << std::endl;
            return 1;
        }

        path = argv[1];
    }

    /// This should never happen
    assert(path.has_value() && "no path found?");

    /// But this could happen
    if (!exists(*path)) {
        std::cerr << std::format("specified game path {} does not exist", path->string()) << std::endl;
        return 1;
    }

    std::cout << std::format("*** game path resolved to {}", path->string()) << std::endl;

    std::cout << "*** setting up the env" << std::endl;

    std::string new_path_val = getenv_impl(kEnvVarName).value_or("");
    new_path_val += kEnvVarPathSep + (*path / "game" / "bin" / "win64").string();

    std::cout << std::format("*** new env var: {}", new_path_val) << std::endl;
#if TARGET_OS == WINDOWS
    _putenv_s(kEnvVarName, new_path_val.c_str());
#else
    setenv(kEnvVarName, new_path_val.c_str(), 1);
#endif

    std::cout << "*** loading source2gen" << std::endl;

    if (auto expr = loader::load_module("source2gen"); !expr.has_value()) {
        throw std::runtime_error(expr.error().as_string().data());
    }

    return 0;
} catch (const std::runtime_error& error) {
    std::cerr << error.what() << std::endl;
    return 1;
}
