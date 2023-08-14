#include <QApplication>

#include "ClientUi/MainWindow/MainWindow.h"
#include "Logger/Dump.h"
#include "Logger/Logger.h"

int main(int argc, char* argv[])
{
    DumpColletor::registerDumpHandle();
    Logger::GetInstance();

    QApplication app(argc, argv);

    MainWindow maimWindow;
    maimWindow.show();

    return app.exec();
}
