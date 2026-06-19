# Runtime Flows

## 启动流程

```text
main
  -> initialize curl global
  -> set current dir, log dir, dump dir, db path
  -> initialize crashpad
  -> create Restarter
  -> AppInitializer::initApplicationBefore
  -> create SApplication
  -> parseCommandLineOption
  -> GUI: SingleAppHelper check
  -> AppInitializer::init
  -> SApplication::init
  -> GUI: MainWindow show
  -> CLI: execCommandLine
  -> restartOrExit
```

关键要求：

- 日志记录版本、分支、hash、构建时间、工作目录、应用目录和 OS。
- GUI 模式下应做单实例检查。
- 数据库路径必须在 storage 使用前设置。

## URL 解析流程

```text
HomePage/MainWindow receives URL
  -> MainWindow::parseUrl or VideoWidget::searchUrl
  -> PluginInterface/PluginManager selects plugin
  -> IPlugin::canParseUrl
  -> IPlugin::getVideoView
  -> VideoWidget::searchedVideoItem
  -> list/grid widgets show items
  -> SearchHistoryStorage records history
```

失败处理：

- 没有插件可解析时 UI 应提示失败或保持空状态。
- 插件解析异常不应中断主程序。

## 下载流程

```text
User selects media item
  -> VideoInfoFull composed
  -> IPlugin::getDownloader
  -> UiDownloader wraps real downloader
  -> DownloadManager::addItem
  -> UiDownloader::start
  -> createDbDownloadingItem
  -> DownloadStatusThread polls status
  -> UiDownloader emits update/statusChanged
  -> on finish: delete downloading item, create downloaded item
```

状态要求：

- 下载状态应以 `AbstractDownloader::Status` 为基准。
- UI 展示状态不得和数据库状态长期不一致。
- 完成任务必须具备可追踪的 `uniqueId`。
- `DownloadStatusThread` 仅在复制任务快照和提交任务删除时持有任务集合锁；状态查询、下载器控制、持久化和信号通知必须在锁外执行。

## 登录和 Cookie 流程

```text
Plugin provides LoginProxy
  -> UI opens LoginDialog/LoginWebDialog/SetCookieDialog
  -> login succeeds or user sets cookie
  -> CookiesInfoStorage::insertOrUpdate
  -> plugin later uses stored cookie
```

约束：

- Cookie 按插件和 domain 隔离。
- Cookie 更新应保留更新时间。

## 插件加载流程

```text
SApplication owns PluginInterface/PluginManager
  -> PluginManager::loadConfig
  -> create plugin directory if needed
  -> discover dynamic library files
  -> DynamicLibLoader::loadLibrary
  -> DynamicLibLoader::loadPluginSymbol
  -> PluginProxy wraps IPlugin
  -> enabled valid plugins become available
```

约束：

- 动态库扩展名平台相关。
- 插件配置保存后应可在下次启动恢复。

## 设置变更流程

```text
Settings page changes value
  -> SingleConfig setter
  -> Settings/CustomSettings writes QSettings
  -> related service or UI applies value
```

影响面：

- 主题、语言和模糊效果影响 UI。
- 下载目录、质量、命名规则影响新建下载任务。
- aria2 设置影响下载服务连接。

## 退出流程

```text
MainWindow closeEvent
  -> system tray policy may hide instead of exit
  -> application event loop exits
  -> Restarter decides restart or exit
  -> logs exit time and code
```

约束：

- 退出时应释放 aria2、插件、线程和下载状态线程资源。
## 2026-06-19 Async Plugin Loading Flow

```text
SApplication::init
  -> set plugin loading flag
  -> std::async(std::launch::async) loads plugins
  -> PluginInterface sets cookies from snapshot
  -> clear plugin loading flag
  -> emit pluginsLoaded
```

- URL parsing and login-plugin UI must handle the async loading state before reading plugin snapshots.
- Plugin list iteration must use `PluginManager::pluginsSnapshot()`.
- `SApplication` shutdown must consume the plugin loading future with `get()` before `PluginInterface` is destroyed.
- `pluginsLoaded(bool succeeded)` tells the main window whether initial plugin loading completed successfully so the UI can show success or failure feedback.
## 2026-06-19 CLI Plugin Loading Flow

```text
main
  -> SApplication::init
  -> GUI branch: continue with MainWindow after async plugin loading starts
  -> CLI branch: waitForPluginLoadTask
  -> execCommandLine
```

- CLI 模式在调用 `execCommandLine()` 前必须等待 `SApplication::waitForPluginLoadTask()` 完成。

## 2026-06-19 CLI Help and Version Fast Exit

```text
main
  -> parseCommandLineOption
  -> print help/version
  -> exit before SApplication::init
```

- `-h` / `--help` / `-v` / `--version` should exit before `SApplication::init()` so Windows CLI output returns immediately and does not start plugin loading or background threads.
