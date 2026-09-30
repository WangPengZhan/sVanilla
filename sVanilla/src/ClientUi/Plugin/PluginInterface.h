#pragma once

#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

#include "Plugin/PluginManager.h"

namespace plugin
{
class IPlugin;
}

class PluginInterface
{
public:
    PluginInterface();
    ~PluginInterface();

    plugin::PluginManager& pluginManager();

    std::shared_ptr<plugin::IPlugin> getPlugin(int pluginId);
    std::shared_ptr<plugin::IPlugin> parseUrl(const std::string& url, std::string& locationUrl, int pluginId = -1);

    void setCookiesForPlugins();

    static void setCookiesForPlugin(std::shared_ptr<plugin::IPlugin> plugin);

private:
    std::vector<std::shared_ptr<plugin::IPlugin>> orderedPluginsSnapshot() const;
    bool canParseUrl(const std::shared_ptr<plugin::IPlugin>& plugin, const std::string& url);

    plugin::PluginManager m_pluginManager;

    mutable std::mutex m_parseDurationMutex;
    std::unordered_map<int, std::chrono::steady_clock::duration> m_parseDurations;

    mutable std::recursive_mutex m_pluginsMutex;
    std::unordered_map<int, std::string> m_idNames;
    std::unordered_multimap<std::string, int> m_nameIds;
};
