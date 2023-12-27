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
    }
    ~SignalReturnTask() override = default;


    
signals:
    void result(const std::any& ret);
};
