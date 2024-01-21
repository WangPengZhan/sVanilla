#include <QLocalSocket>
#include <QLocalServer>
#include <QWidget>

#include "SingleAppHelper.h"

QString SingleAppHelper::m_strServerName{"sVanilla"};

SingleAppHelper::SingleAppHelper()
    : m_pLocalServer(nullptr)
    , m_pMainWidget(nullptr)
    , m_bHaveInstance(false)
{
    tryConnectLocaled();
}

SingleAppHelper::~SingleAppHelper()
{
}

void SingleAppHelper::setMainWidget(QWidget* widget)
{
    m_pMainWidget = widget;
}

QWidget* SingleAppHelper::mainWidget() const
{
    return m_pMainWidget;
}

bool SingleAppHelper::isHaveInstance() const
{
    return m_bHaveInstance;
}

void SingleAppHelper::tryConnectLocaled()
{
    m_bHaveInstance = false;
    QLocalSocket socket;
    socket.connectToServer(m_strServerName);

    if (socket.waitForConnected(3000))
    {
        m_bHaveInstance = true;
    }
    else
    {
        m_pLocalServer = new QLocalServer(this);
        connect(m_pLocalServer, &QLocalServer::newConnection, this, &SingleAppHelper::newLocalConnection);
        if (!m_pLocalServer->listen(m_strServerName))
        {
            // 此时监听失败，可能是程序崩溃时, 残留进程服务导致的, 移除之
            if (m_pLocalServer->serverError() == QAbstractSocket::AddressInUseError)
            {
                QLocalServer::removeServer(m_strServerName);
                m_pLocalServer->listen(m_strServerName);
            }
        }
    }
}

void SingleAppHelper::newLocalConnection()
{
    QLocalSocket* pSocket = m_pLocalServer->nextPendingConnection();
    if (!pSocket)
    {
        return;
    }

    delete pSocket;
    if (m_pMainWidget)
    {
        // active running windows
        m_pMainWidget->raise();
        m_pMainWidget->activateWindow();
        m_pMainWidget->setWindowState((m_pMainWidget->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        m_pMainWidget->show();
    }
}
