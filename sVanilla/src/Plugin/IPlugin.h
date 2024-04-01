#pragma once
#include <string>
#include <atomic>

#define C_EXPORT_PLUGIN 0

#if C_EXPORT_PLUGIN
struct IPlugin
{
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
