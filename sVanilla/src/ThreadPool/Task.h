#pragma once

#include <QObject>
#include <any>

class Task
{
public:
    virtual ~Task() = default;
    virtual void Run() = 0;
    virtual void Destroy()
    {
        delete this;
    }
};

class SignalReturnTask : public QObject, public Task
{
    Q_OBJECT
public:
    explicit SignalReturnTask(QObject* parent = nullptr)
        : QObject(parent)
    {
        qRegisterMetaType<std::any>("std::any");
    }
    ~SignalReturnTask() = default;

signals:
    void result(const std::any& ret);
};

template <typename CallBack, typename = std::enable_if_t<std::is_invocable_v<CallBack>>> class TemplateSignalReturnTask : public SignalReturnTask
{
public:
    explicit TemplateSignalReturnTask(CallBack callback, QObject* parent = nullptr)
        : SignalReturnTask(parent)
        , m_callback(std::move(callback))
    {
    }
    ~TemplateSignalReturnTask() = default;

    void Run() override
    {
        emit result(m_callback());
    }

private:
    CallBack m_callback;
};
