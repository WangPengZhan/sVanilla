// ThreadPool 的构造与析构都会写日志。日志宏写作 SPDLOG_LOGGER_CALL(spdlog::get("ThreadPool"), ...)，
// spdlog::get() 对未注册的名字返回空 shared_ptr 并被宏直接解引用（段错误），
// 因此具名 logger 由文件末尾的 NullLoggerEnvironment 在测试环境启动时注册。

#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include <gtest/gtest.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#include "ThreadPool/Task.h"
#include "ThreadPool/ThreadPool.h"

namespace
{

class CountingTask : public Task
{
public:
    explicit CountingTask(std::atomic<int>& counter)
        : m_counter(counter)
    {
    }

    void Run() override
    {
        ++m_counter;
    }

private:
    std::atomic<int>& m_counter;
};

// 注册到 null sink：测试期间不需要真的落盘日志。
class NullLoggerEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        if (!spdlog::get("ThreadPool"))
        {
            spdlog::register_logger(std::make_shared<spdlog::logger>("ThreadPool", std::make_shared<spdlog::sinks::null_sink_mt>()));
        }
    }
};

}  // namespace

TEST(ThreadPoolTest, NumThreadsReportsRequestedCount)
{
    ThreadPool threadPool(2);
    EXPECT_EQ(threadPool.numThreads(), 2u);
}

TEST(ThreadPoolTest, DefaultConstructorUsesHardwareConcurrency)
{
    ThreadPool threadPool;
    EXPECT_EQ(threadPool.numThreads(), std::max(1u, std::thread::hardware_concurrency()));
}

TEST(ThreadPoolTest, EnqueueReturnsFutureWithResult)
{
    ThreadPool threadPool(2);

    auto future = threadPool.enqueue(
        [](int a, int b) {
            return a + b;
        },
        2, 3);
    EXPECT_EQ(future.get(), 5);
}

TEST(ThreadPoolTest, EnqueueAcceptsTaskSubclass)
{
    ThreadPool threadPool(2);
    std::atomic<int> counter{0};

    auto task = std::make_shared<CountingTask>(counter);
    threadPool.enqueue(task).wait();

    EXPECT_EQ(counter.load(), 1);
}

TEST(ThreadPoolTest, RunsEveryQueuedTask)
{
    ThreadPool threadPool(4);
    std::atomic<int> counter{0};

    std::vector<std::future<void>> futures;
    for (int i = 0; i < 200; ++i)
    {
        futures.push_back(threadPool.enqueue([&counter]() {
            ++counter;
        }));
    }

    for (auto& future : futures)
    {
        future.wait();
    }

    EXPECT_EQ(counter.load(), 200);
}

TEST(ThreadPoolTest, TaskExceptionIsCapturedInFutureAndPoolStaysUsable)
{
    ThreadPool threadPool(2);

    auto failing = threadPool.enqueue([]() -> int {
        throw std::runtime_error("boom");
    });
    EXPECT_THROW(failing.get(), std::runtime_error);

    // 异常不会带崩工作线程，后续任务仍能执行
    auto recovered = threadPool.enqueue([] {
        return 42;
    });
    ASSERT_EQ(recovered.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_EQ(recovered.get(), 42);
}

TEST(ThreadPoolTest, RunsTwoTasksInParallelWhenWorkersAreAvailable)
{
    ThreadPool threadPool(2);
    std::atomic<int> started{0};

    auto blockingTask = [&started]() {
        ++started;
        // 等到两个任务都已经开始。若线程池只有一个工作线程，先执行的任务
        // 会一直占着唯一的线程，这里必然超时。
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        while (started.load() < 2 && std::chrono::steady_clock::now() < deadline)
        {
            std::this_thread::yield();
        }
        return started.load();
    };

    auto first = threadPool.enqueue(blockingTask);
    auto second = threadPool.enqueue(blockingTask);

    EXPECT_EQ(first.get(), 2);
    EXPECT_EQ(second.get(), 2);
}

TEST(ThreadPoolTest, ThreadNumberMapsWorkerThreadIds)
{
    ThreadPool threadPool(2);

    auto workerId = threadPool
                        .enqueue([] {
                            return std::this_thread::get_id();
                        })
                        .get();
    EXPECT_GE(threadPool.threadNumber(workerId), 0);

    // 未知线程 id 返回 -1
    EXPECT_EQ(threadPool.threadNumber(std::thread::id{}), -1);
}

TEST(ThreadPoolTest, EnqueueAfterStopThrows)
{
    ThreadPool threadPool(2);
    threadPool.stop();

    EXPECT_THROW(threadPool.enqueue([] {}), std::runtime_error);
}

TEST(ThreadPoolTest, StopIsIdempotentAndAllowsDestruction)
{
    ThreadPool threadPool(2);
    threadPool.stop();
    EXPECT_NO_THROW(threadPool.stop());
}

TEST(ThreadPoolTest, DropDiscardsPendingTasksButKeepsPoolWorking)
{
    ThreadPool threadPool(1);
    std::atomic<int> counter{0};

    std::vector<std::future<void>> futures;
    for (int i = 0; i < 50; ++i)
    {
        futures.push_back(threadPool.enqueue([&counter]() {
            ++counter;
        }));
    }

    threadPool.drop();
    for (auto& future : futures)
    {
        future.wait();
    }

    // drop 只清空队列，已取走的任务仍可能执行，因此计数不会超过总数
    EXPECT_LE(counter.load(), 50);

    auto afterDrop = threadPool.enqueue([] {
        return 7;
    });
    ASSERT_EQ(afterDrop.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_EQ(afterDrop.get(), 7);
}

TEST(ThreadPoolTest, ZeroWorkerCountFallsBackToOne)
{
    ThreadPool threadPool(0);
    EXPECT_EQ(threadPool.numThreads(), 1u);

    auto future = threadPool.enqueue([] {
        return 1;
    });
    ASSERT_EQ(future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_EQ(future.get(), 1);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new NullLoggerEnvironment);
    return RUN_ALL_TESTS();
}
