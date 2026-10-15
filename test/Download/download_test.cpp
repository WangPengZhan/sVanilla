// Download 模块单元测试。
//
// DownloadStatusThread 是一个 600ms 轮询的"状态泵"：每轮把 m_downloadTasks 快照一份，
// 按 AbstractDownloader::status() 分派到对应虚函数，再把 Stopped / Finished / Error
// 三类任务从表里移除。这里的用例把「状态 → 回调 → 是否移除」的映射表逐条固定下来。
//
// 注意：轮询间隔是硬编码的 600ms，因此每个"等一轮"的用例至少要花约 0.6s。
//
// DownloadStatusThread 内部写日志，宏为 SPDLOG_LOGGER_CALL(spdlog::get("Download"), ...)：
// spdlog::get() 对未注册的名字返回空 shared_ptr 并被宏直接解引用（段错误），
// 因此具名 logger 由文件末尾的 NullLoggerEnvironment 在测试环境启动时注册。

#include <atomic>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

#include <gtest/gtest.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#include "Download/AbstractDownloader.h"
#include "Download/DownloadStatusThread.h"

namespace
{
// 记录各回调调用次数。
class RecordingDownloader : public download::AbstractDownloader
{
public:
    RecordingDownloader(std::string guid, Status initialStatus)
    {
        m_guid = std::move(guid);
        setStatus(initialStatus);
    }

    void start() override
    {
        ++startCalls;
    }
    void stop() override
    {
        ++stopCalls;
    }
    void pause() override
    {
        ++pauseCalls;
    }
    void resume() override
    {
        ++resumeCalls;
    }
    void downloadStatus() override
    {
        ++statusCalls;
    }
    void finish() override
    {
        ++finishCalls;
    }

    int totalCalls() const
    {
        return startCalls.load() + stopCalls.load() + pauseCalls.load() + resumeCalls.load() + statusCalls.load() + finishCalls.load();
    }

    std::atomic<int> startCalls{0};
    std::atomic<int> stopCalls{0};
    std::atomic<int> pauseCalls{0};
    std::atomic<int> resumeCalls{0};
    std::atomic<int> statusCalls{0};
    std::atomic<int> finishCalls{0};
};

// start() 抛异常，用于验证异常被转成 Error 并移除任务。
class ThrowingDownloader : public download::AbstractDownloader
{
public:
    ThrowingDownloader()
    {
        m_guid = "throwing-guid";
        setStatus(Ready);
    }

    void start() override
    {
        throw std::runtime_error("start failed");
    }
    void stop() override
    {
    }
    void pause() override
    {
    }
    void resume() override
    {
    }
    void downloadStatus() override
    {
    }
    void finish() override
    {
    }
};

template <typename Predicate>
bool waitUntil(Predicate predicate, std::chrono::milliseconds timeout = std::chrono::seconds(8))
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline)
    {
        if (predicate())
        {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return predicate();
}

// 让轮询线程至少完整跑过一轮（间隔 600ms）。
void waitOnePollRound()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1400));
}
}  // namespace

TEST(DownloadStatusThreadTest, StartsEmpty)
{
    download::DownloadStatusThread statusThread;
    EXPECT_TRUE(statusThread.empty());
}

TEST(DownloadStatusThreadTest, AddTaskRejectsDuplicateGuid)
{
    download::DownloadStatusThread statusThread;
    // Downloading 状态不会移除任务，便于验证重复插入被拒绝
    auto downloader = std::make_shared<RecordingDownloader>("dup-guid", download::AbstractDownloader::Downloading);

    EXPECT_TRUE(statusThread.addTask(downloader));
    EXPECT_FALSE(statusThread.addTask(downloader));
    EXPECT_FALSE(statusThread.empty());

    statusThread.stop();
}

TEST(DownloadStatusThreadTest, AddTaskDistinctGuidsCoexist)
{
    download::DownloadStatusThread statusThread;
    auto first = std::make_shared<RecordingDownloader>("guid-a", download::AbstractDownloader::Downloading);
    auto second = std::make_shared<RecordingDownloader>("guid-b", download::AbstractDownloader::Downloading);

    EXPECT_TRUE(statusThread.addTask(first));
    EXPECT_TRUE(statusThread.addTask(second));
    EXPECT_FALSE(statusThread.empty());

    statusThread.stop();
}

// Ready → start()，且 Ready 不在移除集合里。
TEST(DownloadStatusThreadTest, ReadyDispatchesStartAndKeepsTask)
{
    download::DownloadStatusThread statusThread;
    auto downloader = std::make_shared<RecordingDownloader>("ready-guid", download::AbstractDownloader::Ready);
    ASSERT_TRUE(statusThread.addTask(downloader));

    EXPECT_TRUE(waitUntil([&] {
        return downloader->startCalls.load() >= 1;
    }));
    EXPECT_EQ(downloader->stopCalls.load(), 0);
    EXPECT_EQ(downloader->finishCalls.load(), 0);
    EXPECT_FALSE(statusThread.empty());

    statusThread.stop();
}

// Downloading → downloadStatus()，不移除。
TEST(DownloadStatusThreadTest, DownloadingDispatchesStatusAndKeepsTask)
{
    download::DownloadStatusThread statusThread;
    auto downloader = std::make_shared<RecordingDownloader>("downloading-guid", download::AbstractDownloader::Downloading);
    ASSERT_TRUE(statusThread.addTask(downloader));

    EXPECT_TRUE(waitUntil([&] {
        return downloader->statusCalls.load() >= 1;
    }));
    EXPECT_EQ(downloader->startCalls.load(), 0);
    EXPECT_FALSE(statusThread.empty());

    statusThread.stop();
}

