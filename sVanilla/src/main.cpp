#include <QApplication>
#include <QMainWindow>

#include "Logger/dump.h"

int main(int argc, char *argv[])
{
	DumpColletor::registerDumpHandle();

    QApplication a(argc, argv);

    QMainWindow w;
    w.show();
    
    return a.exec();
}


