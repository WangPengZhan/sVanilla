#pragma once
#include <string>
#include <atomic>
#include <memory>

#include "IPlugin.h"

namespace Plugin
{
class DynamicLibLoader
{
public:
    DynamicLibLoader(std::string libPath = "");
    ~DynamicLibLoader();

    bool loadLibrary();
    std::shared_ptr<IPlugin> loadPluginSymbol();
    void unloadLibrary();

    void* handle() const;

    bool isLoaded() const;
    const std::string& libraryPath() const;
    void setLibraryPath(const std::string& libPath);

    static void* loadLibrary(const std::string& path);
    static void* loadSymbol(void* handle, const std::string& symbol);
    static void unloadLibrary(void* handle);

private:
    std::string m_libPath;
    void* m_libHandle;
    std::atomic_bool m_loaded;
};

}  // namespace Plugin
