#include "Dump.h"

#include <crashpad/client/crashpad_client.h>
#include <crashpad/client/crashpad_info.h>
#include <crashpad/client/crash_report_database.h>
#include <crashpad/client/settings.h>

#include <map>
#include <memory>

#include "version.h"
#include "Config/GlobalData.h"

namespace DumpColletor
{

void setDumpDir(const std::string& strDumpDir)
{
    dumpDir = strDumpDir;
}

#ifdef _WIN32
bool initializeCrashpad(const std::wstring& crashHandler, const std::wstring& tempPath)
#else
bool initializeCrashpad(const std::string& crashHandler, const std::string& tempPath)
#endif
{
    using namespace crashpad;

    base::FilePath handler(crashHandler);
    base::FilePath reportsDir(tempPath);
    base::FilePath metricsDir(tempPath);

    std::map<std::string, std::string> annotations;
    annotations["format"] = "minidump";
    annotations["database"] = svanilla::softwareName;
    annotations["product"] = svanilla::softwareName;
    annotations["version"] = SVNLA_VERSION_STR;

    // Disable crashpad rate limiting so that all crashes have dmp files
    std::vector<std::string> arguments;
    arguments.push_back("--no-rate-limit");

    // Initialize crashpad database
    std::unique_ptr<CrashReportDatabase> database = CrashReportDatabase::Initialize(reportsDir);
    if (!database)
    {
        return false;
    }

    // Enable automated crash uploads
    Settings* settings = database->GetSettings();
    if (!settings)
    {
        return false;
    }
    settings->SetUploadsEnabled(true);

    // Start crash handler
    CrashpadClient* client = new CrashpadClient();
#ifdef __liunx__
    bool status = client->StartHandler(handler, reportsDir, metricsDir, "", annotations, arguments, true, false, {});
#else
    bool status = client->StartHandler(handler, reportsDir, metricsDir, "", annotations, arguments, true, true, {});
#endif
    return status;
}

}  // namespace DumpColletor
