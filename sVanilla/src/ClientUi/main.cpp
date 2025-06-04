#include "Dump/Dump.h"
#include "Logger/Logger.h"
#include "MainWindow/Restarter.h"
#include "MainWindow/SingleAppHelper.h"
#include "MainWindow/SApplication.h"
#include "MainWindow/MainWindow.h"
#include "PlatformInitializer/AppInitializer.h"
#include "NetWork/CurlCpp/CurlGlobal.h"
#include "Sqlite/Storage/SqliteDBManager.h"
#include "Util/TimerUtil.h"
#include "ClientLog.h"
#include "const_string.h"
#include "CommandLine/CommandLine.h"
#include "version.h"

#include <QDir>
#include <QDateTime>
#include <QStandardPaths>

std::string getOsType()
{
#if defined(_WIN32)
    const std::string osType = "Windows";
#elif defined(__APPLE__)
    const std::string osType = "macOS";
#elif defined(__linux__)
    const std::string osType = "Linux";
#else
    const std::string osType = "Unknown";
#endif
    return osType;
}

void startLog()
{
    auto exePath = getModulePath();
    MLogI(svanilla::cMainModule, "-----------------------------");
    MLogI(svanilla::cMainModule, "start run svanilla, time: {} ", QDateTime::currentDateTime().toString().toStdString());
    MLogI(svanilla::cMainModule, "Version: {}", SVNLA_VERSION_STR);
    MLogI(svanilla::cMainModule, "GitBranch: {}", GIT_BRANCH);
    MLogI(svanilla::cMainModule, "GitHash: {}", GIT_HASH);
    MLogI(svanilla::cMainModule, "BuildTime: {}", SVNLA_BUILD_STR);
    MLogI(svanilla::cMainModule, "WorkDir: {}", exePath);
    MLogI(svanilla::cMainModule, "AppDir: {}", (SApplication::appDir().isEmpty() ? exePath : SApplication::appDir().toStdString()));
    MLogI(svanilla::cMainModule, "Operating system: {}", getOsType());
    MLogI(svanilla::cMainModule, "-----------------------------");
}

void myMessageHandler(QtMsgType, const QMessageLogContext&, const QString&)
{
}

int main(int argc, char* argv[])
{
#ifndef _DEBUG
    qInstallMessageHandler(myMessageHandler);
#endif

    auto exePath = getModulePath();
    QString qExePath = QString::fromStdString(exePath);
    QDir::setCurrent(qExePath);
    Logger::setLogDir(SApplication::appDir().toLocal8Bit().toStdString() + (SApplication::appDir().isEmpty() ? "" : "/"));
    Logger::getInstance();
    DumpColletor::setDumpDir(SApplication::appDir().toStdString() + (SApplication::appDir().isEmpty() ? "" : "/") + std::string("dump"));
    sqlite::SqliteDBManager::setDbPath(SApplication::appDir().toStdString() + (SApplication::appDir().isEmpty() ? "" : "/") + std::string(".db"));
    network::CurlGlobal curlGlobal;
    auto crashHandler = QStandardPaths::findExecutable("crashpad_handler", QStringList() << qExePath);
#if defined(_WIN32)
    DumpColletor::initializeCrashpad(crashHandler.toStdWString(),
                                     SApplication::appDir().toStdWString() + (SApplication::appDir().isEmpty() ? L"" : L"/") + std::wstring(L"dump"));
#else
    DumpColletor::initializeCrashpad(crashHandler.toStdString(), DumpColletor::dumpDir);
#endif
    startLog();

    CLog_Unique_TimerK(MainWindow_firstShow);
    Restarter restarter(argc, argv);

    AppInitializer sVanilla;
    sVanilla.initApplicationBefore();

    SApplication application(argc, argv);

    auto commandLine = parseCommandLineOption(argc, argv);
    MLogI(svanilla::cMainModule, "start sVanilla with gui {}", commandLine.showGui ? "enabled" : "disabled");

    std::shared_ptr<SingleAppHelper> singleAppHelper;
    if (commandLine.showGui)
    {
        singleAppHelper = std::make_shared<SingleAppHelper>();
        if (singleAppHelper->isHaveInstance())
        {
            MLogW(svanilla::cMainModule, "sVanilla has opened, please check it");
            MLogI(svanilla::cMainModule, "-----------------------------");
            MLogI(svanilla::cMainModule, "==== second exit ====");
            MLogI(svanilla::cMainModule, "-----------------------------");
            return 0;
        }
    }

    sVanilla.init();
    application.init();

    int exitCode = 0;
    if (commandLine.showGui)
    {
        MainWindow maimWindow;
        singleAppHelper->setMainWidget(&maimWindow);
        maimWindow.show();
        CLog_Unique_TimerK_END(MainWindow_firstShow);

        if (!commandLine.url.empty())
        {
            maimWindow.setUrl(QString::fromStdString(commandLine.url));
        }

        exitCode = restarter.restartOrExit(SApplication::exec());
    }
    else
    {
        attachConsole();
        exitCode = execCommandLine(commandLine, application);
    }

    exitCode = restarter.restartOrExit(exitCode);
    MLogI(svanilla::cMainModule, "-----------------------------");
    MLogI(svanilla::cMainModule, "exit svanilla, time: {} exitCode: {}", QDateTime::currentDateTime().toString().toStdString(), exitCode);
    MLogI(svanilla::cMainModule, "-----------------------------");

    return exitCode;
}
