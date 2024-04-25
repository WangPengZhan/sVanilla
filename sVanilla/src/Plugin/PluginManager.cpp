#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include "PluginManager.h"

namespace Plugin
{

std::string const PluginManager::m_pluginDir = "plugin/";
std::string const PluginManager::m_configPath = "config/plugin_config.json";

#ifdef _WIN32
std::string const PluginManager::m_dynamicExtension = ".dll";
#elif __linux__
std::string const PluginManager::m_dynamicExtension = ".so";
#elif __APPLE__
std::string const PluginManager::m_dynamicExtension = ".dylib";
#endif

PluginManager::PluginManager()
{
    createPluginDir();
    loadConfig();
    initPluginPaths();
}

PluginManager::~PluginManager()
{
    saveConfig();
}

std::string PluginManager::pluginDir()
{
    return m_pluginDir;
}

void PluginManager::loadPlugins()
{
    for (const auto& pluginPath : m_pluginsPaths)
    {
        addPlugin(pluginPath);
    }
}

void PluginManager::unloadPlugins()
{
    std::lock_guard lk(m_pluginsMutex);
    m_plugins.clear();
    m_libHandles.clear();
}

void PluginManager::addPlugin(const std::string& pluginPath)
{
    auto pLoader = std::make_shared<DynamicLibLoader>(pluginPath);
    pLoader->loadLibrary();
    auto plugin = pLoader->loadPluginSymbol();
    if (!plugin)
    {
        return;
    }

    PluginConfig pluginConfig;
    for (const auto& config : m_pluginConfig)
    {
        if (config.pluginName == plugin->pluginName())
        {
            pluginConfig = config;
            break;
        }
    }

    if (pluginConfig.pluginName.empty())
    {
        pluginConfig.enabled = true;
        pluginConfig.pluginName = plugin->pluginName();
        pluginConfig.libName = std::filesystem::path(pluginPath).stem().string();
        pluginConfig.version = plugin->pluginVersion();
        m_configChanged = true;
        m_pluginConfig.emplace_back(pluginConfig);
    }

    if (pluginConfig.enabled)
    {
        std::lock_guard lk(m_pluginsMutex);
        m_libHandles.insert({plugin->pluginName(), pLoader});
        m_plugins.insert({plugin->pluginName(), plugin});
    }
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& pluginName)
{
    std::lock_guard lk(m_pluginsMutex);
    if (m_plugins.find(pluginName) != m_plugins.end())
    {
        return m_plugins.at(pluginName);
    }

    return std::shared_ptr<IPlugin>();
}

void PluginManager::removePlugin(const std::string& pluginName)
{
    std::lock_guard lk(m_pluginsMutex);
    m_plugins.erase(pluginName);
    m_libHandles.erase(pluginName);
}

void PluginManager::pluginDirFileAdded()
{
    auto pluginPaths = pluginDirHaving();
    for (const auto& pluginPath : pluginPaths)
    {
        auto [_, successed] = m_pluginsPaths.insert(pluginPath);
        if (successed)
        {
            addPlugin(pluginPath);
        }
    }
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

void PluginManager::initPluginPaths()
{
    auto pluginPaths = pluginDirHaving();
    std::lock_guard lk(m_pluginsMutex);
    m_pluginsPaths.insert(pluginPaths.begin(), pluginPaths.end());
}

void PluginManager::createPluginDir()
{
    if (!std::filesystem::is_directory(m_pluginDir))
    {
        std::filesystem::create_directory(m_pluginDir);
    }
}

std::vector<std::string> PluginManager::pluginDirHaving()
{
    std::vector<std::string> res;
    std::filesystem::path plugPath(m_pluginDir);

    for (const auto& entry : std::filesystem::directory_iterator(plugPath))
    {
        if (entry.path().extension() == m_dynamicExtension)
        {
            res.emplace_back(std::filesystem::absolute(entry.path()).string());
        }
    }

    return res;
}

}  // namespace  Plugin