#include <fstream>
#include <iostream>

#include "PluginManager.h"
#include "PluginLog.h"

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

class EmptyLoginApi : public AbstractLoginApi
{
public:
    EmptyLoginApi() = default;
    ~EmptyLoginApi() = default;

    bool supportLogin() const override
    {
        return false;
    }

    bool isLogin() const override
    {
        return false;
    }
    std::string cookies() const override
    {
        return {};
    }

    void setCookies(std::string cookies) override
    {
    }

    bool refreshCookies(std::string cookies) override
    {
        return {};
    }
    bool logout() override
    {
        return false;
    }

    UserInfo getUserInfo(std::string dir) override
    {
        return {};
    }

    std::vector<adapter::BaseVideoView> history() override
    {
        return {};
    }

    int pluginId() const override
    {
        return 0;
    }

    // thread-safe
    LoginStatus getLoginStatus() override
    {
        return AbstractLoginApi::Unknow;
    }
    bool getScanContext(std::string& content) override
    {
        return false;
    }
    void loginSuccess() override
    {
    }

    // resource
    const LoginResource& allResources() const override
    {
        static LoginResource empty;
        return empty;
    }

    const std::vector<uint8_t>& resource(ResourceIndex index) const override
    {
        static std::vector<uint8_t> empty;
        return empty;
    }
};

PluginProxy::PluginProxy(std::shared_ptr<IPlugin> realPlugin)
    : m_realPlugin(std::move(realPlugin))
{
}

