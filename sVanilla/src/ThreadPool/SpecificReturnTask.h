#pragma once

#include "ThreadPool.h"
#include "Task.h"

class SpecificReturnTask : public QObject
{
    Q_OBJECT
public:
    static auto runner() -> SpecificReturnTask&;

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
    void run(TaskFunc taskFunc, Callback callback);
};

template <typename TaskFunc, typename Callback>
void SpecificReturnTask::run(TaskFunc taskFunc, Callback callback)
{
    using ResultType = typename CallableTraits<Callback>::FirstArgType;
    auto task = std::make_shared<TemplateSignalReturnTask<decltype(taskFunc)>>(taskFunc);
    QObject::connect(task.get(), &SignalReturnTask::result, this, [this, callback](const std::any& res) {
        try
        {
            const auto& result = std::any_cast<ResultType>(res);
            callback(result);
        }
        catch (const std::bad_any_cast& e)
        {
            // 这里可以添加错误处理代码
        }
    });
    ThreadPool::instance().enqueue(task);
}
