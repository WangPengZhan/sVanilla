#include <QApplication>

#include "ClientUi/MainWindow/MainWindow.h"
#include "Logger/Dump.h"
#include "Logger/Logger.h"

int main(int argc, char *argv[])
{
	DumpColletor::registerDumpHandle();
	Logger::GetInstance();
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    
    return a.exec();
}


