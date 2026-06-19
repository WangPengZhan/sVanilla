# Data And Storage Specification

## 配置存储

配置入口：

- `ClientUi/Config/SingleConfig.h`
- `ClientUi/Utils/Setting.h`

配置项：

| 配置 | 字段 | 用途 |
| --- | --- | --- |
| Aria2Config | `url`、`token`、`port`、`isRemote`、`enableAdvancedSetting` | aria2 连接和高级配置 |
| SystemTrayConfig | `enable`、`minimize` | 系统托盘行为 |
| StartUpConfig | `autoStart`、`keepMainWindow`、`autoRemuseUnfinishedTask` | 启动行为 |
| VideoWidgetConfig | `order`、`orderBy`、`isNoParseList`、`widgetLayout` | 视频列表排序和布局 |
| DownloadConfig | `downloadDir`、`videoQuality`、`nameRule` | 下载目录、质量、命名规则 |

规则：

- 配置读写应通过 `SingleConfig` 或 `Settings` 封装。
- `CustomSettings` 使用共享互斥保护读写。
- 新增配置必须给出默认值和迁移策略。

## SQLite 数据库

入口：

- `Sqlite/Storage/SqliteDBManager.h`
- `Sqlite/Storage/BaseStorage.h`
- `Sqlite/Storage/StorageFactory.h`

数据库路径：

- Confirmed：启动时设置为应用目录下 `.db`。

表创建：

- `StorageFactory::createStorage` 根据实体的 `TABLESTRUCTINFO` 自动创建表。

## DownloadingItem

位置：`ClientUi/Storage/DownloadingItemStorage.h`

用途：记录下载中任务。

主键：

- `uniqueId`

字段：

| 字段 | 含义 |
| --- | --- |
| `uniqueId` | 下载任务唯一标识 |
| `pluginId` | 来源插件 |
| `filePath` | 目标文件路径 |
| `coverPath` | 封面路径 |
| `id`、`idType` | 源站内容标识 |
| `title`、`author`、`url` | 展示和来源信息 |
| `option1`、`option2`、`option3` | 插件扩展选项 |
| `progress` | 下载进度 |
| `duration` | 时长 |
| `status` | 下载状态 |
| `fileType` | 文件类型 |
| `fileExtension` | 扩展名 |

## DownloadedItem

位置：`ClientUi/Storage/DownloadedItemStorage.h`

用途：记录已完成任务。

主键：

- `uniqueId`

额外字段：

- `fileExist`：标记本地文件是否仍存在。

规则：

- 完成任务应从 DownloadingItem 转移到 DownloadedItem。
- 文件存在性可通过 `updateFileExist` 刷新。

## CookiesInfo

位置：`ClientUi/Storage/CookiesInfoStorage.h`

用途：保存插件登录 Cookie。

联合唯一维度：

- `pluginId`
- `domain`

字段：

- `updateTimestamp`
- `cookie`
- `expires`
- `storedata`

规则：

- 插入同一 pluginId 和 domain 时应更新已有记录。
- 登录相关逻辑不应直接绕过 storage 层写数据库。

## SearchHistory

位置：`ClientUi/Storage/SearchHistoryStorage.h`

用途：保存用户搜索 URL 历史。

主键：

- `url`

字段：

- `pluginId`
- `timestamp`
- `searchTimes`

规则：

- 最大记录数为 `maxNum = 20`。
- 重复 URL 应更新次数或时间，而不是无限新增。
