#include <QDateTime>

#include "PluginInterface.h"
#include "Storage/StorageManager.h"
#include "Storage/CookiesInfoStorage.h"
#include "PluginCommon/ILogin.h"
#include "NetWork/CurlCpp/CurlHeader.h"
#include "NetWork/CurlCpp/CurlOption.h"
#include "NetWork/CurlCpp/CurlWriter.h"
#include "NetWork/CurlCpp/CurlEasy.h"
#include "NetWork/CNetWork.h"
#include "NetWork/CurlCpp/CurlResponseWrapper.h"
#include "NetWork/LocationUrlResponseWrapper.h"
#include "ClientLog.h"
#include "const_string.h"

namespace
{

bool getUrlLocation(const std::string& url, std::string& location)
{
    if (url.empty())
    {
        return false;
    }

    network::LocationUrl response;
    network::CurlEasy easy;
    network::CurlResponseWrapper writer(response);

    network::CurlHeader headers;
    std::string userAgent = std::string("User-Agent: ") + network::chrome;
    headers.add(userAgent);
    headers.add(network::accept_language);
    headers.add(network::accept_encoding);

    network::NetWork::CurlOptions options;
    constexpr time_t timeoutSecond = 5000;
    auto timeout = std::make_shared<network::TimeOut>(timeoutSecond);
    options.insert({timeout->getOption(), timeout});
    auto acceptEncoding = std::make_shared<network::AcceptEncoding>("gzip");
    options.insert({acceptEncoding->getOption(), acceptEncoding});
    auto sslVerifyHost = std::make_shared<network::SSLVerifyHost>(false);
    options.insert({sslVerifyHost->getOption(), sslVerifyHost});
    auto sslVerifyPeer = std::make_shared<network::SSLVerifyPeer>(false);
    options.insert({sslVerifyPeer->getOption(), sslVerifyPeer});
    auto verbose = std::make_shared<network::Verbose>(false);
    options.insert({verbose->getOption(), verbose});

    curl_easy_setopt(easy.handle(), CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(easy.handle(), CURLOPT_HTTPHEADER, headers.get());
    for (const auto& option : options)
    {
        option.second->setToCurl(easy.handle());
    }
    writer.setToCurl(easy);

    easy.perform();

    writer.readAfter(easy);

    location = response.locationUrl;
    if (location.empty())
    {
        MLogE(svanilla::cPluginModule, "Failed to get location from URL: {}", url);
        return false;
    }

    return true;
}
}  // namespace

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

std::shared_ptr<plugin::IPlugin> PluginInterface::parseUrl(const std::string& url, std::string& locationUrl)
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

    if (getUrlLocation(url, locationUrl))
    {
        for (auto& [_, plugin] : m_pluginManager.plugins())
        {
            if (plugin->canParseUrl(locationUrl))
            {
                return plugin;
            }
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
