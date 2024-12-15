#pragma once
#include <QObject>

#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/Task.h"
#include "ClientLog.h"
#include "const_string.h"

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

template <typename TaskFunc, typename Callback, typename = std::enable_if_t<std::is_invocable_v<std::decay_t<TaskFunc>>>,
          typename = std::enable_if_t<std::is_invocable_v<std::decay_t<Callback>, std::invoke_result_t<std::decay_t<TaskFunc>>>>>
void runTask(TaskFunc taskFunc, Callback callback, QObject* object = nullptr)
{
    static_assert(std::is_same_v<std::decay_t<typename CallableTraits<Callback>::FirstArgType>, std::invoke_result_t<std::decay_t<TaskFunc>>>,
                  "Callback FirstArgType must be result of task");
    using ResultType = std::invoke_result_t<std::decay_t<TaskFunc>>;
    auto task = std::make_shared<TemplateSignalReturnTask<decltype(taskFunc)>>(taskFunc);
    QObject::connect(task.get(), &SignalReturnTask::result, object, [callback](const std::any& res) {
        try
        {
            const auto& result = std::any_cast<ResultType>(res);
            callback(result);
        }
        catch (const std::exception& e)
        {
            CLINET_LOG_ERROR("exception ocurred, message: {}", e.what());
        }
    });
    ThreadPool::instance().enqueue(task);
}
