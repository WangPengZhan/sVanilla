
#include "App.h"

int main(int argc, char* argv[])
{
    auto a = std::make_unique<App>(argc, argv);
    return a->exec();
//    DumpColletor::registerDumpHandle();
//    Logger::getInstance();
//
//    QApplication app(argc, argv);
//
//    aria2net::AriaServer ariaServer;
//    ariaServer.setErrorFunc([] {});
//    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
//
//    MainWindow maimWindow;
//    maimWindow.show();
//
//    return app.exec();
}

void test()
{
    if (true)
    {
    }
}