#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <any>

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

#include <QApplication>
#include <QTimer>

#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/ThreadPoolLog.h"

TEST(TheadPool_Test, INIT)
{
    ThreadPool& threadPool = ThreadPool::instance();
    std::cout << "threadPool.numThreads() = " << threadPool.numThreads() << std::endl;

    EXPECT_EQ(threadPool.numThreads(), std::thread::hardware_concurrency());
}

TEST(TheadPool_Test, EN)
{
    // spdlog::rotating_logger_mt<spdlog::async_factory>("TheadPool", "log/TheadPool.log", 1024 * 1024 * 10, 100);

    ThreadPool& threadPool = ThreadPool::instance();
    std::cout << "threadPool.numThreads() = " << threadPool.numThreads() << std::endl;

    threadPool.enqueue([]() {
        std::cout << "start task1" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "end task1" << std::endl;
    });

    class TestTask : public Task
    {
    public:
        void Run() override
        {
            std::cout << "start task2" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << "end task2" << std::endl;
        }
    };

    auto pTask = std::make_shared<TestTask>();
    threadPool.enqueue(pTask);
}

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
            THEADPOOL_LOG_INFO("exception ocurred, message: {}", e.what());
        }
    });
    ThreadPool::instance().enqueue(task);
}

TEST(TheadPool_Test, EN_Multi_Tasks)
{
    spdlog::rotating_logger_mt<spdlog::async_factory>("TheadPool", "log/TheadPool.log", 1024 * 1024 * 10, 100);

    spdlog::init_thread_pool(32768, 1);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%g:%#,%!][%l] %v");
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::err);
    spdlog::flush_every(std::chrono::seconds(5));
    spdlog::rotating_logger_mt<spdlog::async_factory>("TheadPool", "log/TheadPool.log", 1024 * 1024 * 10, 100);

    int a;
    QApplication app(a, nullptr);

    ThreadPool& threadPool = ThreadPool::instance();
    std::cout << "threadPool.numThreads() = " << threadPool.numThreads() << std::endl;
    std::atomic_int num = 0;
    std::vector<std::thread> threads;
    QObject obj;
    for (int i = 0; i < 10; ++i)
    {
        threads.emplace_back(std::thread([&num, &obj]() {
            for (int i = 0; i < 1000; ++i)
            {
                auto task = [&num]() -> int {
                    int ret = num.fetch_add(1);
                    THEADPOOL_LOG_INFO("task num is {}", ret);
                    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 20));
                    return ret;
                };
                auto callback = [](int num) {
                    std::cout << "num is " << std::endl;
                    THEADPOOL_LOG_INFO("callback num is {}", num);
                };
                runTask(task, callback, &obj);
            }
        }));
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    threadPool.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    QTimer::singleShot(10000, [&app]() {
        app.quit();
    });

    app.exec();

    spdlog::shutdown();
}
