#include "Logger/Dump.h"
#include "ClientUi/MainWindow/Restarter.h"
#include "ClientUi/MainWindow/SingleAppHelper.h"
#include "ClientUi/MainWindow/SApplication.h"
#include "ClientUi/MainWindow/MainWindow.h"
#include "ClientUi/PlatformInitializer/AppInitializer.h"
#include "Plugin/PluginManager.h"
#include "NetWork/CNetWork.h"
#include "NetWork/CurlCpp/CurlGlobal.h"

int main(int argc, char* argv[])
{
    network::CurlGlobal curlGlobal;
    network::NetWork testNetwork;
    std::string response;
    testNetwork.get("https://www.baidu.com", response);
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

    application.pluginManager().loadPlugins();

    MainWindow maimWindow;
    maimWindow.show();

    return restarter.restartOrExit(SApplication::exec());
}
