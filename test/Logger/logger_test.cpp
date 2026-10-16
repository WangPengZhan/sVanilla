// Logger 模块单元测试。
//
// 覆盖三层：
//   1. LogUtil.h 的 scope_guard / 计时宏（纯头文件逻辑）
//   2. Logger 门面（defaultName、registerLogger → get）
//   3. 日志宏的 logger 路由（通用宏走 "log"，模块宏走各自名字）
//
// 注意：所有模块日志宏都写作 SPDLOG_LOGGER_CALL(spdlog::get("Xxx"), ...)，
// spdlog::get() 对未注册的名字返回空 shared_ptr，宏会直接解引用它。
// 测试进程不会执行 ClientUi 的 Logger::getInstance()，因此下面这些具名 logger
// 统一在测试环境启动时注册（见文件末尾的 NullLoggerEnvironment）。

#include <atomic>
#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <system_error>

#include <gtest/gtest.h>
#include <spdlog/async.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#include "Aria2Net/AriaLog.h"
#include "ClientUi/ClientLog.h"
#include "Download/DownloadLog.h"
#include "Logger/LogUtil.h"
#include "Logger/Logger.h"
#include "Plugin/PluginLog.h"
#include "Sqlite/SQLiteLog.h"
#include "ThreadPool/ThreadPoolLog.h"

namespace
{
constexpr const char* kTempLoggerName = "sv_logger_test_temp";
std::atomic<int> timerLogCount{0};
#define COUNT_TIMER_LOG(...) ++timerLogCount

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
}  // namespace

// ---------- scope_guard（LogUtil.h 公开的 RAII 工具） ----------

TEST(ScopeGuardTest, RunsReleaseOnScopeExit)
{
    int calls = 0;
    {
        scope_guard guard = [&calls] {
            ++calls;
        };
        EXPECT_EQ(calls, 0);  // 构造时不执行
    }
    EXPECT_EQ(calls, 1);  // 离开作用域恰好执行一次
}

TEST(ScopeGuardTest, MultipleGuardsAreIndependent)
{
    int first = 0;
    int second = 0;
    {
        scope_guard guardA = [&first] {
            ++first;
        };
        scope_guard guardB = [&second] {
            ++second;
        };
    }
    EXPECT_EQ(first, 1);
    EXPECT_EQ(second, 1);
}

TEST(ScopeGuardTest, MutatesCapturedStateByReference)
{
    std::string collected;
    {
        scope_guard guard = [&collected] {
            collected = "done";
        };
    }
    EXPECT_EQ(collected, "done");
}

TEST(ScopeGuardTest, ReleaseRunsOnceEvenAfterScopeExit)
{
    int calls = 0;
    {
        scope_guard guard = [&calls] { ++calls; };
        guard.release();
        guard.release();
        EXPECT_EQ(calls, 1);
    }
    EXPECT_EQ(calls, 1);
}

// ---------- Logger 门面 ----------

TEST(LoggerTest, DefaultNameIsLog)
{
    // 通用宏 LogT/D/I/W/E/C/O 全部写死这个名字，是宏路由的关键常量。
    EXPECT_STREQ(Logger::defaultName, "log");
}

TEST(LoggerTest, GetReturnsNullForUnregisteredName)
{
    // 这正是"未注册即段错误"的根源：get() 不抛异常，只返回空指针。
    EXPECT_EQ(Logger::get("sv_definitely_not_registered_logger"), nullptr);
}

// Logger::registerLogger() 使用 spdlog 的 async_factory，必须先建立全局线程池。
class LoggerFacadeTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        spdlog::init_thread_pool(8192, 1);
    }
};

TEST_F(LoggerFacadeTest, RegisterLoggerMakesItRetrievable)
{
    ScopedTempDir directory("logger");
    Logger::setLogDir(directory.path().string() + "/");

    Logger::registerLogger(kTempLoggerName);
    EXPECT_NE(Logger::get(kTempLoggerName), nullptr);
}

TEST_F(LoggerFacadeTest, RegisteringSameNameTwiceThrows)
{
    ScopedTempDir directory("logger");
    Logger::setLogDir(directory.path().string() + "/");

    Logger::registerLogger("sv_logger_test_dup");
    // spdlog 的 register_logger 对重名会抛 spdlog_ex
    EXPECT_THROW(Logger::registerLogger("sv_logger_test_dup"), spdlog::spdlog_ex);
}

// ---------- 日志宏路由 ----------

TEST(LogMacroTest, GeneralMacrosDoNotThrowWhenDefaultLoggerRegistered)
{
    EXPECT_NO_THROW(LogI("info {}", 1));
    EXPECT_NO_THROW(LogW("warn"));
    EXPECT_NO_THROW(LogE("error {}", 2));
    EXPECT_NO_THROW(LogD("debug"));
}

TEST(LogMacroTest, ModuleMacrosTargetTheirOwnLogger)
{
    // 每个模块宏绑定各自的具名 logger；它们在测试环境启动时已注册，逐个调用不应抛异常。
    EXPECT_NO_THROW(THEADPOOL_LOG_INFO("threadpool"));  // 注意拼写：THEADPOOL（少一个 R）
    EXPECT_NO_THROW(SQLITE_LOG_INFO("sqlite"));
    EXPECT_NO_THROW(DOWNLOAD_LOG_INFO("download"));
    EXPECT_NO_THROW(PLUGIN_LOG_INFO("plugin"));
    EXPECT_NO_THROW(ARIA_LOG_INFO("aria2net"));
    EXPECT_NO_THROW(CLINET_LOG_INFO("client"));  // 注意拼写：CLINET
}

TEST(LogMacroTest, TimerMacrosRunAtScopeExitWithoutThrowing)
{
    EXPECT_NO_THROW({
        LogLimitTimeI();  // 默认阈值 100ms，本用例不超时，不产生输出
        LogTimerI();
    });
}

TEST(LogMacroTest, TimerAlwaysLogsButShortLimitTimerDoesNot)
{
    timerLogCount = 0;
    {
        LogTimer(COUNT_TIMER_LOG);
        LogLimitTime(COUNT_TIMER_LOG, 1000000000);
    }
    EXPECT_EQ(timerLogCount.load(), 1);
}

TEST(LogMacroTest, UniqueTimerEndLogsExactlyOnce)
{
    timerLogCount = 0;
    {
        Log_Unique_Timer(test_timer, COUNT_TIMER_LOG);
        Log_Unique_Timer_END(test_timer);
        EXPECT_EQ(timerLogCount.load(), 1);
    }
    EXPECT_EQ(timerLogCount.load(), 1);
}

TEST(LogMacroTest, UniqueTimerWithExplicitKeyRuns)
{
    EXPECT_NO_THROW({
        Log_Unique_Timer(KEY_Start, LogI);  // 注意：不要用 Log_Unique_TimerI()，它多一个括号
    });
}

namespace
{
// 见文件开头说明：具名 logger 必须在任何用例运行前就存在，否则日志宏会解引用空指针。
// 注册到 null sink，测试期间不产生任何日志文件输出。
class NullLoggerEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        for (const char* name : {"log", "ThreadPool", "SQLite", "Download", "Plugin", "Aria2Net", "Client"})
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
