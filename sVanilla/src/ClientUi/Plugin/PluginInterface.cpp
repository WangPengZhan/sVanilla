#include <QDateTime>

#include "PluginInterface.h"
#include "Storage/StorageManager.h"
#include "Storage/CookiesInfoStorage.h"
#include "PluginCommon/ILogin.h"
#include "ClientLog.h"
#include "const_string.h"

PluginInterface::PluginInterface()
{
}

PluginInterface::~PluginInterface()
{
}

plugin::PluginManager& PluginInterface::pluginManager()
{
    return m_pluginManager;
}

std::shared_ptr<plugin::IPlugin> PluginInterface::getPlugin(int pluginId)
{
    return m_pluginManager.getPlugin(pluginId);
}

std::shared_ptr<plugin::IPlugin> PluginInterface::parseUrl(const std::string& url)
{
    if (url.empty())
    {
        return {};
    }

    for (auto& [_, plugin] : m_pluginManager.plugins())
    {
        if (plugin->canParseUrl(url))
        {
            return plugin;
        }
    }

    return {};
}

void PluginInterface::setCookiesForPlugins()
{
    for (auto& [_, plugin] : m_pluginManager.plugins())
    {
        setCookiesForPlugin(plugin);
    }
}

void PluginInterface::setCookiesForPlugin(std::shared_ptr<plugin::IPlugin> plugin)
{
    auto pluginId = plugin->pluginMessage().pluginId;
    auto cookiesInfoStorage = sqlite::StorageManager::instance().cookiesInfoStorage();
    auto cookiesInfo = cookiesInfoStorage->getCookiesInfo(pluginId);
    if (cookiesInfo.cookie.empty())
    {
        MLogI(svanilla::cPluginModule, "No cookies for plugin: {} , id: {}", plugin->pluginMessage().name, pluginId);
        return;
    }

    QDateTime dt = QDateTime::fromString(cookiesInfo.expires.c_str(), "ddd, dd-MMM-yyyy HH:mm:ss 'GMT'");
    dt.setTimeSpec(Qt::UTC);
    if (dt.isValid() && dt < QDateTime::currentDateTimeUtc())
    {
        plugin->loginer().refreshCookies(cookiesInfo.cookie);
    }
    else
    {
        plugin->loginer().setCookies(cookiesInfo.cookie);
    }
}
