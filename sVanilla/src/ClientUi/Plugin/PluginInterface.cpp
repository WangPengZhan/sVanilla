#include <QDateTime>

#include <algorithm>
#include <chrono>

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

std::vector<std::shared_ptr<plugin::IPlugin>> PluginInterface::orderedPluginsSnapshot() const
{
    auto plugins = m_pluginManager.pluginsSnapshot();
    std::lock_guard lk(m_parseDurationMutex);

    const auto durationFor = [this](const auto& plugin) {
        const auto duration = m_parseDurations.find(plugin->pluginMessage().pluginId);
        return duration == m_parseDurations.end() ? std::chrono::steady_clock::duration::zero() : duration->second;
    };
    std::stable_sort(plugins.begin(), plugins.end(), [&durationFor](const auto& lhs, const auto& rhs) {
        const auto lhsDuration = durationFor(lhs);
        const auto rhsDuration = durationFor(rhs);
        const bool lhsIsSlow = lhsDuration > std::chrono::seconds(1);
        const bool rhsIsSlow = rhsDuration > std::chrono::seconds(1);
        if (lhsIsSlow != rhsIsSlow)
        {
            return !lhsIsSlow;
        }
        return lhsIsSlow && lhsDuration < rhsDuration;
    });

    return plugins;
}

bool PluginInterface::canParseUrl(const std::shared_ptr<plugin::IPlugin>& plugin, const std::string& url)
{
    const auto startedAt = std::chrono::steady_clock::now();
    const bool canParse = plugin->canParseUrl(url);
    const auto duration = std::chrono::steady_clock::now() - startedAt;
    const auto pluginId = plugin->pluginMessage().pluginId;
    {
        std::lock_guard lk(m_parseDurationMutex);
        m_parseDurations[pluginId] = duration;
    }

    MLogI(svanilla::cPluginModule, "canParseUrl completed, pluginId: {}, durationMs: {}, canParse: {}", pluginId,
          std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(), canParse);
    return canParse;
}

std::shared_ptr<plugin::IPlugin> PluginInterface::parseUrl(const std::string& url, std::string& locationUrl, int pluginId)
{
    if (url.empty())
    {
        return {};
    }

    if (pluginId != -1)
    {
        auto plugin = m_pluginManager.getPlugin(pluginId);
        if (!plugin || canParseUrl(plugin, url))
        {
            return plugin;
        }

        if (getUrlLocation(url, locationUrl))
        {
            canParseUrl(plugin, locationUrl);
        }
        return plugin;
    }

    for (auto& plugin : orderedPluginsSnapshot())
    {
        if (canParseUrl(plugin, url))
        {
            return plugin;
        }
    }

    if (getUrlLocation(url, locationUrl))
    {
        for (auto& plugin : orderedPluginsSnapshot())
        {
            if (canParseUrl(plugin, locationUrl))
            {
                return plugin;
            }
        }
    }

    return {};
}

void PluginInterface::setCookiesForPlugins()
{
    for (auto& plugin : m_pluginManager.pluginsSnapshot())
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
