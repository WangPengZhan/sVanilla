#pragma once
#include <string>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <compare>

#include <nlohmann/json.hpp>

#include "IPlugin.h"
#include "DynamicLibLoader.h"

namespace plugin
{

struct PluginConfig
{
    std::string name;
    std::string version;
    std::string libName;
    int id;
    std::string description;
    std::string libFile;
    bool enabled{false};
    bool isValid{false};
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PluginConfig, name, version, libName, id, description, libFile, enabled)

    std::strong_ordering operator<=>(const PluginConfig& other) const
    {
        return id <=> other.id;
    }
};

class PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    static std::string pluginDir();
    static void setPluginConfigDir(const std::string& dir);
    void loadPlugins();
    void unloadPlugins();

    void addPlugin(const std::string& pluginPath);
    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName);
    void removePlugin(const std::string& pluginName);

    void pluginDirFileAdded();

    std::set<PluginConfig> getPluginConfig() const;
    void setPluginConfig(std::vector<PluginConfig> configs);

    void loadConfig();
    void saveConfig() const;

public:
    static const std::string m_dynamicExtension;
    static const std::string m_pluginDir;

private:
    void initPluginPaths();
    void createPluginDir();
    static std::vector<std::string> pluginDirHaving();
    static std::string configFilePath();

private:
    std::unordered_map<std::string, std::shared_ptr<DynamicLibLoader>> m_libHandles;
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> m_plugins;
    std::unordered_set<std::string> m_pluginsPaths;
    mutable std::recursive_mutex m_pluginsMutex;

    static const std::string m_configPath;
    static std::string m_dir;

    mutable std::atomic_bool m_configChanged;
    std::vector<PluginConfig> m_pluginConfig;
};

}  // namespace plugin

namespace std
{

template <>
class hash<plugin::PluginConfig>
{
    size_t operator()(const plugin::PluginConfig& pluginInfo) const
    {
        return hash<int>()(pluginInfo.id);
    }
};

}  // namespace std
