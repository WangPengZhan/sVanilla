#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <any>

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

#include "ThreadPool/ThreadPool.h"

TEST(TheadPool_Test, INIT)
{
    spdlog::rotating_logger_mt<spdlog::async_factory>("TheadPool", "log/TheadPool.log", 1024 * 1024 * 10, 100);

    ThreadPool &threadPool = ThreadPool::instance();
    std::cout << "threadPool.numThreads() = " << threadPool.numThreads() << std::endl;

    EXPECT_EQ(threadPool.numThreads(), std::thread::hardware_concurrency());
}

TEST(TheadPool_Test, EN)
{
    //spdlog::rotating_logger_mt<spdlog::async_factory>("TheadPool", "log/TheadPool.log", 1024 * 1024 * 10, 100);

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

    TestTask* pTask = new TestTask;
    threadPool.enqueue(pTask);
}
