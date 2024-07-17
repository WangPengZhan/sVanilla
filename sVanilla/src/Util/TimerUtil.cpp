#include <iomanip>
#include <sstream>
#include <chrono>

#include <iostream>
#include <string>

#ifdef _WIN32
#    include <windows.h>
#else
#    ifdef __APPLE__
#        include <mach-o/dyld.h>
#    else
#        include <unistd.h>
#        include <limits.h>
#        include <cstring>
#    endif
#endif

#include "TimerUtil.h"

std::string convertTimestamp(long long timestamp)
{
    std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::from_time_t(static_cast<std::time_t>(timestamp));
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm* t = std::gmtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(t, "%F");
    return oss.str();
}

std::string formatDuration(const int duration)
{
    std::chrono::seconds sec(duration);
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(sec);
    sec -= std::chrono::duration_cast<std::chrono::seconds>(hours);
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(sec);
    sec -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

    std::ostringstream formattedDuration;
    if (hours.count() > 0)
    {
        formattedDuration << std::setfill('0') << std::setw(2) << hours.count() << ":";
    }
    formattedDuration << std::setfill('0') << std::setw(2) << minutes.count() << ":" << std::setfill('0') << std::setw(2) << sec.count();

    return formattedDuration.str();
}

#ifdef _WIN32
std::string utf16ToUtf8(const std::wstring& utf16Str)
{
    if (utf16Str.empty())
    {
        return std::string();
    }

    int utf8Size = ::WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8Str(utf8Size, 0);
    WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, &utf8Str[0], utf8Size, nullptr, nullptr);
    return utf8Str;
}
#endif

std::string getModulePath()
{
    std::string modulePath;

#ifdef _WIN32
    HMODULE hModule = ::GetModuleHandle(nullptr);
    if (hModule != nullptr)
    {
        wchar_t path[MAX_PATH];
        ::GetModuleFileName(hModule, path, sizeof(path));
        modulePath = utf16ToUtf8(path);
    }
    else
    {
        throw std::runtime_error("Failed to get module handle.");
    }
#elif __APPLE__
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0)
    {
        throw std::runtime_error("Buffer size is too small; it needs to be " + std::to_string(size));
    }
    modulePath = path;
#else
    char path[1024];
    ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
    if (count == -1)
    {
        throw std::runtime_error("Failed to read link /proc/self/exe.");
    }
    path[count] = '\0';
    modulePath = path;
#endif

    return modulePath.substr(0, modulePath.find_last_of("/\\"));
}
