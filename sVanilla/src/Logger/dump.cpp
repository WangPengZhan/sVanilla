#ifdef _WIN32
#    include <Windows.h>
#    include <DbgHelp.h>
#    include <client/windows/handler/exception_handler.h>
#elif __linux__  //
#    include "client/linux/handler/exception_handler.h"
#elif __APPLE__
#    include "client/mac/handler/exception_handler.h"
#endif

#include "Dump.h"

#include <QApplication>
#include <QDir>

#ifdef _WIN32

int GenerateMiniDump(PEXCEPTION_POINTERS pExceptionPointers)
{
    // 定义函数指针
    typedef BOOL(WINAPI * MiniDumpWriteDumpT)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION,
                                              PMINIDUMP_CALLBACK_INFORMATION);

    // 从 "DbgHelp.dll" 库中获取 "MiniDumpWriteDump" 函数
    MiniDumpWriteDumpT pfnMiniDumpWriteDump = nullptr;
    HMODULE hDbgHelp = LoadLibrary(LPCSTR(L"DbgHelp.dll"));
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
    wchar_t szFileName[MAX_PATH] = {0};
    QString dirPath = QApplication::applicationDirPath() + "/dump/";
    QDir dir;
    if (!dir.exists(dirPath))
    {
        dir.mkpath(dirPath);
    }

    // 创建 dump 文件
    std::wstring appName = QApplication::applicationName().toStdWString();
    SYSTEMTIME stLocalTime;
    ::GetLocalTime(&stLocalTime);
    ::wsprintf(LPSTR(szFileName), LPCSTR(L"%s%s-%04d%02d%02d-%02d%02d%02d.dmp"), dirPath.toStdWString().c_str(), appName.c_str(), stLocalTime.wYear,
               stLocalTime.wMonth, stLocalTime.wDay, stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
    HANDLE hDumpFile = ::CreateFile(LPCSTR(szFileName), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
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
    Q_UNUSED(dump_path)
    Q_UNUSED(id)
    Q_UNUSED(context)
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
    google_breakpad::MinidumpDescriptor descriptor("./dump");
    google_breakpad::ExceptionHandler eh(descriptor, NULL, dumpCallback, NULL, true, -1);
#elif __APPLE__
    QString dumpDir("./dump");
    std::string pathAsStr = dumpDir.toStdString();
    google_breakpad::ExceptionHandler eh(pathAsStr, nullptr, dumpCallback, nullptr, true, nullptr);
#endif
}
