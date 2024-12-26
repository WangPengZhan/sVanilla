#pragma once
#include <string>

namespace DumpColletor
{
void registerDumpHandle();
void setDumpDir(const std::string& strDumpDir);

static std::string dumpDir = "./dump";
}  // namespace DumpColletor
