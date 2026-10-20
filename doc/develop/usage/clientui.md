# ClientUi 使用

新增页面、取全局服务、安全刷新界面、加业务表。启动流程与服务持有关系见 [`../architecture/clientui.md`](../architecture/clientui.md)。

## 一、取用全局服务

```cpp
auto* plugin = sApp->pluginInterface().getPlugin(pluginId);
sApp->threadPool().enqueue(...);
auto storage = sqlite::StorageManager::instance().searchHistoryStorage();
```

## 二、在工作线程做事并安全更新界面

```cpp
runTask(
    [this] { return computeOnWorkerThread(); },     // 工作线程执行
    this,                                           // 回调在 this 所在线程执行
    [this](auto&& result) { updateUi(result); });   // 安全更新界面
```

## 三、新增一个页面

1. 在 `ClientUi/<Page>/` 新建 `QWidget` 子类（与其 `.ui`）。
2. 在 `MainWindow` 的 `QStackedWidget` 中 `addWidget` 并记下索引，同步更新标题栏按钮与快捷键映射。
3. 需要全局服务时经 `sApp->...` 取用，不要自行 new 服务。

## 四、新增一张业务表

按 [sqlite.md](sqlite.md) 定义实体与 `TABLESTRUCTINFO`，在 `Storage/` 新增 `XxxStorage`，并在 `StorageManager` 中创建表、建索引、暴露访问器。

## 五、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[约束]` | **`main()` 开头就把进程工作目录改成了 exe 目录。** 插件目录 `plugin/`、配置 `config/plugin_config.json`、数据库 `.db/`、日志 `log/` 全部依赖这个前提。任何在 `main()` 之前或之外解析这些相对路径的代码都会失效。 |
| `[约束]` | **不要在非主线程直接操作控件。** 统一走 `runTask`；下载页里已有反面示例（工作线程中调用可能弹 Toast 的路径）。 |
| `[约束]` | **`SApplication::appDir()` 依赖 `QApplication` 已存在，但它在 `QApplication` 构造之前就被调用**（日志与数据库路径的设置更早）。调整启动顺序时不要把它挪到更早的位置。 |
| `[陷阱]` | **`runTask` 的任务对象会在非属主线程被销毁。** 不要给任务对象加依赖线程亲和性的成员。 |
| `[陷阱]` | **`StorageManager::instance()` 可能首次在异步插件线程中被触发**（经 `PluginInterface::setCookiesForPlugins → CookiesInfoStorage`）。首次调用点决定了初始化时机，不要假设它只在主线程发生。 |
| `[陷阱]` | **多处使用函数内 `static` 变量做去重 / 防抖**（`MainWindow::parseUrl` 的 `lastUrl`、各页面的 `parseUrlVersion` 等），属进程级共享状态，隐含"单窗口、单线程访问"的假设。 |
| `[陷阱]` | **`MainWindow::closeEvent` 默认不退出程序。** 托盘可用且开启"最小化到托盘"时执行 `hide()` + `event->ignore()`；真正退出走托盘菜单。 |
| `[陷阱]` | **Debug 构建不安装 Qt 消息处理器**（被 `#ifndef _DEBUG` 包裹），因此 Debug 下 `qDebug` / `qWarning` 不会进入 spdlog。 |
| `[事实]` | **重启功能未接线**：`Restarter` 读取的 `qApp->property("restart")` 全仓库只读不写，重启分支永不触发。 |
