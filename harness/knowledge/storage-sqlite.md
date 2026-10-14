# Storage And SQLite Knowledge

## 模块范围

- 本地配置。
- SQLite 封装。
- Cookie、插件状态、下载记录等持久化。

## 已知规则

- 新增持久化字段必须说明默认值、升级行为和失败处理。
- 写入失败时不能让 UI 误以为操作成功。
- 不要改变已有 key 或唯一维度，除非提供迁移策略。
- `SqliteDBManager::createDBPtr()` 和 `createDBWithMutexPtr()` 在 `createNew=true` 时返回新建且已初始化的独立连接；`createNew=false` 时返回对应的缓存默认写连接。
- `writeDBPtr()` 和 `writeDBWithMutexPtr()` 按规范化后的绝对路径复用默认写连接。Storage 应显式使用缓存写连接，并按需创建独立读连接。

## 证据

- 需要结合实际任务读取 `sVanilla/src/Sqlite` 和 ClientUi 存储代码。
