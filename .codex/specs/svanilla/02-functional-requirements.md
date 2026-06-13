# Functional Requirements

## FR-001 启动和运行模式

系统应支持 GUI 模式和命令行模式。

证据：

- `ClientUi/main.cpp`
- `ClientUi/CommandLine/CommandLine.h`

验收标准：

- 给定正常 GUI 启动参数，应用创建 `SApplication` 和 `MainWindow`。
- 给定命令行模式参数，应用调用 `execCommandLine` 而不是显示主窗口。
- 给定 URL 参数且 GUI 启动，主窗口应接收 URL 并触发后续解析。

## FR-002 单实例控制

系统在 GUI 模式下应避免多个主窗口实例同时运行。

证据：

- `ClientUi/MainWindow/SingleAppHelper.h`
- `ClientUi/main.cpp`

验收标准：

- 已有 GUI 实例时，新实例应退出。
- 新实例不应破坏已有实例状态。

## FR-003 URL 输入、历史和解析

系统应允许用户输入 URL，记录搜索历史，并请求插件解析为媒体视图列表。

证据：

- `ClientUi/Home/HomePage.h`
- `ClientUi/VideoList/VideoWidget.h`
- `ClientUi/Storage/SearchHistoryStorage.h`

验收标准：

- 输入 URL 后应发出解析请求。
- 可解析 URL 应生成 `adapter::VideoView`。
- 搜索历史最多保留 `SearchHistoryStorage::maxNum` 条。

## FR-004 插件管理

系统应发现、加载、卸载、启用、禁用和配置插件。

证据：

- `sVanilla/src/Plugin/PluginManager.h`
- `sVanilla/src/Plugin/DynamicLibLoader.h`
- `ThirdParty/sVanillaPluginCommon/PluginCommon/IPlugin.h`

验收标准：

- 插件目录不存在时应创建。
- 插件配置应能加载和保存。
- 禁用或无效插件不应参与 URL 解析。
- 插件加载失败不应导致主程序崩溃。

## FR-005 媒体列表展示和筛选

系统应展示解析后的媒体条目，并支持列表/网格展示、排序、搜索和详情查看。

证据：

- `ClientUi/VideoList/VideoWidget.h`
- `ClientUi/VideoList/VideoListWidget.h`
- `ClientUi/VideoList/VideoGridWidget.h`
- `ClientUi/VideoList/VideoInfoWidget.h`

验收标准：

- 解析结果为空时应显示空状态或失败提示。
- 用户可按标题、日期、时长等维度排序。
- 用户可切换展示布局。

## FR-006 下载任务创建和控制

系统应从媒体条目创建下载任务，并支持启动、暂停、恢复、停止、完成和状态更新。

证据：

- `ClientUi/Download/DownloadManager.h`
- `ClientUi/Download/UiDownloader.h`
- `ThirdParty/sVanillaPluginCommon/Download/AbstractDownloader.h`
- `Download/DownloadStatusThread.h`

验收标准：

- 创建下载任务时应生成唯一标识。
- 下载中状态应持久化到 DownloadingItem。
- 下载完成后应删除下载中记录并创建完成记录。
- 状态变化应通过 Qt signal 通知 UI。

## FR-007 下载记录和完成记录

系统应保存下载中和已完成任务，并在启动或页面打开时可查询最近记录。

证据：

- `ClientUi/Storage/DownloadingItemStorage.h`
- `ClientUi/Storage/DownloadedItemStorage.h`

验收标准：

- 下载中记录包含进度、状态、文件路径、封面和来源信息。
- 完成记录包含文件存在性字段。
- 系统可刷新已完成文件是否仍存在。

## FR-008 账号、登录和 Cookie

系统应支持插件提供登录能力，并保存 Cookie 信息。

证据：

- `ThirdParty/sVanillaPluginCommon/PluginCommon/IPlugin.h`
- `ClientUi/Login`
- `ClientUi/Storage/CookiesInfoStorage.h`
- `ClientUi/Setting/AccountsInfo.h`

验收标准：

- 插件可返回 `LoginProxy`。
- Cookie 按 pluginId 和 domain 维度存储。
- Cookie 可插入或更新。

## FR-009 应用设置

系统应保存和读取下载、主题、语言、系统托盘、启动行为、aria2 和视频列表配置。

证据：

- `ClientUi/Config/SingleConfig.h`
- `ClientUi/Utils/Setting.h`
- `ClientUi/Setting`

验收标准：

- 设置读取失败时应使用默认值。
- 修改设置应持久化。
- 主题和语言设置应在 UI 层生效。

## FR-010 本地工具和平台初始化

系统应在启动时初始化网络、日志、崩溃收集、数据库路径、平台特定行为和本地 aria2 服务。

证据：

- `ClientUi/main.cpp`
- `ClientUi/MainWindow/SApplication.h`
- `ClientUi/PlatformInitializer`
- `Aria2Net/AriaServer/AriaServer.h`

验收标准：

- 日志应记录版本、分支、hash、构建时间和 OS。
- 数据库路径应指向应用目录下 `.db`。
- crashpad handler 可用时应初始化崩溃收集。

## FR-011 压缩和解压

系统应提供文件和目录压缩/解压能力。

证据：

- `Zipper/Zipper.h`
- `test/Zipper`

验收标准：

- 可将文件列表或目录打包为 zip。
- 可将 zip 解压到目标目录。
- 资源清理应通过 RAII 辅助对象执行。

## FR-012 线程池和异步任务

系统应提供线程池执行后台任务，并支持返回 future 或通过 Qt signal 返回结果。

证据：

- `ThreadPool/ThreadPool.h`
- `ThreadPool/Task.h`
- `test/ThreadPool`

验收标准：

- 停止后的线程池不应接受新任务。
- 任务执行结果应可通过 future 获取。
- Qt signal 任务应能传递 `std::any` 结果。
