#pragma once

#define _WIN32
#include <windows.h>


int GenerateMiniDump(PEXCEPTION_POINTERS pExceptionPointers);
LONG ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo);

#endif

class DumpColletor
{

    static void setDumpPath(const std::string& strPath);
    static void setFilenameFormat(const std::string& strFilenameFormat);
    
    static void registerDumpHandle();

private:
    static std::string m_strDumpPath;
    static std::string m_strFilenameForrmat;
}


