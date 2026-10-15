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
#include "Plugin/PluginManager.h"

namespace
{
class ThrowingDownloader : public download::AbstractDownloader
{
public:
    ThrowingDownloader()
    {
        m_guid = "throwing-downloader";
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

// 记录各回调被调用次数，用于验证 DownloadStatusThread 的状态分派与移除策略。
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

    std::atomic<int> startCalls{0};
    std::atomic<int> stopCalls{0};
    std::atomic<int> pauseCalls{0};
    std::atomic<int> resumeCalls{0};
    std::atomic<int> statusCalls{0};
    std::atomic<int> finishCalls{0};
};

class MetadataPlugin : public plugin::IPlugin
{
public:
    explicit MetadataPlugin(bool alwaysThrow)
        : m_alwaysThrow(alwaysThrow)
    {
        m_message.pluginId = 42;
        m_message.name = "test-plugin";
    }

    const PluginMessage& pluginMessage() const override
    {
        ++m_metadataCalls;
        if (m_alwaysThrow || m_metadataCalls > 1)
        {
            throw std::runtime_error("metadata failed");
        }
        return m_message;
    }

    const std::vector<uint8_t>& websiteIcon() override
    {
        throw std::runtime_error("icon failed");
    }
    bool canParseUrl(const std::string&) override
    {
        throw std::runtime_error("parse failed");
    }
    adapter::VideoView getVideoView(const std::string&) override
    {
        throw std::runtime_error("view failed");
    }
    std::shared_ptr<download::FileDownloader> getDownloader(const VideoInfoFull&) override
    {
        throw std::runtime_error("downloader failed");
    }
    LoginProxy loginer() override
    {
        throw std::runtime_error("login failed");
    }

    int metadataCalls() const
    {
        return m_metadataCalls;
    }

private:
    bool m_alwaysThrow;
    mutable int m_metadataCalls{0};
    PluginMessage m_message;
};
}  // namespace

TEST(DownloadStatusThreadTest, ConvertsDownloaderExceptionToErrorAndRemovesTask)
{
    download::DownloadStatusThread statusThread;
    auto downloader = std::make_shared<ThrowingDownloader>();
    ASSERT_TRUE(statusThread.addTask(downloader));

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (!statusThread.empty() && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    EXPECT_TRUE(statusThread.empty());
    EXPECT_EQ(downloader->status(), download::AbstractDownloader::Error);
}

TEST(PluginProxyTest, CachesMetadataAndDoesNotCallPluginFromCatch)
{
    auto realPlugin = std::make_shared<MetadataPlugin>(false);
    plugin::PluginProxy proxy(realPlugin);

    ASSERT_TRUE(proxy.isValid());
    EXPECT_EQ(proxy.pluginMessage().pluginId, 42);
    EXPECT_EQ(proxy.pluginMessage().name, "test-plugin");
    EXPECT_FALSE(proxy.canParseUrl("https://example.com"));
    EXPECT_EQ(realPlugin->metadataCalls(), 1);
}

TEST(PluginProxyTest, InvalidatesPluginWhenInitialMetadataThrows)
{
    plugin::PluginProxy proxy(std::make_shared<MetadataPlugin>(true));

    EXPECT_FALSE(proxy.isValid());
    EXPECT_EQ(proxy.pluginMessage().pluginId, 0);
}

TEST(DownloadStatusThreadTest, StartsEmpty)
{
    download::DownloadStatusThread statusThread;

    EXPECT_TRUE(statusThread.empty());
}

TEST(DownloadStatusThreadTest, RejectsDuplicateGuid)
{
    download::DownloadStatusThread statusThread;

    // 用 Downloading 状态让任务留在队列里（该状态既不结束也不移除任务）
    auto downloader = std::make_shared<RecordingDownloader>("duplicated-guid", download::AbstractDownloader::Downloading);

    EXPECT_TRUE(statusThread.addTask(downloader));
    EXPECT_FALSE(statusThread.addTask(downloader));
}

TEST(DownloadStatusThreadTest, RemovesTaskAndCallsFinishForFinishedStatus)
{
    download::DownloadStatusThread statusThread;

    auto downloader = std::make_shared<RecordingDownloader>("finished-guid", download::AbstractDownloader::Finished);
    ASSERT_TRUE(statusThread.addTask(downloader));

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (!statusThread.empty() && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    EXPECT_TRUE(statusThread.empty());
    EXPECT_GE(downloader->finishCalls.load(), 1);
    EXPECT_EQ(downloader->startCalls.load(), 0);
}

TEST(DownloadStatusThreadTest, DispatchesCallbacksPerStatus)
{
    download::DownloadStatusThread statusThread;

    auto downloader = std::make_shared<RecordingDownloader>("paused-guid", download::AbstractDownloader::Pause);
    ASSERT_TRUE(statusThread.addTask(downloader));

    // 状态轮询每轮间隔 600ms，这里等到至少发生过一次分派
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (downloader->pauseCalls.load() == 0 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    EXPECT_GE(downloader->pauseCalls.load(), 1);
    // Pause 状态不会移除任务
    EXPECT_FALSE(statusThread.empty());

    statusThread.stop();
}

namespace
{
// 被测的 DownloadStatusThread / PluginProxy 内部写日志，宏为
// SPDLOG_LOGGER_CALL(spdlog::get("Xxx"), ...)：spdlog::get() 对未注册的名字返回空
// shared_ptr 并被宏直接解引用（段错误），因此在测试环境启动时统一注册到 null sink。
class NullLoggerEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        for (const char* name : {"Download", "Plugin"})
        {
            if (!spdlog::get(name))
            {
                spdlog::register_logger(std::make_shared<spdlog::logger>(name, std::make_shared<spdlog::sinks::null_sink_mt>()));
            }
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
