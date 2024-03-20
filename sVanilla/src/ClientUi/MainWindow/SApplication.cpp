
#include "SApplication.h"
#include "ClientUi/Config/SingleConfig.h"

SApplication::SApplication(int& argc, char** argv)
    : QApplication(argc, argv)
{
    startServer();
}

SApplication::~SApplication()
{
}

aria2net::AriaServer& SApplication::ariaServer()
{
    return m_ariaServer;
}

void SApplication::startServer()
{
    const auto aria2Config = SingleConfig::instance().getAriaConfig();
    if (!aria2Config.isRemote)
    {
        m_ariaServer.setErrorFunc([] {});
        m_ariaServer.startLocalServerAsync();
    }
}
