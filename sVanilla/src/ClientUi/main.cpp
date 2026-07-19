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

#include <spdlog/spdlog.h>

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

void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    const auto logger = Logger::get(Logger::defaultName);
    if (!logger)
    {
        return;
    }

    const auto category = context.category ? context.category : "qt";
    const auto text = message.toUtf8().toStdString();
    auto level = spdlog::level::info;
    switch (type)
    {
    case QtDebugMsg:
        level = spdlog::level::debug;
        break;
    case QtInfoMsg:
        level = spdlog::level::info;
        break;
    case QtWarningMsg:
        level = spdlog::level::warn;
        break;
    case QtCriticalMsg:
        level = spdlog::level::err;
        break;
    case QtFatalMsg:
        level = spdlog::level::critical;
        break;
    }

    const spdlog::source_loc source{context.file ? context.file : "", context.line, context.function ? context.function : ""};
    logger->log(source, level, "[Qt:{}] {}", category, text);
    if (type == QtFatalMsg)
    {
        logger->flush();
    }
}

int main(int argc, char* argv[])
{
    network::CurlGlobal::instance();

    auto exePath = getModulePath();
    QString qExePath = QString::fromStdString(exePath);
    QDir::setCurrent(qExePath);

    Logger::setLogDir(SApplication::appDir().toLocal8Bit().toStdString() + (SApplication::appDir().isEmpty() ? "" : "/"));
    Logger::getInstance();

#ifndef _DEBUG
    qInstallMessageHandler(myMessageHandler);
#endif

    sqlite::SqliteDBManager::setDbPath(SApplication::appDir().toStdString() + (SApplication::appDir().isEmpty() ? "" : "/") + std::string(".db"));

    auto crashHandler = QStandardPaths::findExecutable("crashpad_handler", QStringList() << qExePath);
#if defined(_WIN32)
    DumpColletor::initializeCrashpad(crashHandler.toStdWString(),
                                     SApplication::appDir().toStdWString() + (SApplication::appDir().isEmpty() ? L"" : L"/") + std::wstring(L"dump"));
#else
    DumpColletor::initializeCrashpad(crashHandler.toStdString(),
                                     SApplication::appDir().toStdString() + (SApplication::appDir().isEmpty() ? "" : "/") + std::string("dump"));
#endif
    startLog();

    auto commandLine = parseCommandLineOption(argc, argv);
    MLogI(svanilla::cMainModule, "start sVanilla with gui: {}", commandLine.showGui ? "enabled" : "disabled");

    int exitCode = 0;
    if (commandLine.isOnlyPrint())
    {
        attachConsole();
        exitCode = execCommandLine(commandLine);

        MLogI(svanilla::cMainModule, "-----------------------------");
        MLogI(svanilla::cMainModule, "exit svanilla, time: {} exitCode: {}", QDateTime::currentDateTime().toString().toStdString(), exitCode);
        MLogI(svanilla::cMainModule, "-----------------------------");
        return exitCode;
    }

    CLog_Unique_TimerK(MainWindow_firstShow);
    Restarter restarter(argc, argv);

    AppInitializer sVanilla;
    sVanilla.initApplicationBefore();

    SApplication application(argc, argv);

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
        exitCode = restarter.restartOrExit(exitCode);
    }

    MLogI(svanilla::cMainModule, "-----------------------------");
    MLogI(svanilla::cMainModule, "exit svanilla, time: {} exitCode: {}", QDateTime::currentDateTime().toString().toStdString(), exitCode);
    MLogI(svanilla::cMainModule, "-----------------------------");

    return exitCode;
}
