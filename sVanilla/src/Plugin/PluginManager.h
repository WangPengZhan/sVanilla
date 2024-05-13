#pragma once
#include <string>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <set>

#include <nlohmann/json.hpp>

#include "IPlugin.h"
#include "DynamicLibLoader.h"

namespace plugin
{

struct PluginConfig
{
    std::string pluginName;
    std::string version;
    std::string libName;
    bool enabled;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PluginConfig, pluginName, version, libName, enabled)
};

struct PluginInfo
{
    std::string name;
    std::string version;
    int id;
    std::string description;
    std::string libFile;

    bool operator>(const PluginInfo& other) const
    {
        return id > other.id;
    }

    bool operator==(const PluginInfo& other) const
    {
        return id == other.id;
    }

    bool operator<(const PluginInfo& other) const
    {
        return id < other.id;
    }
};

class PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    static std::string pluginDir();
    void loadPlugins();
    void unloadPlugins();

    void addPlugin(const std::string& pluginPath);
    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName);
    void removePlugin(const std::string& pluginName);

    void pluginDirFileAdded();

    std::set<PluginInfo> getPluginsInfo() const;

    void loadConfig();
    void saveConfig() const;

public:
    static const std::string m_dynamicExtension;
    static const std::string m_pluginDir;

private:
    void initPluginPaths();
    void createPluginDir();
    static std::vector<std::string> pluginDirHaving();

private:
    std::unordered_map<std::string, std::shared_ptr<DynamicLibLoader>> m_libHandles;
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> m_plugins;
    std::unordered_set<std::string> m_pluginsPaths;
    mutable std::recursive_mutex m_pluginsMutex;

    static const std::string m_configPath;

    mutable std::atomic_bool m_configChanged;
    std::vector<PluginConfig> m_pluginConfig;
};

}  // namespace plugin

namespace std
{

template <>
class hash<plugin::PluginInfo>
{
    size_t operator()(const plugin::PluginInfo& pluginInfo) const
    {
        return hash<int>()(pluginInfo.id);
    }
};

}  // namespace std
