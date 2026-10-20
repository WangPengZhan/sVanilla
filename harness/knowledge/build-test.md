# Build And Test Knowledge

## 模块范围

- CMake。
- vcpkg。
- GoogleTest。
- CI。

## 已知规则

- 测试开关为 `ENABLE_TEST`。
- macOS CI 使用 Qt 5.15.2；将信号连接到 `std::function` 回调时，应显式传入 `QObject` 上下文，避免 Qt 5 的三参数模板重载推导失败。
- Qt 5.15 的 `QProcess::finished` 有重载；连接带退出码和退出状态的信号时，应显式指定成员函数指针类型，以便 Qt 5 和 Qt 6 都能编译。
- Harness/AI 使用独立的 `out/harness/` 构建树，避免覆盖或依赖开发者的 `build/`。
- 测试按模块归属维护：`test/Base`、`test/Logger`、`test/BaseQt`、`test/SQLite`、`test/ThreadPool`、`test/Zipper`、`test/Download`、`test/Plugin`、`test/Aria2Net`、`test/SUI`、`test/ClientUi`。
- UI 任务无法自动化时，应至少构建相关 target 并提供手工验收步骤。

## 常用命令

```sh
cmake -B out/harness -S . -DCMAKE_BUILD_TYPE=Debug -DENABLE_TEST=ON
cmake --build out/harness --config Debug --parallel
ctest --test-dir out/harness --build-config Debug --output-on-failure
```


## 2026-10-14 主仓库测试补齐

当时补齐了 SQLite、ThreadPool 和 Zipper 的回归覆盖，并增加了独立审计回归目标。审计回归目标现已移除；下载器断言归入 `Download_test`，插件代理断言归入 `Plugin_test`。

三个必须先解决的测试基建陷阱：

1. **未注册的具名 logger 等于段错误。** 各模块日志宏统一写作 `SPDLOG_LOGGER_CALL(spdlog::get("Xxx"), ...)`，spdlog 对未注册名字返回空 `shared_ptr`，宏会直接解引用它。原 `ThreadPool_test` 的段错误即由此产生。注册统一放在**测试目标的启动环境**里：目标自带 `main()`，在其中 `::testing::AddGlobalTestEnvironment(new NullLoggerEnvironment)`，由 `Environment::SetUp()` 把本模块用到的 logger 挂到 `null_sink` 上。因此这类目标链接 `GTest::gtest`（不含 main），不能再用 `gtest_main`。
2. **`TEST_F` 不接受命名空间限定的夹具名。** GTest 把夹具名拼进生成类名，`TEST_F(ns::Fixture, X)` 会报 C2039/C3083。用例必须与夹具处于同一命名空间。
3. **`TABLESTRUCTINFO_COLUMN` 的 `primaryKey` 与 `autoIncrement` 不能同时置真。** 否则 `createSql()` 同时产出列级 `INTEGER PRIMARY KEY AUTOINCREMENT` 与表级 `PRIMARY KEY(id)`，SQLite 报 multiple primary key。参照 `SearchHistoryStorage.h`，只置 `autoIncrement`。

构建树调用约定：本机 `build/` 树的 `cmake --build --target <裸目标名>` 会失败（MSB1009，裸名被当作文件名传给 MSBuild），必须使用相对路径形式，如 `--target test/SQLite/SQLite_test`。

已知的封装语义（已用用例固化，未修改实现）：

- 空 `std::vector<uint8_t>{}` 的 `data() == nullptr`，使 `sqlite3_bind_blob` 退化为 `bind_null`，该封装下「空 BLOB」与「NULL」不可区分。
- `ThreadPool(0)` 现在退化为 1 个工作线程，避免任务静默积压；默认构造在 `hardware_concurrency()` 返回 0 时也适用。
- 范围条件合并非对称：`newPrepareInfo = std::move(prepareInfo)` 会覆盖任一方向，先加入的边界被替换。
- `Zipper::zip()` 创建目录迭代器时使用带 `error_code` 的重载；源目录不存在时返回 `false`，由 `ZipperTest.ReturnsFalseForMissingSourceDirectory` 覆盖。循环中的 `operator++`、`absolute()` 和 `relative()` 仍可能抛 `filesystem_error`。

`Zipper::collectFileInDirToZip` 不再修改进程全局工作目录，而是以 `baseDir` 计算相对路径；递归遍历使用 `error_code` 并累计条目写入结果。`zip()` 在文件列表非空时逐项打包，否则按目录遍历。`Zipper`/`Unzipper` 目前在主仓库中无生产调用方。

临时目录脚手架不再集中到 `Common`（该目录已删除）：需要临时目录的测试在**自己的匿名 namespace** 里定义 `ScopedTempDir`。目录名必须是「进程级 runId + 递增序号」：只写进程内序号的话，两次运行会复用同一目录，而 `SqliteDBManager` 的连接缓存直到进程退出才释放，上一轮残留的 `.db`（含已建好的表）在 Windows 上删不掉，会让下一轮的 `CREATE TABLE` 直接失败（表现为**隔一次跑就红**的偶发用例）。