// Pause → pause()，Resumed → resume()，两者都不移除任务。
TEST(DownloadStatusThreadTest, PauseAndResumedDispatchTheirOwnCallbacks)
{
    download::DownloadStatusThread statusThread;
    auto paused = std::make_shared<RecordingDownloader>("pause-guid", download::AbstractDownloader::Pause);
    auto resumed = std::make_shared<RecordingDownloader>("resumed-guid", download::AbstractDownloader::Resumed);
    ASSERT_TRUE(statusThread.addTask(paused));
    ASSERT_TRUE(statusThread.addTask(resumed));

    EXPECT_TRUE(waitUntil([&] {
        return paused->pauseCalls.load() >= 1 && resumed->resumeCalls.load() >= 1;
    }));
    EXPECT_EQ(paused->resumeCalls.load(), 0);
    EXPECT_EQ(resumed->pauseCalls.load(), 0);
    EXPECT_FALSE(statusThread.empty());

    statusThread.stop();
}

// Waiting / Paused 不在 switch 的任何一个 case 上，既不回调也不移除。
TEST(DownloadStatusThreadTest, WaitingAndPausedStatusesAreIgnored)
{
    download::DownloadStatusThread statusThread;
    auto waiting = std::make_shared<RecordingDownloader>("waiting-guid", download::AbstractDownloader::Waiting);
    auto paused = std::make_shared<RecordingDownloader>("paused-guid", download::AbstractDownloader::Paused);
    ASSERT_TRUE(statusThread.addTask(waiting));
    ASSERT_TRUE(statusThread.addTask(paused));

    waitOnePollRound();

    EXPECT_EQ(waiting->totalCalls(), 0);
    EXPECT_EQ(paused->totalCalls(), 0);
    EXPECT_FALSE(statusThread.empty());

    statusThread.stop();
}

// Stopped → stop() + 移除；Finished → finish() + 移除；Error → 只移除、不回调。
TEST(DownloadStatusThreadTest, StoppedFinishedAndErrorAreRemoved)
{
    download::DownloadStatusThread statusThread;
    auto stopped = std::make_shared<RecordingDownloader>("stopped-guid", download::AbstractDownloader::Stopped);
    auto finished = std::make_shared<RecordingDownloader>("finished-guid", download::AbstractDownloader::Finished);
    auto errored = std::make_shared<RecordingDownloader>("error-guid", download::AbstractDownloader::Error);
    ASSERT_TRUE(statusThread.addTask(stopped));
    ASSERT_TRUE(statusThread.addTask(finished));
    ASSERT_TRUE(statusThread.addTask(errored));

    EXPECT_TRUE(waitUntil([&] {
        return statusThread.empty();
    }));

    EXPECT_GE(stopped->stopCalls.load(), 1);
    EXPECT_GE(finished->finishCalls.load(), 1);
    // Error 只做移除，不触发任何回调
    EXPECT_EQ(errored->totalCalls(), 0);
}

// 任务回调抛异常时，线程把它转成 Error 并移除，不能把整个线程带崩。
TEST(DownloadStatusThreadTest, ThrowingDownloaderBecomesErrorAndIsRemoved)
{
    download::DownloadStatusThread statusThread;
    auto downloader = std::make_shared<ThrowingDownloader>();
    ASSERT_TRUE(statusThread.addTask(downloader));

    EXPECT_TRUE(waitUntil([&] {
        return statusThread.empty();
    }));
    EXPECT_EQ(downloader->status(), download::AbstractDownloader::Error);
}

// 线程在异常之后仍能继续处理后续任务。
TEST(DownloadStatusThreadTest, ThreadSurvivesTaskException)
{
    download::DownloadStatusThread statusThread;
    ASSERT_TRUE(statusThread.addTask(std::make_shared<ThrowingDownloader>()));

    auto healthy = std::make_shared<RecordingDownloader>("healthy-guid", download::AbstractDownloader::Finished);
    ASSERT_TRUE(statusThread.addTask(healthy));

    EXPECT_TRUE(waitUntil([&] {
        return healthy->finishCalls.load() >= 1;
    }));
}

TEST(DownloadStatusThreadTest, StopIsIdempotent)
{
    download::DownloadStatusThread statusThread;
    EXPECT_NO_THROW(statusThread.stop());
    EXPECT_NO_THROW(statusThread.stop());
}

TEST(DownloadStatusThreadTest, DestructionJoinsPollingThread)
{
    // 构造后立刻析构：析构函数会 stop() + join()，不应挂起。
    EXPECT_NO_THROW({
        download::DownloadStatusThread statusThread;
        EXPECT_TRUE(statusThread.empty());
    });
}

namespace
{
// 注册到 null sink：测试期间不需要真的落盘日志。
class NullLoggerEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        if (!spdlog::get("Download"))
        {
            spdlog::register_logger(std::make_shared<spdlog::logger>("Download", std::make_shared<spdlog::sinks::null_sink_mt>()));
        }
    }
};
}  // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new NullLoggerEnvironment);
    return RUN_ALL_TESTS();
}
