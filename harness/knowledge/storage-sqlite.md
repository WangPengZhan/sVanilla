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
- 并发保护挂在连接对象（`SqliteDbWithMutex::mutex`）上，且只在 `SqliteUtil` 的入口内部加锁。**业务存储里自己拼 SQL 时必须显式 `std::lock_guard lk(db->mutex)`**，否则同一连接会被多线程并发使用；现有 `DownloadedItemStorage::isDownload()`、`DownloadingItemStorage::isDownload()` 就漏了这一步。
- `updateEntities<Entity>` 的 WHERE 完全来自主键列；没有 `primaryKey` / `autoIncrement` 列时接口返回 0，不执行更新。`DownloadedItem`、`DownloadingItem` 两个实体只有 UNIQUE，没有主键列；`BaseStorage::deleteEntities()` 不传条件时仍会生成 `DELETE FROM <t>`（清空整表）。
- 当前实现对没有主键列的实体批量更新返回 0；搜索历史只更新必要列，避免把触发器维护的 `searchTimes` 写回默认值。`deleteEntities` 绑定条件参数，视图条件中的字符串采用 SQL 单引号转义。
- `createViewIfNotExist` 的条件必须展开成 SQL 文本（SQLite 不允许视图定义保留绑定参数）；字符串字面量中的单引号必须加倍。
- 条件归并（`ConditionWrapper::mergeOptimization`）在 GE 与 GT 混用且新边界更严时保留新条件的操作符：`x > 10 AND x >= 30` 归并成 `x >= 30`。由 `SqliteTest.RangeConditionMixKeepsTighterValueAndItsOperator` 验证。
- 模块设计文档分三级：`doc/develop/architecture/`（架构设计）、`doc/develop/detailed-design/`（详细设计）、`doc/develop/usage/`（使用）。详细设计以 `detailed-design/sqlite.md` 为样板；架构级风险见架构文档的 `R1`…。

## 证据

- 需要结合实际任务读取 `sVanilla/src/Sqlite` 和 ClientUi 存储代码。
- 上述结论的实现级证据、SQL 模板与测试映射见 `doc/develop/detailed-design/sqlite.md`；测试位于 `test/SQLite`。