const PluginMessage& PluginProxy::pluginMessage() const
{
    try
    {
        PLUGIN_LOG_INFO("call pluginMessage, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
        return m_realPlugin->pluginMessage();
    }
    catch (const std::exception& e)
    {
        PLUGIN_LOG_WARN("pluginMessage throw exception, msg: {}, pluginId: {}", e.what(), m_realPlugin->pluginMessage().pluginId);
    }
    catch (...)
    {
        PLUGIN_LOG_WARN("pluginMessage throw unknow exception, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
    }

    static PluginMessage empty;
    return empty;
}

const std::vector<uint8_t>& PluginProxy::websiteIcon()
{
    try
    {
        PLUGIN_LOG_INFO("call websiteIcon, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
        return m_realPlugin->websiteIcon();
    }
    catch (const std::exception& e)
    {
        PLUGIN_LOG_WARN("websiteIcon throw exception, msg: {}, pluginId: {}", e.what(), m_realPlugin->pluginMessage().pluginId);
    }
    catch (...)
    {
        PLUGIN_LOG_WARN("websiteIcon throw unknow exception, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
    }

    static std::vector<uint8_t> empty;
    return empty;
}

bool PluginProxy::canParseUrl(const std::string& url)
{
    try
    {
        PLUGIN_LOG_INFO("call canParseUrl, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
        return m_realPlugin->canParseUrl(url);
    }
    catch (const std::exception& e)
    {
        PLUGIN_LOG_WARN("canParseUrl throw exception, msg: {}, pluginId: {}", e.what(), m_realPlugin->pluginMessage().pluginId);
    }
    catch (...)
    {
        PLUGIN_LOG_WARN("canParseUrl throw unknow exception, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
    }
    return false;
}

adapter::VideoView PluginProxy::getVideoView(const std::string& url)
{
    try
    {
        PLUGIN_LOG_INFO("call getVideoView, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
        return m_realPlugin->getVideoView(url);
    }
    catch (const std::exception& e)
    {
        PLUGIN_LOG_WARN("getVideoView throw exception, msg: {}, pluginId: {}", e.what(), m_realPlugin->pluginMessage().pluginId);
    }
    catch (...)
    {
        PLUGIN_LOG_WARN("getVideoView throw unknow exception, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
    }
    return {};
}

std::shared_ptr<download::FileDownloader> PluginProxy::getDownloader(const VideoInfoFull& videoInfo)
{
    try
    {
        PLUGIN_LOG_INFO("call getDownloader, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
        return m_realPlugin->getDownloader(videoInfo);
    }
    catch (const std::exception& e)
    {
        PLUGIN_LOG_WARN("getDownloader throw exception, msg: {}, pluginId: {}", e.what(), m_realPlugin->pluginMessage().pluginId);
    }
    catch (...)
    {
        PLUGIN_LOG_WARN("getDownloader throw unknow exception, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
    }
    return {};
}

LoginProxy PluginProxy::loginer()
{
    try
    {
        PLUGIN_LOG_INFO("call loginer, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
        return m_realPlugin->loginer();
    }
    catch (const std::exception& e)
    {
        PLUGIN_LOG_WARN("loginer throw exception, msg: {}, pluginId: {}", e.what(), m_realPlugin->pluginMessage().pluginId);
    }
    catch (...)
    {
        PLUGIN_LOG_WARN("loginer throw unknow exception, pluginId: {}", m_realPlugin->pluginMessage().pluginId);
    }

    static EmptyLoginApi empty;

    return LoginProxy(empty);
}

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

const std::string& PluginManager::configDir()
{
    return m_dir;
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
    PLUGIN_LOG_INFO("add Plugin: {}", pluginPath);
    auto pLoader = std::make_shared<DynamicLibLoader>(pluginPath);
    pLoader->loadLibrary();
    auto plugin = pLoader->loadPluginSymbol();
    if (!plugin)
    {
        PLUGIN_LOG_WARN("load plugin failed, path: {}", pluginPath);
        return;
    }

    plugin = std::make_shared<PluginProxy>(plugin);

    PluginConfig pluginConfig;
    for (const auto& config : m_pluginConfig)
    {
        if (config.name == plugin->pluginMessage().name)
        {
            pluginConfig = config;
            break;
        }
    }

    const auto& pluginMessage = plugin->pluginMessage();
    if (pluginConfig.name.empty())
    {
        pluginConfig.enabled = true;
        pluginConfig.name = pluginMessage.name;
        pluginConfig.libName = std::filesystem::path(pluginPath).stem().string();
        pluginConfig.libFile = std::filesystem::path(pluginPath).filename().string();
        pluginConfig.version = pluginMessage.version;
        pluginConfig.id = pluginMessage.pluginId;
        pluginConfig.description = pluginMessage.description;
        m_configChanged = true;
        m_pluginConfig.emplace_back(pluginConfig);
    }
    pluginConfig.isValid = true;

    if (pluginConfig.enabled)
    {
        std::lock_guard lk(m_pluginsMutex);
        m_libHandles.insert({pluginMessage.pluginId, pLoader});
        m_plugins.insert({pluginMessage.pluginId, plugin});
    }
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(int pluginId)
{
    std::lock_guard lk(m_pluginsMutex);
    if (m_plugins.find(pluginId) != m_plugins.end())
    {
        return m_plugins.at(pluginId);
    }

    return std::shared_ptr<IPlugin>();
}

void PluginManager::removePlugin(int pluginId)
{
    std::lock_guard lk(m_pluginsMutex);
    m_plugins.erase(pluginId);
    m_libHandles.erase(pluginId);
}

const std::unordered_map<int, std::shared_ptr<IPlugin>>& PluginManager::plugins() const
{
    return m_plugins;
}

void PluginManager::pluginDirFileAdded()
{
    auto pluginPaths = pluginDirHaving();
    for (const auto& pluginPath : pluginPaths)
    {
        auto [_, succeeded] = m_pluginsPaths.insert(pluginPath);
        if (succeeded)
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

    PLUGIN_LOG_INFO("plugin dir: {}", plugPath.string());
    for (const auto& entry : std::filesystem::directory_iterator(plugPath))
    {
        PLUGIN_LOG_INFO("entry dir: {}", entry.path().string());
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
