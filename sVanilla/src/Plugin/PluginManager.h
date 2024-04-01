#pragma once
#include <string>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <mutex>

#include "IPlugin.h"

class DynamicLibLoader;
struct PluginConfig
{
    std::string pluginName;
    std::string version;
    std::string libName;
    bool enabled;
};

class PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    void loadPlugins();
    void unloadPlugins();

    
    void addPlugin(const std::string& pluginName);
    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName);

    void loadConfig();
    void saveConfig() const;

private:
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> m_plugins;
    std::unordered_map<std::string, DynamicLibLoader> m_libHandles;
    std::recursive_mutex plugins_mutex_;
    
    static std::string m_pluginDir;
    static std::string m_configPath;

    std::atomic_bool m_configChanged;
    std::vector<PluginConfig> m_pluginConfig;
};
