#pragma once
#include <QObject>

#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/Task.h"

template <typename Func>
struct CallableTraits : CallableTraits<decltype(&Func::operator())>
{
};

template <typename ReturnType, typename ClassType, typename Arg, typename... Args>
struct CallableTraits<ReturnType (ClassType::*)(Arg, Args...) const>
{
    // using FirstArgType = std::tuple_element_t<0, std::tuple<Args...>>;
    using FirstArgType = Arg;
};

template <typename TaskFunc, typename Callback>
void runTask(TaskFunc taskFunc, Callback callback, QObject* object = nullptr)
{
    using ResultType = typename CallableTraits<Callback>::FirstArgType;
    auto task = std::make_shared<TemplateSignalReturnTask<decltype(taskFunc)>>(taskFunc);
    QObject::connect(task.get(), &SignalReturnTask::result, object, [callback](const std::any& res) {
        try
        {
            const auto& result = std::any_cast<ResultType>(res);
            callback(res);
        }
        catch (const std::bad_any_cast& e)
        {
            // 这里可以添加错误处理代码
        }
    });
    ThreadPool::instance().enqueue(task);
}
