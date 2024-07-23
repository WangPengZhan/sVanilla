#include <fstream>
#include <iostream>

#include "PluginManager.h"

namespace plugin
{

std::string const PluginManager::m_pluginDir = "plugin/";
std::string const PluginManager::m_configPath = "config/plugin_config.json";
std::string PluginManager::m_dir;

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

void PluginManager::setPluginConfigDir(const std::string& dir)
{
    m_dir = dir;
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
        if (config.name == plugin->pluginName())
        {
            pluginConfig = config;
            break;
        }
    }

    if (pluginConfig.name.empty())
    {
        pluginConfig.enabled = true;
        pluginConfig.name = plugin->pluginName();
        pluginConfig.libName = std::filesystem::path(pluginPath).stem().string();
        pluginConfig.libFile = std::filesystem::path(pluginPath).filename().string();
        pluginConfig.version = plugin->pluginVersion();
        pluginConfig.id = plugin->pluginID();
        pluginConfig.description = plugin->pluginDescription();
        m_configChanged = true;
        m_pluginConfig.emplace_back(pluginConfig);
    }
    pluginConfig.isValid = true;

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

std::set<PluginConfig> PluginManager::getPluginConfig() const
{
    std::lock_guard lk(m_pluginsMutex);
    return std::set<PluginConfig>(m_pluginConfig.begin(), m_pluginConfig.end());
}

void PluginManager::setPluginConfig(std::vector<PluginConfig> configs)
{
    std::lock_guard lk(m_pluginsMutex);
    m_pluginConfig = std::move(configs);
}

void PluginManager::loadConfig()
{
    try
    {
        std::ifstream f(configFilePath());
        nlohmann::json data = nlohmann::json::parse(f);
        m_pluginConfig = data;
    }
    catch (const std::exception& e)
    {
        // ...
    }

    if (m_pluginConfig.empty())
    {
        static PluginConfig biliConfig{"bili", "1.0.0", "bili", 1, "bili plugin for https://www.bilibili.com/", "bilibili.dll", true};
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

    std::ofstream o(configFilePath());
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

std::string PluginManager::configFilePath()
{
    if (m_dir.empty())
    {
        return m_configPath;
    }
    else
    {
        return (m_dir.back() == '/' || m_dir.back() == '\\') ? m_dir + m_configPath : m_dir + "/" + m_configPath;
    }

    return {};
}

}  // namespace plugin
