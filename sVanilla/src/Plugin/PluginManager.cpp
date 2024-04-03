#include <fstream>

#include <nlohmann/json.hpp>

#include "PluginManager.h"

namespace Plugin
{

std::string PluginManager::m_pluginDir = "plugin/";
std::string PluginManager::m_configPath = "config/plugin_config.json";

PluginManager::PluginManager()
{
    loadConfig();
}

PluginManager::~PluginManager()
{
    saveConfig();
}

void PluginManager::loadPlugins()
{
    std::filesystem::path plugPath(m_pluginDir);
    std::string allPath = std::filesystem::absolute(plugPath).string();
    auto pLoader = std::make_shared<DynamicLibLoader>(allPath + "TemplatePlugin.dll");
    pLoader->loadLibrary();
    auto plugin = pLoader->loadPluginSymbol();
    if (plugin)
    {
        m_libHandles.insert({"TemplatePlugin", pLoader});
        m_plugins.insert({"TemplatePlugin", plugin});
    }
}

void PluginManager::addPlugin(const std::string& pluginName)
{
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& pluginName)
{
    if (m_plugins.find(pluginName) != m_plugins.end())
    {
        return m_plugins.at(pluginName);
    }

    return std::shared_ptr<IPlugin>();
}

void PluginManager::loadConfig()
{
    try
    {
        std::ifstream f("example.json");
        nlohmann::json data = nlohmann::json::parse(f);
        m_pluginConfig = data;
    }
    catch (const std::exception& e)
    {
        // ...
    }

    if (m_pluginConfig.empty())
    {
        static PluginConfig biliConfig{"bili", "1.0.0", "bili", true};
        m_pluginConfig.emplace_back(biliConfig);
        m_configChanged = true;
        saveConfig();
    }
}

void PluginManager::saveConfig() const
{
    if (!m_configChanged)
    {
        return;
    }

    std::ofstream o("config/plugin_config.json");
    nlohmann::json json = m_pluginConfig;
    o << json.dump(4);
    m_configChanged = false;
}

}  // namespace  Plugin
