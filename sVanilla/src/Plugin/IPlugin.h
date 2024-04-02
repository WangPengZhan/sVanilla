#pragma once
#include <string>
#include <atomic>

namespace Plugin
{

#define C_EXPORT_PLUGIN 0

#if C_EXPORT_PLUGIN

typedef const char* (PluginNameFunc)();
typedef const char* (PluginVersionFunc)();

struct IPlugin
{
    PluginNameFunc pluginName;
    PluginVersionFunc pluginFunc;
};

#else
class IPlugin
{
public:
    virtual ~IPlugin() = default;

    virtual const std::string pluginName() const = 0;
    virtual const std::string& pluginVersion() const = 0;
};
#endif

} // namespace Plugin
