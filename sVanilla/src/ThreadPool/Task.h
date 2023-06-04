#pragma once

#include <QObject>
#include <any>

class Task
{
public:
    virtual ~Task() = default;
    virtual void Run() = 0;
    virtual void Destory() = 0;
};

class SignalReturnTask : public QObject, public Task
{
    Q_OBJECT
public:
    SignalReturnTask(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    ~SignalReturnTask() = default;

signals:
    void result(const std::any& ret);
};

