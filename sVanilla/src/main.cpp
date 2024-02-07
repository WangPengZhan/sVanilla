#include "Logger/Dump.h"
#include "ClientUi/MainWindow/Restarter.h"
#include "ClientUi/MainWindow/SingleAppHelper.h"
#include "ClientUi/MainWindow/SApplication.h"
#include "ClientUi/MainWindow/MainWindow.h"
#include "ClientUi/PlatformInitializer/AppInitializer.h"

int main(int argc, char* argv[])
{
    DumpColletor::registerDumpHandle();

    Restarter restarter(argc, argv);

    SApplication application(argc, argv);
    SingleAppHelper singleAppHelper;
    if (singleAppHelper.isHaveInstance())
    {
        return 0;
    }

    AppInitializer sVanilla;
    sVanilla.init();
    //
    // MainWindow maimWindow;
    // maimWindow.show();

    // sVanilla.updateAria2Version();
    // BiliApi::Get();
    return restarter.restartOrExit(SApplication::exec());
}
