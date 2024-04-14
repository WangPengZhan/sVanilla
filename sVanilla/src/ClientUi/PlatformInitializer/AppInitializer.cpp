#include <QApplication>
#include <sstream>
#include "MacInitializer.h"
#include "WindowInitializer.h"
#include "LinuxInitializer.h"
#include "AppInitializer.h"

AppInitializer::AppInitializer()
{
#ifdef _WIN32
    m_initializer = std::make_unique<WindowInitializer>();
#elif __linux__
    m_initializer = std::make_unique<LinuxInitializer>();
#elif __APPLE__
    m_initializer = std::make_unique<MacInitializer>();
#endif
}

AppInitializer::~AppInitializer()
{
}

void AppInitializer::initApplicationBefore()
{
    PlatformInitializer::allApplicationBefore();
    m_initializer->initApplicationBefore();
}

void AppInitializer::init()
{
    PlatformInitializer::allInit();
    setHighDpi();
    signalsAndSlots();

    m_initializer->init();
}

void AppInitializer::deinit()
{
    PlatformInitializer::allDeinit();
    m_initializer->deinit();
}

void AppInitializer::setHighDpi()
{
}

void AppInitializer::signalsAndSlots()
{
}
