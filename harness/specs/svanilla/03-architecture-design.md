# Architecture Design

## 架构风格

sVanilla 采用桌面客户端分层架构：

1. UI 层：Qt Widgets 页面、窗口、控件和用户交互。
2. 应用协调层：`SApplication`、`MainWindow`、`PluginInterface`、下载管理。
3. 领域能力层：插件、下载、SQLite、线程池、压缩、日志。
4. 第三方和工具层：Qt、aria2、ffmpeg、crashpad、vcpkg 依赖、插件公共库。

## 主要组件

| 组件 | 位置 | 职责 |
| --- | --- | --- |
| MainWindow | `ClientUi/MainWindow` | 主窗口、导航、主题、托盘、URL 解析入口 |
| SApplication | `ClientUi/MainWindow/SApplication.h` | 应用级服务持有者，管理 aria2、插件、翻译、线程池、下载状态线程 |
| HomePage | `ClientUi/Home` | URL 输入、历史菜单、登录入口 |
| VideoWidget | `ClientUi/VideoList` | 解析结果展示、排序、搜索、批量创建下载 |
| DownloadWidget | `ClientUi/Download` | 下载中和已完成任务 UI |
| PluginManager | `Plugin` | 插件目录、配置、动态库加载和插件实例管理 |
| UiDownloader | `ClientUi/Download` | UI 和真实下载器之间的适配器，负责状态持久化 |
| SqliteDBManager | `Sqlite/Storage` | 数据库路径、连接和初始化 |
| Storage classes | `ClientUi/Storage` | 下载记录、Cookie、历史记录持久化 |
| AriaServer | `Aria2Net` | 本地 aria2 进程生命周期管理 |
| ThreadPool | `ThreadPool` | 后台任务执行 |
| Zipper | `Zipper` | 压缩和解压 |

## 依赖方向

期望依赖方向：

```text
ClientUi
  -> Plugin / Download / Aria2Net / Sqlite / ThreadPool / Zipper / Logger / SUI / BaseQt
  -> ThirdParty/sVanillaPluginCommon

Plugin
  -> ThirdParty/sVanillaPluginCommon

Download
  -> ThirdParty/sVanillaPluginCommon

Sqlite, ThreadPool, Zipper, Logger
  -> third-party libraries only
```

约束：

- `ThirdParty` 不应依赖 `ClientUi`。
- 插件公共接口修改会影响主程序和插件实现，必须评审。
- UI 层可以组合服务，但核心库不应反向依赖 UI 页面。

## 生命周期

启动顺序由 `main.cpp` 协调：

1. 初始化 curl 全局对象。
2. 设置工作目录、日志目录、dump 目录和数据库路径。
3. 初始化 crashpad。
4. 创建 `Restarter`。
5. 执行平台初始化前置步骤。
6. 创建 `SApplication`。
7. 解析命令行。
8. GUI 模式下检查单实例。
9. 初始化应用服务。
10. 显示主窗口或执行命令行逻辑。
11. 根据退出码决定重启或退出。

## 架构风险

| 风险 | 影响 | 处理规则 |
| --- | --- | --- |
| `ClientUi` 体量大 | UI、状态和业务逻辑容易耦合 | 修改前先定位页面和服务边界 |
| 插件接口跨仓库 | 可能破坏插件兼容 | 修改 `IPlugin`、`BaseVideoView` 前必须更新插件规格 |
| 构建依赖重 | 本地验证可能受 Qt/vcpkg 影响 | 记录无法运行的命令和阻塞原因 |
| 平台差异 | 路径、动态库、进程、打包行为不同 | 修改工具和路径逻辑时检查 Windows/Linux/macOS |
