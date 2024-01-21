#include "Logger/Dump.h"
#include "ClientUi/Mainwindow/Restarter.h"
#include "ClientUi/Mainwindow/SingleAppHelper.h"
#include "ClientUi/Mainwindow/SApplication.h"
#include "ClientUi/Mainwindow/MainWindow.h"
#include "App.h"

int main(int argc, char* argv[])
{
    DumpColletor::registerDumpHandle();

    SingleAppHelper singleAppHelper;
    if (singleAppHelper.isHaveInstance())
    {
        return 0;
    }

    Restarter restarter(argc, argv);

    SApplication app(argc, argv);

    App sVanilla(argc, argv);
    sVanilla.init();

    MainWindow maimWindow;
    maimWindow.show();

    sVanilla.updateAria2Status();

    return restarter.restartOrExit(app.exec());
}
