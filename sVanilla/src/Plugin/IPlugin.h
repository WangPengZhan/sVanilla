#pragma once
#include <string>
#include <atomic>

namespace Plugin
{

#define C_EXPORT_PLUGIN 1

#if C_EXPORT_PLUGIN

using PluginNameFunc = const char* (*)();
using PluginVersionFunc = const char* (*)();
using PluginDeinitFunc = void(*)();

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
    PluginDeinitFunc pluginDeinit = nullptr;
};

#else
class IPlugin
{
public:
    virtual ~IPlugin() = default;

    virtual const std::string& pluginName() const = 0;
    virtual const std::string& pluginVersion() const = 0;
};
#endif

}  // namespace Plugin
