#pragma once

class ConfigManager
{
public:
    void Init(const std::string& config_folder);

    template <typename T>
    T Get(const std::string& key) const
    {
        try { return config_json_.at(key).get<T>(); }
        catch (const nlohmann::json::exception&) { throw std::runtime_error("Failed to get value for key: " + key); }
    }

private:
    void        Load(const std::string& config_path);
    std::string GetProcessName();

private:
    nlohmann::json config_json_;
};

extern std::unique_ptr<ConfigManager> g_config_manager;
