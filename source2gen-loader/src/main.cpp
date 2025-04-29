#include <cassert>
#include <print>
#include <ranges>
#include <span>

#include "steam_resolver.h"

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

    struct Config {
        std::optional<std::filesystem::path> game_path = std::nullopt;
    };

    [[nodiscard]] Config parse_arguments(std::vector<std::string>& arguments) {
        using Iter = std::decay_t<decltype(arguments)>::iterator;
        Config result;

        auto show_help = [&]() -> void {
            std::println("Usage: source2gen-loader [--help] [--game-path VAR]\n\n"
                         "More cli options that will be forwarded to source2gen can be seen via:\n"
                         "\tsource2gen --help\n\n"
                         "Optional arguments:\n"
                         "--help              shows help message and exits\n"
                         "--game-path         set the game path manually (ignore the game path resolver)");
            std::exit(0);
        };

        auto read_val = [&](Iter& it, std::string_view command) -> std::optional<std::string> {
            if (*it != command) {
                return std::nullopt;
            }

            const auto next_it = std::next(it);
            if (next_it == arguments.end()) {
                throw std::runtime_error(std::format("{} requires a value", command));
            }

            auto value = *next_it;
            it = arguments.erase(it, std::next(next_it));
            return value;
        };

        for (auto it = arguments.begin(); it != arguments.end();) {
            if (*it == "--help") {
                show_help();
                continue;
            }

            if (auto game_path = read_val(it, "--game-path"); game_path.has_value()) {
                result.game_path = *game_path;
                continue;
            }

            it = std::next(it);
        }

        return result;
    }

} // namespace

int main(const int argc, char* argv[]) try {
    std::span args_raw(argv, argc);
    std::vector<std::string> arguments(args_raw.begin(), args_raw.end());

    const auto config = parse_arguments(arguments);

    std::println("*** loading for game with app_id={:d}", kGameId);
    auto path = config.game_path;
    if (!path.has_value()) {
        path = steam_resolver::find_game(kGameId);
    }

    if (!path.has_value()) {
        std::println(stderr,
                     "game directory not found!\n"
                     "please specify it via the command line option like this:\n"
                     "\t* {} --game-path c:\\Some\\Path\\",
                     argv[0]);
        return 1;
    }

    /// This should never happen
    assert(path.has_value() && "no path found?");

    /// But this could happen
    if (!exists(*path)) {
        std::println(stderr, "specified game path {} does not exist", path->string());
        return 1;
    }

    std::println("*** game path resolved to {}", path->string());
    std::println("*** setting up the env");

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

    /// Erase source2gen-loader.exe
    assert(!arguments.empty());
    arguments.erase(arguments.begin());

    std::string invoke_cmd = kExecutableName;
    invoke_cmd += " " + (arguments | std::views::join_with(' ') | std::ranges::to<std::string>());

    std::println("*** loading source2gen: {}", invoke_cmd);
    std::fflush(stdout);

    std::system(invoke_cmd.c_str());
    return 0;
} catch (const std::runtime_error& error) {
    std::println(stderr, "Fatal error: {}", error.what());
    return 1;
}
