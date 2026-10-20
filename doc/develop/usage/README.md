# 使用文档

各模块的**使用说明**：前置条件、最小可运行示例、调用约定与容易踩的坑。

> 本文档只回答"**怎么用**"。要回答"**怎么设计的**"（定位、依赖、分层、关键抽象、设计取舍）请看 [`../architecture/`](../architecture/README.md)；要回答"**具体怎么实现的**"（类型清单、数据结构、算法与 SQL 细节、边界与错误处理）请看 [`../detailed-design/`](../detailed-design/README.md)。

## 一、上手顺序

1. 先跑通构建与测试：[`../started.md`](../started.md)。
2. 再读下面的「跨模块约定」——它们是多条模块共同的前提，违反会直接崩溃或行为错乱。
3. 之后按需要查具体模块的使用篇。

## 二、模块索引

| 模块 | 何时读它 | 使用文档 |
| --- | --- | --- |
| Base | 需要写「按参数个数分发」的宏 | [base.md](base.md) |
| Logger | 打日志、加新模块的日志宏 | [logger.md](logger.md) |
| ThreadPool | 提交后台任务、把结果送回界面线程 | [threadpool.md](threadpool.md) |
| Zipper | 打包目录 / 解压外部归档 | [zipper.md](zipper.md) |
| BaseQt | 跨线程投递到指定对象、用树结构、UI 工具函数 | [baseqt.md](baseqt.md) |
| SUI | 用现成控件、内嵌浏览器与 JS 互通 | [sui.md](sui.md) |
| Sqlite | 定义业务表、增删改查、取连接 | [sqlite.md](sqlite.md) |
| Aria2Net | 启动 / 关闭本地 aria2 服务 | [aria2net.md](aria2net.md) |
| Download | 接入下载器、理解状态驱动契约 | [download.md](download.md) |
| Plugin | 加载插件、写一个插件动态库 | [plugin.md](plugin.md) |
| ClientUi | 新增页面、取全局服务、安全刷新界面 | [clientui.md](clientui.md) |

SQLite 的设计与实现细节（条件组装、业务表、接新表步骤）见 [`../architecture/sqlite.md`](../architecture/sqlite.md) 与 [`../detailed-design/sqlite.md`](../detailed-design/sqlite.md)。

## 三、跨模块约定

1. **具名 logger 必须先注册。** 各模块日志宏统一写作 `SPDLOG_LOGGER_CALL(spdlog::get("Xxx"), ...)`，`spdlog::get()` 对未注册的名字返回**空 `shared_ptr`**，而宏会直接解引用——**未注册即段错误**。运行时由 `Logger::getInstance()` 注册全部 9 个 logger；单元测试须在**自己的测试启动环境**（gtest `Environment::SetUp`，由目标自带的 `main()` 注册）里注册。测试之间不共享 `Common` 目录。详见 [logger.md](logger.md)。**新增模块 logger 时，必须同时把它加进注册表。**



3. **跨线程更新界面统一走 `runTask`。** 工作线程不能直接操作控件，`ClientUi/Utils/RunTask.h` 把任务投递到线程池、再用 Qt 队列连接把结果送回主线程。直接在工作线程里碰 `QWidget` 是未定义行为。详见 [clientui.md](clientui.md) 与 [threadpool.md](threadpool.md)。

4. **`AbstractDownloader` 与 `IPlugin` 定义在子模块。** 这两个契约类型位于 `ThirdParty/sVanillaPluginCommon`，主仓库只包含其头文件并使用。读 [download.md](download.md) / [plugin.md](plugin.md) 时请以主仓库的**实际调用点**为准。

5. **不要在库代码里用 `std::filesystem::current_path()` 改工作目录**（那样会污染插件目录、配置、日志等一切相对路径的解析，多线程下更是竞态）。需要相对路径时显式传入基准目录。

6. **部分模块链接了并不使用的依赖。** 例如 `BaseQt`、`Zipper` 链接 `spdlog` 但源码未使用；`SUI` 链接了 `spdlog` 且不依赖 `Logger`。判断真实依赖应看源码 `#include`，而非只看 `CMakeLists.txt`。

## 四、构建与测试

见 [`../started.md`](../started.md)。测试目标位于 `test/`，由开关 `ENABLE_TEST` 控制。
