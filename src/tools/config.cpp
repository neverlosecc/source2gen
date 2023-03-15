#include <Include.h>
#include <tools/config.h>

std::unique_ptr<ConfigManager> g_config_manager = std::make_unique<ConfigManager>();

void ConfigManager::Init(const std::string& config_folder)
{
    const std::string process_name = GetProcessName();
    const std::string config_path  = config_folder + "\\" + process_name + ".json";
    Load(config_path);
}

void ConfigManager::Load(const std::string& config_path)
{
    TCHAR exe_path[MAX_PATH];
    GetModuleFileName(nullptr, exe_path, MAX_PATH);

    const std::filesystem::path path = std::filesystem::path(exe_path).parent_path() / config_path;
    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error("Failed to find config file: " + path.string());
    }

    std::ifstream config_file;
    config_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        config_file.open(path, std::ios::in);
    }
    catch (const std::ifstream::failure& e)
    {
        throw std::runtime_error("Failed to open config file: " + path.string() + " - Error: " + e.what());
    }

    try
    {
        config_file >> config_json_;
    }
    catch (const nlohmann::json::exception& e)
    {
        throw std::runtime_error("Failed to parse config file: " + path.string() + " - Error: " + e.what());
    }
}

std::string ConfigManager::GetProcessName()
{
    std::array<char, MAX_PATH> buffer{};
    const DWORD                length = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));

    if (length == 0 || length == buffer.size()) { throw std::runtime_error("Failed to get process name"); }

    const std::string           process_path(buffer.data(), length);
    const std::filesystem::path path(process_path);
    std::string                 process_name = path.stem().string();

    return process_name;
}
