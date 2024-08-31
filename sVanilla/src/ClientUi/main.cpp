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
#include <QDateTime>

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
    MLogI(svanilla::cMainModule, "-----------------------------");
}

int main(int argc, char* argv[])
{
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

    CLog_Unique_TimerK(MainWindow_firstShow);
    Restarter restarter(argc, argv);

    AppInitializer sVanilla;
    sVanilla.initApplicationBefore();

    SApplication application(argc, argv);
    SingleAppHelper singleAppHelper;
    if (singleAppHelper.isHaveInstance())
    {
        MLogW(svanilla::cMainModule, "sVanilla has opened, please check it");
        MLogW(svanilla::cMainModule, "==== second exit ====");
        return 0;
    }

    sVanilla.init();
    application.init();

    MainWindow maimWindow;
    singleAppHelper.setMainWidget(&maimWindow);
    maimWindow.show();
    CLog_Unique_TimerK_END(MainWindow_firstShow);

    return restarter.restartOrExit(SApplication::exec());
}
