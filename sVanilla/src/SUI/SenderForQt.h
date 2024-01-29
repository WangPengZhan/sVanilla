#pragma once
#include <QObject>
#include <QThread>

#include <functional>
#include <memory>

Q_DECLARE_METATYPE(std::function<void()>)

class ActivateQbject : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    void runTask(const std::function<void()>& f) const;

signals:
    void sendTask(const std::function<void()>& f);
};

class TransceiveThread : public QThread
{
    Q_OBJECT
public:
    TransceiveThread();
    ~TransceiveThread();

    void runTask(const std::function<void()>& f) const;

protected:
    void run() override;

signals:
    void sendTask(const std::function<void()>& f);
};

class SenderForQt
{
public:
    SenderForQt(ActivateQbject* reciver);
    SenderForQt(SenderForQt&&) noexcept = default;

    bool enable() const;

    template<typename Lambda>
    void sendTask(Lambda &&lambda) const
    {
        emit m_sender->sendTask(
            [lambda = std::make_shared<std::remove_reference_t<Lambda>>(std::forward<Lambda>(lambda))]() {
                (*lambda)();
            }
        );
    }
private:
    std::unique_ptr<ActivateQbject> m_sender;
};

class TransceiverForQt
{
public:
    TransceiverForQt();
    ~TransceiverForQt();

    template<typename Lambda>
    void sendTask(Lambda &&lambda) const
    {
        emit m_transceiver->sendTask(
            [lambda = std::make_shared<std::remove_reference_t<Lambda>>(std::forward<Lambda>(lambda))]() {
                (*lambda)();
            }
        );
    }
    
private:
    std::unique_ptr<TransceiveThread> m_transceiver;
};
