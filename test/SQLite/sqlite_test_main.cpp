// Sqlite 测试的进程入口。
//
// Sqlite 的日志宏写作 SPDLOG_LOGGER_CALL(spdlog::get("SQLite"), ...)：
// spdlog::get() 对未注册的名字返回空 shared_ptr，宏会直接解引用它（段错误）。
// 测试进程不会执行 ClientUi 的 Logger::getInstance()，所以具名 logger 必须
// 在测试环境启动时就注册好；这里统一挂到丢弃输出的 null sink 上。

#include <memory>

#include <gtest/gtest.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

namespace
{
class NullLoggerEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        for (const char* name : {"SQLite", "log"})
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
