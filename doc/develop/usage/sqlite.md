# Sqlite 使用

定义业务表、增删改查、取连接。分层与设计取舍见 [`../architecture/sqlite.md`](../architecture/sqlite.md)；实现细节、条件组装与拼 SQL 全流程见 [`../detailed-design/sqlite.md`](../detailed-design/sqlite.md)。

## 一、定义实体并注册表结构

实体**必须位于全局命名空间**：

```cpp
struct SearchHistory
{
    std::string url;
    int pluginId;
    int64_t timestamp;
    int searchTimes{};

    int bind(sqlite::SQLiteStatement& stmt) const;            // 顺序与 COLUMN 声明一致
    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0);
};

TABLESTRUCTINFO_BEGIN(SearchHistory)
    TABLESTRUCTINFO_COLUMN(url, url, false, true, true)       // 唯一 + 主键
    TABLESTRUCTINFO_COLUMN(pluginId)
    TABLESTRUCTINFO_COLUMN(timestamp)
    TABLESTRUCTINFO_COLUMN(searchTimes)
TABLESTRUCTINFO_END(SearchHistory)
```

## 二、创建 / 获取连接并建表

```cpp
sqlite::SqliteDBManager::setDbPath(appDir + "/.db");           // 必须在建连接前
auto readDb  = sqlite::SqliteDBManager::createDBWithMutexPtr(sqlite::dbPath + "/data.db");
auto writeDb = sqlite::SqliteDBManager::createDBWithMutexPtr(sqlite::dbPath + "/data.db");

auto& tableStruct = sqlite::TableStructInfo<SearchHistory>::self();
sqlite::SqliteUtil::createTableIfNotExists(writeDb, "SearchHistory", tableStruct);
```

## 三、查询 / 插入 / 更新 / 删除

```cpp
sqlite::ConditionWrapper condition;
condition.addCondition(tableStruct.url, sqlite::Condition::LIKE, std::string("%bilibili%"));

auto items = storage->queryEntities<SearchHistory>(0, 20, condition);   // 分页查询
storage->insertEntities<SearchHistory>({history});
storage->updateEntities<SearchHistory>({history});                     // 按主键更新
storage->deleteEntities(condition);
auto count = storage->countEntities({});
```

## 四、单元测试参考

见 `test/SQLite`，其中包含 SQL 生成与临时数据库测试。测试夹具 `test/SQLite/sqlite_fixture.h` 提供了实体定义与 `TABLESTRUCTINFO` 的可运行写法。

## 五、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[约束]` | **`setDbPath()` 必须在任何连接创建之前调用。** 它是全局变量赋值，而 `init()` 只在**首次**创建连接时执行一次并据此建目录。运行中改 `dbPath` 不会迁移或重建已有连接——`ClientUi` 在 `main()` 早期就完成了这一步。 |
| `[约束]` | **`createDBPtr(path, true)` 不是"取缓存"。** 它每次返回一个**新的独立连接**（缓存里存的是首个实例）。若代码期望"两次调用拿到同一对象"（依赖同一事务上下文或同一把锁），这是错的；那应该用 `writeDBPtr` 或 `*WithMutexPtr`。 |
| `[约束]` | **表结构里同时置 `primaryKey` 与 `autoIncrement` 会建表失败**（生成的双主键被 SQLite 拒绝）。自增主键只置 `autoIncrement`，参照 `SearchHistoryStorage.h`。 |
| `[陷阱]` | **`TABLESTRUCTINFO_*` 宏要求实体类型位于全局命名空间。** 宏会为实体生成 `TableStructInfo<T>` 特化，把实体放进命名空间会导致特化不匹配。 |
| `[陷阱]` | **空 BLOB 与 NULL 不可区分。** 空 `std::vector<uint8_t>{}` 的 `data()` 为 `nullptr`，`sqlite3_bind_blob` 退化为 `bind_null`。要存"长度为 0 的 BLOB"，读回来会是 NULL。 |
| `[陷阱]` | **`originColumnName()` 可能返回 `nullptr`。** `sqlite3_column_origin_name` 依赖库开启 `SQLITE_ENABLE_COLUMN_METADATA`，使用前建议先用 `sqlite3_compileoption_used("SQLITE_ENABLE_COLUMN_METADATA")` 探测。当前 vcpkg 构建已开启，但**表达式列**（`COUNT(1)`、`? AS x`）下该函数仍返回 `nullptr`。 |
| `[陷阱]` | **Windows 下路径编码经 ANSI 代码页转换。** `SqliteDBManager.cpp` 的 `utf8ToLocale` / `localeToUtf8` 使用 `GetACP()`，路径含超出本机代码页的字符时可能打不开数据库。 |
| `[约束]` | **绕开 `SqliteUtil` 自己拼 SQL 时，必须自己取锁。** 正确写法是 `std::lock_guard lk(db->mutex);`（`recursive_mutex`，可重入）。 |
| `[事实]` | **同列同向的范围条件会自动归并。** 例：`x > 10 AND x >= 30` 合并成 `x >= 30`，保留等于边界的行。 |
| `[事实]` | `conditionString()` 对 `IS NULL` / `IS NOT NULL` 输出 SQL 字面量 `NULL`（仅用于日志 / 调试渲染；实际查询走占位符绑定），由用例 `SqliteTest.NullConditionsRenderLiteralNull` 锁定。 |
