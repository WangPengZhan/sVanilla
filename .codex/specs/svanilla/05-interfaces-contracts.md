# Interfaces And Contracts

## 插件接口 IPlugin

位置：`ThirdParty/sVanillaPluginCommon/PluginCommon/IPlugin.h`

契约：

| 方法 | 要求 |
| --- | --- |
| `pluginMessage()` | 返回插件 id、名称、版本、描述、domain |
| `websiteIcon()` | 返回站点图标字节数组 |
| `canParseUrl(url)` | 判断插件是否支持该 URL |
| `getVideoView(url)` | 返回解析后的媒体视图列表 |
| `getDownloader(videoInfo)` | 为指定媒体生成真实下载器 |
| `loginer()` | 返回登录代理 |

约束：

- 插件不应假设 UI 线程执行耗时解析。
- `pluginId` 必须稳定，否则历史、Cookie、下载记录会错配。
- `getVideoView` 失败时应可被主程序安全处理。

## BaseVideoView 和 VideoInfoFull

位置：`ThirdParty/sVanillaPluginCommon/PluginCommon/BaseVideoView.h`

核心字段：

- 标识：`Identifier`、`IdType`、`ParentId`、`ParentIdType`
- 展示：`Title`、`Publisher`、`Cover`、`Duration`、`Description`、`PublishDate`
- 下载：`fileExtension`、`fileType`、`pluginId`
- 插件扩展：`Option1`、`Option2`、`Option3`

`VideoInfoFull` 负责组合：

- `DownloadConfig`
- `BaseVideoView`
- 文件名和命名规则解析
- guid、封面路径生成

## 下载器接口 AbstractDownloader

位置：`ThirdParty/sVanillaPluginCommon/Download/AbstractDownloader.h`

状态：

```text
Waiting -> Ready -> Downloading -> Pause/Paused/Resumed -> Finished
                                  -> Stopped
                                  -> Error
```

方法契约：

| 方法 | 要求 |
| --- | --- |
| `start()` | 开始任务 |
| `stop()` | 停止任务 |
| `pause()` | 请求暂停 |
| `resume()` | 恢复任务 |
| `downloadStatus()` | 刷新任务状态和进度 |
| `finish()` | 执行完成逻辑 |

`UiDownloader` 适配规则：

- 包装真实下载器。
- 负责数据库下载中记录创建、更新、删除。
- 负责完成记录创建。
- 通过 signal 通知 UI。

## PluginManager

位置：`sVanilla/src/Plugin/PluginManager.h`

契约：

- `loadPlugins()` 加载插件目录和配置中的插件。
- `unloadPlugins()` 释放插件。
- `addPlugin()` 添加插件路径。
- `getPlugin(id)` 按 id 查询插件。
- `removePlugin(id)` 移除插件。
- `loadConfig()` 和 `saveConfig()` 维护插件配置。

线程规则：

- 插件集合由递归互斥保护。
- 配置变更由原子标记追踪。

## AriaServer

位置：`sVanilla/src/Aria2Net/AriaServer/AriaServer.h`

契约：

- 可异步启动本地 aria2 服务。
- 可关闭或强制关闭服务。
- 可设置错误和关闭回调。
- 可设置日志目录。

## Storage 接口

位置：`sVanilla/src/Sqlite/Storage`

契约：

- `BaseStorage` 提供查询、插入、更新、删除、计数和 distinct 查询。
- `StorageFactory` 在创建 storage 时根据表结构创建表。
- `SqliteDBManager` 负责连接复用、路径设置和初始化。

## ThreadPool

位置：`sVanilla/src/ThreadPool`

契约：

- 支持任意 callable 入队并返回 `std::future`。
- 支持继承 `Task` 的任务对象。
- `stop()` 后不得再接受任务。
- `drop()` 应丢弃待执行任务。

## Zipper

位置：`sVanilla/src/Zipper`

契约：

- `Zipper` 压缩文件列表或目录。
- `Unzipper` 解压文件到目标路径。
- `ResourceHelper` 负责 RAII 清理函数执行。
