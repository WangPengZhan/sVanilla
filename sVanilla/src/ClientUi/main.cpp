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

int main(int argc, char* argv[])
{
    Logger::setLogDir(SApplication::appDir().toLocal8Bit().toStdString() + (SApplication::appDir().isEmpty() ? "" : "/"));
    Logger::getInstance();
    DumpColletor::setDumpDir(SApplication::appDir().toStdString() + "/dump");
    sqlite::SqliteDBManager::setDbPath(SApplication::appDir().toStdString() + "/.db");
    biliapi::setCookieDataDir(SApplication::appDir().toLocal8Bit().toStdString());
    network::CurlGlobal curlGlobal;
    DumpColletor::registerDumpHandle();

    Restarter restarter(argc, argv);

    AppInitializer sVanilla;
    sVanilla.initApplicationBefore();

    SApplication application(argc, argv);
    SingleAppHelper singleAppHelper;
    if (singleAppHelper.isHaveInstance())
    {
        return 0;
    }

    sVanilla.init();
    application.init();

    MainWindow maimWindow;
    singleAppHelper.setMainWidget(&maimWindow);
    maimWindow.show();

    return restarter.restartOrExit(SApplication::exec());
}
