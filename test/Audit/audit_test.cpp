#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include <gtest/gtest.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#include "Download/AbstractDownloader.h"
#include "Download/DownloadStatusThread.h"
#include "Plugin/PluginManager.h"

namespace
{
void registerTestLogger(const std::string& name)
{
    if (!spdlog::get(name))
    {
        auto sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        spdlog::register_logger(std::make_shared<spdlog::logger>(name, std::move(sink)));
    }
}

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
    registerTestLogger("Download");
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
    registerTestLogger("Plugin");
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
    registerTestLogger("Plugin");
    plugin::PluginProxy proxy(std::make_shared<MetadataPlugin>(true));

    EXPECT_FALSE(proxy.isValid());
    EXPECT_EQ(proxy.pluginMessage().pluginId, 0);
}
