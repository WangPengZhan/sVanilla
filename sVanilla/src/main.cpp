#include <QApplication>

//#include "ClientUi/MainWindow/MainWindow.h"
#include <FramelessHelper/Core/private/framelessconfig_p.h>
#include "ClientUi/BaseWindow.h"
#include "Logger/Dump.h"
#include "Logger/Logger.h"

FRAMELESSHELPER_USE_NAMESPACE
#define CREATE_WINDOW(Name) \
    const auto Name = std::make_unique<BaseWindow>(); \
    Name->setObjectName(FRAMELESSHELPER_STRING_LITERAL(#Name)); \
    Name->waitReady(); \
    Name->show();

int main(int argc, char *argv[])
{
	DumpColletor::registerDumpHandle();
	Logger::GetInstance();
    FramelessHelper::Widgets::initialize();

    const auto application = std::make_unique<QApplication>(argc, argv);

    FramelessHelper::Core::setApplicationOSThemeAware();
    FramelessConfig::instance()->set(Global::Option::EnableBlurBehindWindow);

    CREATE_WINDOW(mainWindow)
    return QApplication::exec();
}


