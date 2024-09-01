#include <cassert>
#include <iostream>
#include <ranges>

#include "steam_resolver.h"
#include <argparse/argparse.hpp>

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
#elif defined(DEADLOCK)
        1422450
#else
    #error
#endif
        ;

    constexpr auto kEnvVarName = "PATH";
    constexpr auto kEnvVarPathSep = ";";
    constexpr auto kPlatformDirName = "win64";
    constexpr auto kExecutableName = "source2gen.exe";

    [[nodiscard]] std::optional<std::string> getenv_impl(const std::string& key) {
        const char* val = std::getenv(key.c_str());
        if (val == nullptr) {
            return std::nullopt;
        }
        return std::string(val);
    }

    [[nodiscard]] std::filesystem::path find_second_bin_directory(const std::filesystem::path& game_path) {
        for (const auto& it : std::filesystem::directory_iterator{game_path / "game"}) {
            if (!it.is_directory()) {
                continue;
            }

            const auto path = it.path() / "bin" / kPlatformDirName;
            if (!exists(path)) {
                continue;
            }

            return path;
        }

        throw std::runtime_error("unable to find second bin directory");
    }
} // namespace

int main(const int argc, char* argv[]) try {
    argparse::ArgumentParser program("source2gen-loader");
    program.add_argument("--game_path").help("set the game path manually (ignore the game path resolver)");
    program.parse_args(argc, argv);

    std::cout << std::format("*** loading for game with app_id={:d}", kGameId) << std::endl;
    std::optional<std::filesystem::path> path = std::nullopt;
    if (program.is_used("game_path")) {
        path = program.get<std::string>("game_path");
    }
    if (!path.has_value()) {
        path = steam_resolver::find_game(kGameId);
    }

    if (!path.has_value()) {
        std::cerr << "game directory not found!" << std::endl;
        std::cerr << "please specify it via the command line option like this:" << std::endl;
        std::cerr << std::format("\t* {} --game_path c:\\Some\\Path\\", argv[0]) << std::endl;
        return 1;
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

    /// Uses the same priority as declared
    const auto main_binaries_path = (*path / "game" / "bin" / kPlatformDirName).string();
    const auto second_binaries_path = find_second_bin_directory(*path).string();
    std::array dll_paths = {
        second_binaries_path,
        main_binaries_path,
    };

    /// We are adding our folders to the very start of the env var
    std::string new_path_val;
    for (const auto& [i, dll_path] : std::views::enumerate(dll_paths)) {
        if (i > 0) {
            new_path_val += kEnvVarPathSep;
        }
        new_path_val += dll_path;
    }
    if (const auto old_val = getenv_impl(kEnvVarName); old_val.has_value()) {
        new_path_val += kEnvVarPathSep + *old_val;
    }

    _putenv_s(kEnvVarName, new_path_val.c_str());
    SetDllDirectoryA(main_binaries_path.c_str());

    std::cout << "*** loading source2gen" << std::endl;

    std::string invoke_cmd = kExecutableName;
    for (std::size_t i = 1; i < argc; ++i) {
        invoke_cmd += " ";
        invoke_cmd += argv[i];
    }
    std::system(invoke_cmd.c_str());

    return 0;
} catch (const std::runtime_error& error) {
    std::cerr << error.what() << std::endl;
    return 1;
}
