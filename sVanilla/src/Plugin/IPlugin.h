#pragma once
#include <string>
#include <atomic>

namespace plugin
{

#define C_EXPORT_PLUGIN 1

#if C_EXPORT_PLUGIN

using PluginNameFunc = const char* (*)();
using PluginVersionFunc = const char* (*)();
using PluginIDFunc = int (*)();
using PluginDeinitFunc = void (*)();
using PluginDescriptionFunc = const char* (*)();

struct IPlugin
{
    ~IPlugin()
    {
        if (pluginDeinit)
        {
            pluginDeinit();
        }
    }
    PluginNameFunc pluginName = nullptr;
    PluginVersionFunc pluginVersion = nullptr;
    PluginIDFunc pluginID = nullptr;
    PluginDescriptionFunc pluginDescription = nullptr;
    PluginDeinitFunc pluginDeinit = nullptr;
};

#else
class IPlugin
{
public:
    virtual ~IPlugin() = default;

    virtual const std::string& pluginName() const = 0;
    virtual const std::string& pluginVersion() const = 0;
    virtual int pluginID() const = 0;
    virtual const std::string& pluginDescription() const = 0;
};
#endif

}  // namespace plugin
