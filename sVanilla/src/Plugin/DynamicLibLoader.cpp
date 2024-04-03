#ifdef _WIN32
#    ifndef _WINSOCKAPI_
#        include <Windows.h>
#    endif
#else
#    include <dlfcn.h>
#endif
#include <locale>

#include "DynamicLibLoader.h"
#include "IPlugin.h"

namespace
{
#ifdef _WIN32
#    include <windows.h>

std::wstring stringToWideString(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
    return wstrTo;
}
#endif
}  // namespace
namespace Plugin
{

DynamicLibLoader::DynamicLibLoader(std::string libPath)
    : m_libPath(std::move(libPath))
    , m_libHandle(nullptr)
    , m_loaded(false)
{
}

DynamicLibLoader::~DynamicLibLoader()
{
    unloadLibrary();
}

bool DynamicLibLoader::loadLibrary()
{
    if (m_loaded)
    {
        return true;
    }

    if (m_libPath.empty())
    {
        return false;
    }

    m_libHandle = loadLibrary(m_libPath);
    m_loaded = (m_libHandle != nullptr);
    return m_loaded;
}

std::shared_ptr<IPlugin> DynamicLibLoader::loadPluginSymbol()
{
    std::shared_ptr<IPlugin> res = std::make_shared<IPlugin>();
#if C_EXPORT_PLUGIN
    res->pluginName = static_cast<PluginNameFunc>(loadSymbol(m_libHandle, "pluginName"));
    res->pluginVersion = static_cast<PluginVersionFunc>(loadSymbol(m_libHandle, "pluginVersion"));
#else
    auto pluginFunc = static_cast<IPlugin* (*)()>(loadSymbol(m_libHandle, "plugin"));
    if (pluginFunc)
    {
        res.reset(pluginFunc());
    }
#endif
    return res;
}

void DynamicLibLoader::unloadLibrary()
{
    if (m_libHandle)
    {
        unloadLibrary(m_libHandle);
        m_loaded = false;
        m_libHandle = nullptr;
    }
}

void* DynamicLibLoader::handle() const
{
    return m_libHandle;
}

bool DynamicLibLoader::isLoaded() const
{
    return m_loaded;
}

const std::string& DynamicLibLoader::libraryPath() const
{
    return m_libPath;
}

void DynamicLibLoader::setLibraryPath(const std::string& libPath)
{
    m_libPath = libPath;
}

void* DynamicLibLoader::loadLibrary(const std::string& path)
{
#ifdef _WIN32
    std::wstring wpath = stringToWideString(path);
    return LoadLibrary(LPCWSTR(wpath.c_str()));
#else
    return dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif
}

void* DynamicLibLoader::loadSymbol(void* handle, const std::string& symbolName)
{
    void* symbol = nullptr;
    if (handle)
    {
#ifdef _WIN32
        symbol = GetProcAddress(reinterpret_cast<HMODULE>(handle), symbolName.c_str());
#else
        symbol = dlsym(handle, symbolName.c_str());
#endif

        if (!symbol)
        {
            char* error_message = nullptr;

#ifdef _WIN32
            DWORD error_code = GetLastError();
            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_code,
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&error_message), 0, nullptr);
#else
            error_message = dlerror();
#endif

#ifdef _WIN32
            // Required to release memory allocated by FormatMessageA()
            LocalFree(error_message);
#endif
        }
    }
    return symbol;
}

void DynamicLibLoader::unloadLibrary(void* handle)
{
    if (handle)
    {
#ifdef _WIN32
        FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
        dlclose(handle);
#endif
    }
}

}  // namespace Plugin
