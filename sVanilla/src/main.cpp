#include "Logger/Dump.h"
#include "ClientUi/MainWindow/Restarter.h"
#include "ClientUi/MainWindow/SingleAppHelper.h"
#include "ClientUi/MainWindow/SApplication.h"
#include "ClientUi/MainWindow/MainWindow.h"
#include "App.h"

int main(int argc, char* argv[])
{
    DumpColletor::registerDumpHandle();

    if (const SingleAppHelper singleAppHelper; singleAppHelper.isHaveInstance())
    {
        return 0;
    }

    Restarter restarter(argc, argv);

    SApplication application(argc, argv);

    App sVanilla;
    sVanilla.init();
    //
    // MainWindow maimWindow;
    // maimWindow.show();

    sVanilla.updateAria2Status();

    return restarter.restartOrExit(SApplication::exec());
}
