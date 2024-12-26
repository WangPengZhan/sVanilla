#include "PluginInterface.h"

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

    return m_pluginManager.getPlugin(1);
}
