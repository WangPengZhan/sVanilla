#pragma once

#include <string>
#include <unordered_map>
#include <memory>

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

    std::shared_ptr<plugin::IPlugin> parseUrl(const std::string& url);

private:
    plugin::PluginManager m_pluginManager;

    mutable std::recursive_mutex m_pluginsMutex;
    std::unordered_map<int, std::string> m_idNames;
    std::unordered_multimap<std::string, int> m_nameIds;
};
