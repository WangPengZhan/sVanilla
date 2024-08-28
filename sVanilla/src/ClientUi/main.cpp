#include "Logger/Dump.h"
#include "Logger/Logger.h"
#include "MainWindow/Restarter.h"
#include "MainWindow/SingleAppHelper.h"
#include "MainWindow/SApplication.h"
#include "MainWindow/MainWindow.h"
#include "PlatformInitializer/AppInitializer.h"
#include "NetWork/CurlCpp/CurlGlobal.h"
#include "Sqlite/Storage/SqliteDBManager.h"
#include "BiliApi/BilibiliUtils.h"
#include "Util/TimerUtil.h"
#include "ClientLog.h"
#include "const_string.h"
#include "version.h"

#include <QDir>

void startLog()
{
    auto exePath = getModulePath();
    CLINET_LOG_INFO("-----------------------------");
    CLINET_LOG_INFO("start run svanilla, time: {} ", QDateTime::currentDateTime().toString().toStdString());
    CLINET_LOG_INFO("Version: {}", SVNLA_VERSION_STR);
    CLINET_LOG_INFO("GitBranch: {}", GIT_BRANCH);
    CLINET_LOG_INFO("GitHash: {}", GIT_HASH);
    CLINET_LOG_INFO("BuildTime: {}", SVNLA_BUILD_STR);
    CLINET_LOG_INFO("WorkDir: {}", exePath);
    CLINET_LOG_INFO("AppDir: {}", (SApplication::appDir().isEmpty() ? exePath : SApplication::appDir().toStdString()));
    CLINET_LOG_INFO("-----------------------------");
}

int main(int argc, char* argv[])
{
    // MLogI(svanilla::cMainModule, "start run svanilla, time: {}", QDateTime::currentDateTime().toString().toStdString());
    auto exePath = getModulePath();
    QDir::setCurrent(QString::fromStdString(exePath));
    Logger::setLogDir(SApplication::appDir().toLocal8Bit().toStdString() + (SApplication::appDir().isEmpty() ? "" : "/"));
    Logger::getInstance();
    DumpColletor::setDumpDir(SApplication::appDir().toStdString() + (SApplication::appDir().isEmpty() ? "" : "/") + std::string("dump"));
    sqlite::SqliteDBManager::setDbPath(SApplication::appDir().toStdString() + (SApplication::appDir().isEmpty() ? "" : "/") + std::string(".db"));
    biliapi::setCookieDataDir(SApplication::appDir().toLocal8Bit().toStdString());
    network::CurlGlobal curlGlobal;
    DumpColletor::registerDumpHandle();
    startLog();

    CLog_Unique_Timer();
    Restarter restarter(argc, argv);

    AppInitializer sVanilla;
    sVanilla.initApplicationBefore();

    SApplication application(argc, argv);
    SingleAppHelper singleAppHelper;
    if (singleAppHelper.isHaveInstance())
    {
        CLINET_LOG_WARN("==== second exit ====");
        return 0;
    }

    sVanilla.init();
    application.init();

    MainWindow maimWindow;
    singleAppHelper.setMainWidget(&maimWindow);
    maimWindow.show();
    CLog_Unique_Timer_END();

    return restarter.restartOrExit(SApplication::exec());
}
