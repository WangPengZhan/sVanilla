# Logger 使用

打日志、给新模块加日志宏。设计原理与宏实现细节见 [`../architecture/logger.md`](../architecture/logger.md)。

## 一、启动时初始化

```cpp
// main()：设置工作目录之后、创建 QApplication 之前
Logger::setLogDir(appDir() + "/");   // 落盘到 <dir>/log/<name>.log
Logger::getInstance();               // 注册全部 9 个 logger，必须最先执行
```

## 二、在模块内打日志

```cpp
#include "Sqlite/SQLiteLog.h"
SQLITE_LOG_INFO("open database: {}", path);
```

通用宏 `LogT/LogD/LogI/LogW/LogE/LogC/LogO` 也可以直接用，但它们**固定写往 `log` 这一个 logger**，不按模块分流。

## 三、单元测试中注册

测试不启动单例，**必须自行注册，否则段错误**。约定做法：注册放在**测试目标的启动环境**里，
即每个目标自带 `main()`，在其中挂一个 gtest `Environment`，由它在第一个用例运行前完成注册。
测试之间不共享 `Common` 目录，脚手架（临时目录等）各自本地化。

```cpp
#include <gtest/gtest.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

class NullLoggerEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        for (const char* name : {"SQLite", "ThreadPool"})
        {
            if (!spdlog::get(name))
            {
                spdlog::register_logger(std::make_shared<spdlog::logger>(name, std::make_shared<spdlog::sinks::null_sink_mt>()));
            }
        }
    }
};

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new NullLoggerEnvironment);
    return RUN_ALL_TESTS();
}
```

链接 `GTest::gtest`（不含 main），不要用 `gtest_main`。

## 四、计时

```cpp
void heavyWork()
{
    LogLimitTime(LogI);          // 只在超过 100 ms 时输出
    LogLimitTime(LogI, 5000000); // 自定义阈值（纳秒）
}
```

## 五、新增一个模块的 logger

1. 把名字加进 `Logger.cpp` 的 `initLog()`（**否则首次调用即段错误**）。
2. 在模块目录新增 `<Module>Log.h`，仿照现有写法定义宏，**logger 名与注册名必须一致**。

## 六、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[陷阱]` | **未注册的 logger 名直接段错误。** 所有日志宏都是「`spdlog::get(name)` 后立即使用」，**中间没有判空**；`spdlog::get` 对未注册名返回空 `shared_ptr`，宏内的 `SPDLOG_LOGGER_CALL` 会直接解引用。这既是"程序必须先初始化 `Logger`"的原因，也是单元测试必须自行注册 logger 的原因。`Logger::get(name)` 同样可能返回 `nullptr`。 |
| `[陷阱]` | **`LogUtil.h` 引用 `Logger::defaultName` 却不包含 `Logger.h`。** 使用通用宏的翻译单元必须自己先 `#include "Logger/Logger.h"`。 |
| `[约束]` | 新增模块 logger 时，注册表（`initLog()`）与模块 `<Module>Log.h` 里的名字要同时改，漏一处就会在运行时段错误。 |
| `[事实]` | `LogTimer` 在作用域退出时无条件输出耗时；`LogLimitTime` 才按阈值输出。`Log_Unique_Timer_END` 调用 `release()`，立即执行回调且析构时不重复执行。 |
