#pragma once
#include <string>

namespace DumpColletor
{

#ifdef _WIN32
bool initializeCrashpad(const std::wstring& crashHandler, const std::wstring& tempPath);
#else
bool initializeCrashpad(const std::string& crashHandler, const std::string& tempPath);
#endif

}  // namespace DumpColletor
