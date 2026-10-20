# Plugin 使用

加载插件、取用插件、写一个插件动态库。设计原理与异常隔离机制见 [`../architecture/plugin.md`](../architecture/plugin.md)。

## 一、初始化与加载（主程序）

```cpp
plugin::PluginManager::setPluginConfigDir(appDir);   // 决定 config/ 路径
plugin::PluginManager manager;                       // 构造时建目录 + 读配置 + 收集插件路径
manager.loadPlugins();                               // 逐个 addPlugin
```

## 二、运行时取用插件

```cpp
auto plugin = manager.getPlugin(pluginId);           // 返回 shared_ptr<IPlugin>，可能为空
if (plugin)
{
    auto view = plugin->getVideoView(url);           // 异常已被 PluginProxy 兜住
}
```

`SApplication` 用 `QFileSystemWatcher` 监视 `plugin/` 目录，新放入的动态库经 `pluginDirFileAdded()` 触发热加载。见 [clientui.md](clientui.md)。

## 三、写一个插件动态库

需导出 4 个符号，并保证与主程序**同一套工具链 / 运行库**：

```cpp
extern "C" {
    void initDir(const char* dir) { /* 记录配置目录 */ }
    plugin::IPlugin* pluginInit() { return new MyPlugin; }
    void pluginDeinit(plugin::IPlugin* p) { delete p; }
    void deinit() { /* 清理 */ }
}
```

## 四、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[约束]` | **`loadPlugins()` 之前必须先 `setPluginConfigDir(...)`**；未设置时配置路径退化为相对 CWD 的 `config/plugin_config.json`。 |
| `[约束]` | **新增插件时 `id` 必须全局唯一。** `PluginConfig` 的相等与哈希只看 `id`，同 `id` 的不同插件会被判等并互相覆盖；而 `addPlugin` 更新配置时按 `name` 匹配——两套键不一致，务必保证两者都不冲突。 |
| `[约束]` | 插件与主程序**必须同工具链编译**。边界用 C ABI 函数（`initDir` / `pluginInit` / `pluginDeinit` / `deinit`），但 `IPlugin` 是 C++ 虚接口，STL 布局差异会直接导致崩溃。 |
| `[陷阱]` | **插件目录名与配置文件路径都是相对路径**，依赖进程当前工作目录。`pluginDirHaving()` 直接构造 `std::filesystem::directory_iterator(相对路径)` 且**没有异常保护**，目录不存在时会抛异常。 |
| `[陷阱]` | **只有 `enabled == true` 的插件才真正进入运行表。** 未启用的插件仍会出现在配置里（设置页需要展示），但不会驻留 `PluginManager` 的句柄表与插件表。 |
| `[陷阱]` | **`addPlugin` 更新已存在的同名配置时不会置 `m_configChanged`**，可能导致这次修改不落盘（走设置页的 `setPluginConfig` 路径是安全的）。 |
| `[陷阱]` | **同名重载容易看错。** `DynamicLibLoader` 同时有实例方法 `loadLibrary()` 与静态 `loadLibrary(const std::string&)`（`unloadLibrary` 同理），实例方法内部调用的其实是**静态版**。 |
| `[陷阱]` | **Windows 路径编码用 ANSI 代码页而非 UTF-8。** 内部 `stringToWideString` / `utf16ToString` 使用 `GetACP()`，路径含超出本机代码页的字符时加载会失败。 |
| `[事实]` | **符号缺失时的错误信息不会打印**（Windows 分支只 `LocalFree`），「符号缺失」表现为加载失败但无具体原因。排查插件加载失败时，建议自行用工具确认导出符号是否齐全。 |
