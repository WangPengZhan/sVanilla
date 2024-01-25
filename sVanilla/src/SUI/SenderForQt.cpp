#include "SenderForQt.h"

void ActivateQbject::runTask(const std::function<void()>& f) const
{
	f();
}

TransceiveThread::TransceiveThread()
{
    moveToThread(this);
    connect(this, &TransceiveThread::sendTask, this, &TransceiveThread::runTask, Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));
    start();
}

TransceiveThread::~TransceiveThread()
{
    if (isRunning())
    {
        quit();
        wait();
    }
}

void TransceiveThread::runTask(const std::function<void()>& f) const
{
    f();
}

void TransceiveThread::run()
{
    // for debug
    exec();
}

SenderForQt::SenderForQt(ActivateQbject* reciver)
    : m_sender(reciver ? std::make_unique<ActivateQbject>() : nullptr)
{
    assert(nullptr != reciver);
    if (reciver)
    {
        QObject::connect(m_sender.get(), &ActivateQbject::sendTask, reciver, &ActivateQbject::runTask, 
                         Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));
    }
}

bool SenderForQt::enable() const
{
    return !!m_sender;
}

TransceiverForQt::TransceiverForQt()
    : m_transceiver(std::make_unique<TransceiveThread>())
{
}