需要临时目录的测试在**自己的匿名 namespace** 里定义 `ScopedTempDir`（`%TEMP%` 下独占目录，析构 `remove_all`，可连带清除 SQLite 的 `-wal`/`-shm`），`readAll()` 同理。`test/SQLite/sqlite_fixture.h` 里放的是 `sqlite_test::ScopedTempDir`（仅 Sqlite 测试内部共用）。声明顺序须保证数据库句柄早于 `ScopedTempDir` 销毁，否则 Windows 上会因文件占用删除失败。

## 2026-10-14 覆盖全部主仓库模块

当时将单元测试扩展到 Base、Logger、BaseQt、Download、Plugin、Aria2Net、SUI 和 ClientUi 等模块；这些模块的测试现在分别维护在对应目录。

测试文档不汇总源码中的用例总数。下载异常边界断言归 `Download_test`，插件代理异常隔离断言归 `Plugin_test`。

### 测试源码编码

测试源码统一是 UTF-8（含中文注释）。MSVC 默认按系统代码页 936 解析，遇到「中文字符 + 行尾换行」时会把换行当作双字节字符的尾字节吞掉，直接把下一行并进注释，产生大量莫名的 `C2447`。`test/CMakeLists.txt` 现在统一加 `add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)`。

注意：`spdlog` 目标用 `target_compile_options(spdlog PUBLIC "/utf-8")` 传递 `/utf-8`，所以链接 spdlog 的目标本来就正常；只有**不链接 spdlog 的测试目标**（如 `Base_test`）才会踩到。统一声明后重复指定 `/utf-8` 是安全的，但不要再写 `/source-charset:utf-8` —— 它与 `/utf-8` 共存会报 `D8016`。

### 需要 Qt 的测试目标

`BaseQt_test` / `SUI_test` / `Aria2Net_test` 自带 `main()`（链接 `GTest::gtest` 而非 `gtest_main`），在其中 `qputenv("QT_QPA_PLATFORM", "offscreen")` 之后再构造 `QApplication`；同时在 CMake 里给 `add_test` 设置 `ENVIRONMENT "QT_QPA_PLATFORM=offscreen"`。`util::binToImage`、`util::createMenu`、`QWidget::grab()` 都需要 QGuiApplication。

`SUI_test` 不链接整个 `SUI` 静态库（它会带出 Qt WebEngine），而是把用到的 `DownloadCount.cpp` / `SlidingWaitBar.cpp` 直接编进测试——CMake 对静态库的 PRIVATE 依赖仍会作为链接需求传给消费方，绕不开。

`Aria2Net_test` 必须额外链接 `BaseQt`：`Aria2Server.cpp` 用了 `TransceiverForQt`，但 `Aria2Net/CMakeLists.txt` 没有声明这个依赖。

### 依赖 CWD 的模块的测法

`PluginManager` 的构造函数会 `create_directory("plugin/")` 并按相对路径读写 `config/plugin_config.json`，直接构造会在仓库里留下目录。测法：先把 `std::filesystem::current_path()` 切到 `ScopedTempDir`，构造/析构完成后再切回，并在用例结束时 `setPluginConfigDir("")` 复位静态状态。`ScopedTempDir` 会在析构时清理，因此不能残留。

### 已知缺陷速查（只列当前仍然存在）

- `plugin::PluginConfig` 的 `int id;` 是结构体里唯一没有默认成员初始化器的字段（`enabled`/`isValid` 都有），默认构造后是未定义值。需要确定性默认值时应经 JSON 反序列化（`WITH_DEFAULT` 会先构造默认对象）。
- `ClientUi/Utils/SortItems.h` 的 `sortInitialsItems()` 把提取函数类型写作 `std::function<QueryType(Sentence)>`，两个模板参数都在非推导上下文里，**传 lambda 无法推导**，必须传真正的 `std::function` 对象。该工具在主仓库中没有调用点。
- `AriaServer` 的 `testServer()` 是空实现；`closeServer()` 与 `forceCloseServer()` **实现完全相同**（都直接 `kill()`，没有"强制"语义差异）。`m_closeFunc` 已连到 `QProcess::finished`，但 `setCloseFunc()` 仍无调用点。
- `SUI::DownloadCount::sizeHint()` 被声明为 `protected`，测试中需通过 `static_cast<QWidget&>(widget).sizeHint()` 访问。

### vcpkg / CMake 不可用时的验证替代方案

`cmake --build` 触发的 vcpkg manifest install 会因系统策略禁止 `reg.exe` 失败，并清空 `out/harness/vcpkg_installed`。此时改用 `cl.exe` 直接编译单个 TU（需要的话先跑 `moc.exe` / `uic.exe`）做语法校验，或手工链接迷你测试可执行文件跑用例；已验证可行的套件与坑位见 [`../ai-experience/2026-10-15-offline-single-tu-verification.md`](../ai-experience/2026-10-15-offline-single-tu-verification.md)。要点：本机 `System32` 缺 `api-ms-win-crt-*.dll`，必须用 `/MT` 静态链接 CRT，并在 PowerShell 中运行（Git Bash 的加载器找不到 exe 同目录的 DLL）。

## 2026-10-16 计时宏回归

`scope_guard::release()` 立即执行回调并标记已释放，析构时不再重复执行。`Log_Unique_Timer_END` 应调用 `release()`，不能显式调用栈对象的析构函数。`LogTimer` 应分发到无阈值的 `LogTimer_1`；`LogLimitTime` 才使用时间阈值。`test/Logger` 分别验证释放一次和两种宏的输出次数。
