// Aria2Net 模块单元测试。
//
// AriaServer 的职责是拉起/关闭 aria2c 子进程。测试环境里没有 aria2c 可执行文件，
// 所以这里只覆盖「未启动子进程时」的行为：构造/析构、日志目录创建、回调注册，
// 以及 closeServer / forceCloseServer 在 m_aria2Process 为空时必须是安全的空操作。
//
// 注意：startLocalServerAsync() 依赖 aria2c 二进制并会真正启动外部进程，不在单测范围内。

#include <atomic>
#include <chrono>
#include <filesystem>
#include <memory>
#include <system_error>

#include <QApplication>
#include <QDir>
#include <QString>

#include <gtest/gtest.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#include "Aria2Net/AriaServer/AriaServer.h"

namespace
{
// AriaServer 内部使用 TransceiverForQt（跨线程投递），需要 Qt 事件循环可用。
// offscreen 平台插件在 main() 中设置。

// 用例独占的临时目录，析构时整体删除，避免在系统 temp 里留下垃圾。
class ScopedTempDir
{
public:
    explicit ScopedTempDir(std::string tag)
    {
        // runId 保证不同次运行不会复用同一目录：上一轮可能残留没删干净的文件。
        static const std::string runId = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        static std::atomic<unsigned> sequence{0};
        m_path = std::filesystem::temp_directory_path() / ("svanilla_" + tag + "_" + runId + "_" + std::to_string(sequence++));
        std::filesystem::create_directories(m_path);
    }

    ScopedTempDir(const ScopedTempDir&) = delete;
    ScopedTempDir& operator=(const ScopedTempDir&) = delete;

    ~ScopedTempDir()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    const std::filesystem::path& path() const
    {
        return m_path;
    }

private:
    std::filesystem::path m_path;
};

// AriaServer 写日志的宏是 SPDLOG_LOGGER_CALL(spdlog::get("Aria2Net"), ...)，
// spdlog::get() 对未注册的名字返回空 shared_ptr 并被宏直接解引用（段错误），
// 因此在测试环境启动时把它们注册到丢弃输出的 null sink 上。
class NullLoggerEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        for (const char* name : {"Aria2Net", "log"})
        {
            if (!spdlog::get(name))
            {
                spdlog::register_logger(std::make_shared<spdlog::logger>(name, std::make_shared<spdlog::sinks::null_sink_mt>()));
            }
        }
    }
};
}  // namespace

TEST(AriaServerTest, ConstructsAndDestructsWithoutStartingProcess)
{
    EXPECT_NO_THROW({ aria2net::AriaServer server; });
}

TEST(AriaServerTest, SetLogDirCreatesAriaSubdirectory)
{
    ScopedTempDir directory("aria2net");
    aria2net::AriaServer server;

    server.setLogDir(directory.path().string().c_str());

    // setLogDir() 会在传入目录下再建一级 "aria"
    EXPECT_TRUE(QDir(QString::fromStdString(directory.path().string()) + "/aria").exists());
}

TEST(AriaServerTest, SetErrorFuncAndCloseFuncAcceptCallables)
{
    aria2net::AriaServer server;

    int errorCalls = 0;
    int closeCalls = 0;
    EXPECT_NO_THROW(server.setErrorFunc([&errorCalls] {
        ++errorCalls;
    }));
    EXPECT_NO_THROW(server.setCloseFunc([&closeCalls] {
        ++closeCalls;
    }));

    // 未启动子进程时两个回调都不应被触发。
    // 注：m_closeFunc 在实现中只赋值、从不调用（已知缺陷）。
    EXPECT_EQ(errorCalls, 0);
    EXPECT_EQ(closeCalls, 0);
}

TEST(AriaServerTest, CloseServerOnNeverStartedServerIsSafe)
{
    aria2net::AriaServer server;
    EXPECT_NO_THROW(server.closeServer());
    // closeServer() 是异步投递到 transceiver 线程的；析构会等待其完成。
}

TEST(AriaServerTest, ForceCloseServerOnNeverStartedServerIsSafe)
{
    aria2net::AriaServer server;
    EXPECT_NO_THROW(server.forceCloseServer());
}

TEST(AriaServerTest, RepeatedCloseCallsAreSafe)
{
    aria2net::AriaServer server;
    EXPECT_NO_THROW(server.closeServer());
    EXPECT_NO_THROW(server.closeServer());
    EXPECT_NO_THROW(server.forceCloseServer());
}

TEST(AriaServerTest, TestServerIsCurrentlyNoOp)
{
    aria2net::AriaServer server;
    // testServer() 目前是空实现（仅有一行注释：for getvirsion）
    EXPECT_NO_THROW(server.testServer());
}

int main(int argc, char** argv)
{
    // AriaServer 使用 QApplication::applicationDirPath()，因此需要 QApplication。
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication application(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new NullLoggerEnvironment);
    return RUN_ALL_TESTS();
}
