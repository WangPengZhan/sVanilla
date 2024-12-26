#ifdef _WIN32
#    include <Windows.h>
#    include <DbgHelp.h>
#    include <client/windows/handler/exception_handler.h>
#elif __linux__  //
#    include <client/linux/handler/exception_handler.h>
#elif __APPLE__
#    include <client/mac/handler/exception_handler.h>
#endif

#include "Dump.h"

#ifdef _WIN32

namespace
{

std::wstring stringToWideString(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
    return wstrTo;
}

}  // namespace

int GenerateMiniDump(PEXCEPTION_POINTERS pExceptionPointers)
{
    // 定义函数指针
    typedef BOOL(WINAPI * MiniDumpWriteDumpT)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION,
                                              PMINIDUMP_CALLBACK_INFORMATION);

    // 从 "DbgHelp.dll" 库中获取 "MiniDumpWriteDump" 函数
    MiniDumpWriteDumpT pfnMiniDumpWriteDump = nullptr;
    HMODULE hDbgHelp = LoadLibrary(LPWSTR(L"DbgHelp.dll"));
    if (nullptr == hDbgHelp)
    {
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)::GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
    if (nullptr == pfnMiniDumpWriteDump)
    {
        FreeLibrary(hDbgHelp);
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // 创建 dump 文件夹
    auto dumpDir = stringToWideString(DumpColletor::dumpDir);
    bool success = ::CreateDirectoryW(dumpDir.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
    if (!success)
    {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // 创建 dump 文件
    wchar_t szFileName[MAX_PATH] = {0};
    SYSTEMTIME stLocalTime;
    ::GetLocalTime(&stLocalTime);
    ::wsprintf(LPWSTR(szFileName), LPCWSTR(L"%s/%04d%02d%02d-%02d%02d%02d.dmp"), dumpDir.c_str(), stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
               stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
    HANDLE hDumpFile = ::CreateFile(LPCWSTR(szFileName), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    if (INVALID_HANDLE_VALUE == hDumpFile)
    {
        ::FreeLibrary(hDbgHelp);
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // 写入 dump 文件
    MINIDUMP_EXCEPTION_INFORMATION expParam;
    expParam.ThreadId = ::GetCurrentThreadId();
    expParam.ExceptionPointers = pExceptionPointers;
    expParam.ClientPointers = false;
    pfnMiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hDumpFile, MiniDumpWithDataSegs, (pExceptionPointers ? &expParam : nullptr), nullptr,
                         nullptr);

    // 释放文件
    ::CloseHandle(hDumpFile);
    ::FreeLibrary(hDbgHelp);

    return EXCEPTION_EXECUTE_HANDLER;
}

LONG ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
{
    // 这里做一些异常的过滤或提示
    if (IsDebuggerPresent())
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    return GenerateMiniDump(lpExceptionInfo);
}

bool dumpCallback(const wchar_t* dump_path, const wchar_t* id, void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion, bool succeeded)
{
    return succeeded;
}

#elif __linux__

bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded)
{
    return succeeded;
}
#elif __APPLE__
bool dumpCallback(const char* dump_path, const char* id, void* context, bool succeeded)
{
    (void)(dump_path);
    (void)(id);
    (void)(context);
    return succeeded;
}
#endif

void DumpColletor::registerDumpHandle()
{
#ifdef _WIN32
    ::SetUnhandledExceptionFilter(ExceptionFilter);
    // google_breakpad::ExceptionHandler eh(
    //     L"./dump", NULL, dumpCallback, NULL,
    //     google_breakpad::ExceptionHandler::HANDLER_ALL);
#elif __linux__
    google_breakpad::MinidumpDescriptor descriptor(dumpDir);
    google_breakpad::ExceptionHandler eh(descriptor, NULL, dumpCallback, NULL, true, -1);
#elif __APPLE__
    google_breakpad::ExceptionHandler eh(dumpDir, nullptr, dumpCallback, nullptr, true, nullptr);
#endif
}

void DumpColletor::setDumpDir(const std::string& strDumpDir)
{
    dumpDir = strDumpDir;
}
