#include <QApplication>

#include "ClientUi/MainWindow/MainWindow.h"
#include "Logger/Dump.h"
#include "Logger/Logger.h"
#include "Aria2Net/AriaServer/AriaServer.h"

int main(int argc, char* argv[])
{
    DumpColletor::registerDumpHandle();
    Logger::getInstance();

    QApplication app(argc, argv);

    aria2net::AriaServer ariaServer;
    ariaServer.setErrorFunc([] {});
    ariaServer.startServerAsync();

    MainWindow maimWindow;
    maimWindow.show();

    return app.exec();
}
