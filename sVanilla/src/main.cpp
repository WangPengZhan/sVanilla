#include "Logger/Dump.h"
#include "Logger/Logger.h"
#include "ClientUi/MainWindow/Restarter.h"
#include "ClientUi/MainWindow/SingleAppHelper.h"
#include "ClientUi/MainWindow/SApplication.h"
#include "ClientUi/MainWindow/MainWindow.h"
#include "ClientUi/PlatformInitializer/AppInitializer.h"
#include "Plugin/PluginManager.h"
#include "NetWork/CurlCpp/CurlGlobal.h"
#include "Sqlite/Storage/SqliteDBManager.h"

int main(int argc, char* argv[])
{
    Logger::setLogDir(SApplication::appDir().toStdString() + "/");
    Logger::getInstance();
    sqlite::SqliteDBManager::setDbPath(SApplication::appDir().toStdString() + "/.db");
    network::CurlGlobal curlGlobal;
    DumpColletor::registerDumpHandle();

    Restarter restarter(argc, argv);

    AppInitializer sVanilla;
    sVanilla.initApplicationBefore();

    SApplication application(argc, argv);
    DumpColletor::setDumpDir(SApplication::appDir().toStdString() + "/dump");
    SingleAppHelper singleAppHelper;
    if (singleAppHelper.isHaveInstance())
    {
        return 0;
    }

    sVanilla.init();
    application.init();

    application.pluginManager().loadPlugins();

    MainWindow maimWindow;
    singleAppHelper.setMainWidget(&maimWindow);
    maimWindow.show();

    return restarter.restartOrExit(SApplication::exec());
}
