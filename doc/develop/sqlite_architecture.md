# SQLite 存储设计

本文档说明 `sVanilla/src/Sqlite` 的设计架构、优点和使用方式。该模块不是完整 ORM，而是面向项目业务的轻量 SQLite 封装：底层管理连接和语句，中间层负责 SQL 生成，业务层以 `Storage` 形式暴露读写接口。

## 一、总体架构

```mermaid
flowchart TD
    App[业务代码 / UI 模块] --> StorageManager[ClientUi::StorageManager]
    StorageManager --> BizStorage[业务 Storage<br/>DownloadedItemStorage<br/>DownloadingItemStorage<br/>SearchHistoryStorage<br/>CookiesInfoStorage]
    BizStorage --> BaseStorage[sqlite::BaseStorage]
    BaseStorage --> SqliteUtil[sqlite::SqliteUtil]
    SqliteUtil --> Composer[SqlComposer<br/>TableStructInfo<br/>ConditionWrapper<br/>ColumnInfo]
    SqliteUtil --> Statement[SQLiteStatement]
    Statement --> Database[SQLiteDatabase / SqliteDbWithMutex]
    Database --> Native[(sqlite3)]

    StorageManager --> DBManager[SqliteDBManager]
    DBManager --> Database
```

### 模块依赖图

```mermaid
flowchart LR
    subgraph ClientUi["ClientUi 业务层"]
        UI[UI / Downloader / Setting]
        SM[StorageManager]
        DS[DownloadedItemStorage]
        DLS[DownloadingItemStorage]
        SHS[SearchHistoryStorage]
        CIS[CookiesInfoStorage]
    end

    subgraph Storage["Sqlite::Storage"]
        BStorage[BaseStorage]
        DBM[SqliteDBManager]
        Factory[StorageFactory]
    end

    subgraph Composer["Sqlite::SqlComposer"]
        TSI[TableStructInfo]
        CI[ColumnInfo]
        Cond[ConditionWrapper]
        Util[SqliteUtil]
    end

    subgraph DatabaseLayer["Sqlite::Database"]
        DB[SQLiteDatabase]
        DBMtx[SqliteDbWithMutex]
        Stmt[SQLiteStatement]
        Col[SqliteColumn]
        Ex[SQLiteException]
    end

    UI --> SM
    SM --> DS
    SM --> DLS
    SM --> SHS
    SM --> CIS
    DS --> BStorage
    DLS --> BStorage
    SHS --> BStorage
    CIS --> BStorage
    BStorage --> Util
    Factory --> Util
    DBM --> DBMtx
    Util --> TSI
    Util --> Cond
    TSI --> CI
    Util --> Stmt
    Cond --> Stmt
    Stmt --> DB
    DBMtx --> DB
    Stmt --> Col
    DB --> Ex
```

模块分为三层：

| 层级 | 主要文件 | 职责 |
| --- | --- | --- |
| Database | `Database/SQLiteDatabase.*`、`SQLiteStatement.*`、`SQLiteColumn.*`、`SQLiteException.*` | 封装 `sqlite3` 连接、预编译语句、列值读取、错误信息和事务 |
| SqlComposer | `SqlComposer/BaseInfo.*`、`ConditionWrapper.*`、`SqlUtil.*` | 根据实体元信息生成建表、插入、更新、查询、条件和索引 SQL |
| Storage | `Storage/BaseStorage.*`、`SqliteDBManager.*`、`StorageFactory.*` | 管理数据库连接，提供按业务表读写的统一入口 |

### 调用方向

```mermaid
flowchart TB
    A[业务代码] --> B[业务 Storage]
    B --> C[BaseStorage]
    C --> D[SqliteUtil]
    D --> E[SQLiteStatement]
    E --> F[SQLiteDatabase]
    F --> G[(sqlite3)]

    H[Entity 元信息] --> D
    I[ConditionWrapper] --> D

    G -.结果行.-> E
    E -.SqliteColumn.-> J[Entity::setValue]
    J -.vector Entity.-> A
```

调用方向基本保持单向：业务层依赖 Storage，Storage 依赖 SQL 工具和数据库封装，底层不反向依赖业务。

## 目录结构

```text
sVanilla/src/Sqlite
|-- CMakeLists.txt
|-- SQLiteLog.h
|-- Database/
|   |-- SQLiteDatabase.h/.cpp      # 数据库连接、事务、错误信息
|   |-- SQLiteStatement.h/.cpp     # prepare/bind/step/reset
|   |-- SQLiteColumn.h/.cpp        # 查询结果列的类型和值
|   |-- SQLiteException.h/.cpp     # sqlite 错误异常
|-- SqlComposer/
|   |-- BaseInfo.h/.cpp            # 表结构元信息、字段元信息、宏注册
|   |-- ConditionWrapper.h/.cpp    # WHERE 条件构造与参数绑定
|   |-- SqlUtil.h/.cpp             # CRUD、建表、索引、视图等工具
|   |-- JoinWrapper.h              # join 信息结构，当前预留较轻
|-- Storage/
|   |-- BaseStorage.h/.cpp         # 业务 Storage 基类
|   |-- SqliteDBManager.h/.cpp     # 连接缓存、DB 初始化、WAL 配置
|   |-- StorageFactory.h/.cpp      # 泛型 Storage 创建辅助
```

## 类关系图

```mermaid
classDiagram
    class SQLiteDatabase {
        +execute(sql) bool
        +transaction() bool
        +commit() bool
        +rollback() bool
        +tables() set
        +tableExists(name) bool
        +lastInsertRowid() int64
        +handle() sqlite3*
    }

    class SqliteDbWithMutex {
        +recursive_mutex mutex
    }

    class SQLiteStatement {
        +bind(index, value)
        +bind(name, value)
        +executeStep() bool
        +reset() bool
        +column(index) SqliteColumn
        +expandedSQL() string
    }

    class SqliteColumn {
        +getInt() int32
        +getInt64() int64
        +getDouble() double
        +getText(defaultValue) string
        +getBlob() vector~uint8_t~
        +getType() FieldType
    }

    class BaseTableStructInfo {
        +createSql() string
        +insertPrepareSql() string
        +updatePrepareSql() string
        +primaryKeyPrepareSql() string
    }

    class ColumnInfo {
        +columnSql() string
        +columnName() string
        +valueTypeIndex() int
    }

    class ConditionWrapper {
        +addCondition(column, condition, value)
        +and_()
        +or_()
        +prepareConditionString() string
        +bind(stmt, startIndex) int
    }

    class SqliteUtil {
        +createTableIfNotExists(db, table, struct)
        +insertEntities(db, table, entities)
        +queryEntities(db, table, index, nums, condition)
        +updateEntities(db, table, entities)
        +deleteEntities(db, table, condition)
        +countEntities(db, table, condition)
    }

    class BaseStorage {
        +queryEntities(index, nums, condition)
        +insertEntities(entities)
        +updateEntities(entities)
        +deleteEntities(condition)
        +countEntities(condition)
    }

    class SqliteDBManager {
        +setDbPath(path)
        +createDBWithMutexPtr(path, createNew)
        +writeDBWithMutexPtr(path)
    }

    SQLiteDatabase <|-- SqliteDbWithMutex
    SQLiteStatement --> SQLiteDatabase
    SQLiteStatement --> SqliteColumn
    BaseTableStructInfo o-- ColumnInfo
    ConditionWrapper --> ColumnInfo
    ConditionWrapper --> SQLiteStatement
    SqliteUtil --> SQLiteStatement
    SqliteUtil --> BaseTableStructInfo
    SqliteUtil --> ConditionWrapper
    BaseStorage --> SqliteUtil
    BaseStorage --> SqliteDbWithMutex
    SqliteDBManager --> SqliteDbWithMutex
```

## 核心数据流

### 实体到表结构的映射

```mermaid
flowchart LR
    Entity["struct Entity<br/>C++ 字段"] --> Bind["Entity::bind(stmt)<br/>写入字段值"]
    Entity --> SetValue["Entity::setValue(stmt)<br/>读取查询结果"]
    Entity --> Macro["TABLESTRUCTINFO_BEGIN/END<br/>TABLESTRUCTINFO_COLUMN"]
    Macro --> TableInfo["TableStructInfo<Entity>"]
    TableInfo --> CreateSQL["CREATE TABLE SQL"]
    TableInfo --> InsertSQL["INSERT prepare SQL"]
    TableInfo --> UpdateSQL["UPDATE prepare SQL"]
    TableInfo --> PrimaryKey["primaryColumnInfos<br/>更新条件"]
    Bind --> Statement["SQLiteStatement"]
    SetValue --> Result["vector<Entity>"]
```

这套映射依赖显式声明，不依赖运行时反射。优点是编译期结构清晰、生成逻辑可控；代价是新增实体时需要手写 `bind()` 和 `setValue()`。

### 初始化和建表

```mermaid
sequenceDiagram
    participant SM as StorageManager
    participant DM as SqliteDBManager
    participant DB as SqliteDbWithMutex
    participant TS as TableStructInfo<Entity>
    participant SU as SqliteUtil
    participant BS as BusinessStorage

    SM->>DM: createDBWithMutexPtr(dbPath + "/data.db")
    DM->>DB: open sqlite database
    DM->>DB: PRAGMA journal_mode=WAL
    DM->>DB: PRAGMA synchronous=OFF
    SM->>TS: self()
    SM->>SU: createTableIfNotExists(DB, tableName, TS)
    SU->>TS: createSql()
    SU->>DB: execute(CREATE TABLE IF NOT EXISTS ...)
    SM->>SU: createIndexIfNotExists(...)
    SM->>BS: new Storage(readDB, tableName, writeDB)
```

### 查询流程

```mermaid
sequenceDiagram
    participant Biz as 业务代码
    participant BS as BaseStorage
    participant CW as ConditionWrapper
    participant SU as SqliteUtil
    participant ST as SQLiteStatement
    participant E as Entity

    Biz->>CW: addCondition(table.uniqueId, EQUALS, guid)
    Biz->>BS: queryEntities<Entity>(0, 4000, condition)
    BS->>SU: queryEntities(db, tableName, index, nums, condition)
    SU->>CW: prepareConditionString()
    SU->>ST: prepare SELECT ... WHERE ... LIMIT ...
    SU->>CW: bind(stmt)
    loop each row
        SU->>ST: executeStep()
        SU->>E: entity.setValue(stmt)
    end
    SU-->>Biz: vector<Entity>
```

### 插入和更新流程

```mermaid
sequenceDiagram
    participant Biz as 业务代码
    participant BS as BaseStorage
    participant SU as SqliteUtil
    participant TS as TableStructInfo<Entity>
    participant DB as SqliteDbWithMutex
    participant ST as SQLiteStatement
    participant E as Entity

    Biz->>BS: insertEntities(vector<Entity>)
    BS->>SU: insertEntities(writeDB, tableName, entities)
    SU->>TS: insertPrepareSql()
    SU->>ST: prepare INSERT INTO table (...)
    SU->>DB: transaction()
    loop each entity
        SU->>E: entity.bind(stmt)
        SU->>ST: executeStep()
        SU->>ST: reset()
    end
    SU->>DB: commit()
```

更新实体时，`SqliteUtil::updateEntities<Entity>` 会使用 `TableStructInfo<Entity>::primaryColumnInfos()` 生成 `WHERE` 条件，因此实体需要通过表结构宏声明主键字段。

## 二、重点分析：条件组装和拼 SQL

`ConditionWrapper` 是查询条件的核心封装。它负责两件事：

- 把业务条件组织成 `WHERE` 片段。
- 按占位符顺序把条件值绑定到 `SQLiteStatement`。

### 总览图

```mermaid
flowchart LR
    Biz[业务代码] --> Add[addCondition]
    Add --> Store[保存 PrepareInfo]
    Store --> Build[prepareConditionString]
    Build --> SQL[生成 WHERE 占位符 SQL]
    SQL --> Prepare[SQLiteStatement prepare]
    Store --> Bind[bind]
    Bind --> Param[按顺序绑定参数]
    Prepare --> Exec[executeStep]
    Param --> Exec
```

条件组装采用“SQL 结构”和“参数值”分离的方式：先生成 `WHERE column op ?` 这样的结构，再按顺序绑定具体值。

### 内部数据结构

```mermaid
flowchart TD
    CW[ConditionWrapper]
    CW --> Relation[m_logicRelation<br/>AND or OR]
    CW --> Infos[m_conditionInfos]
    Infos --> Variant[variant]
    Variant --> PI[PrepareInfo<br/>单个字段条件]
    Variant --> Child[ConditionWrapper<br/>嵌套条件组]

    PI --> Column[m_columnName]
    PI --> Op[m_condition]
    PI --> Value[m_value]

    Value --> Empty[monostate<br/>无绑定值]
    Value --> One[SqliteColumnValue<br/>单个值]
    Value --> Many[vector SqliteColumnValue<br/>IN 条件值集合]
```

`m_conditionInfos` 使用 `std::variant<PrepareInfo, ConditionWrapper>`，因此一个条件组里既可以放普通条件，也可以放另一个条件组，用来表达嵌套逻辑。

### 条件类型到 SQL 操作符

| Condition | SQL 片段 |
| --- | --- |
| `EQUALS` | `column == ?` |
| `NOT_EQUALS` | `column <> ?` |
| `LE` | `column <= ?` |
| `GE` | `column >= ?` |
| `LT` | `column < ?` |
| `GT` | `column > ?` |
| `VALUES_IN` | `column IN (?, ?, ...)` |
| `VALUES_NOT_IN` | `column NOT IN (?, ?, ...)` |
| `LIKE` | `column LIKE ?` |
| `NOT_LIKE` | `column NOT LIKE ?` |
| `IS_NOT_NULL` | `column IS NOT ?` |
| `IS_NULL` | `column IS ?` |

当前实现里 `LIKE` 和 `NOT_LIKE` 会在 `addCondition()` 阶段给字符串值追加 `%`，也就是默认做“前缀匹配”：`abc` 会变成 `abc%`。

### 拼 SQL 的主流程

```mermaid
flowchart TD
    A[业务代码创建 ConditionWrapper] --> B[addCondition ColumnInfo Condition Value]
    B --> C[生成 PrepareInfo]
    C --> D[mergeOptimization 尝试合并同类条件]
    D --> E[保存到 m_conditionInfos]

    E --> F[SqliteUtil 拼 SELECT UPDATE COUNT]
    F --> G[condition.prepareConditionString]
    G --> H{条件是否为空}
    H -->|是| I[返回空字符串]
    H -->|否| J[递归生成条件体]
    J --> K[外层补 WHERE]
    K --> L[SQLiteStatement prepare]
    L --> M[condition.bind stmt]
    M --> N[executeStep]
```

`ConditionWrapper::prepareConditionString()` 只生成占位符 SQL，不直接把值拼进去。例如：

```cpp
sqlite::ConditionWrapper condition;
condition.addCondition(table.uniqueId, sqlite::Condition::EQUALS, guid);
condition.addCondition(table.status, sqlite::Condition::GT, 0);
```

生成的条件 SQL 类似：

```sql
WHERE ( uniqueId  == ? AND  status  > ?)
```

随后 `ConditionWrapper::bind()` 会按照条件出现顺序绑定 `guid` 和 `0`。

### 单条件生成规则

```mermaid
flowchart LR
    PI[PrepareInfo] --> C{Condition}
    C -->|普通比较| A["column op ?"]
    C -->|LIKE / NOT LIKE| B["column LIKE ?"]
    C -->|IN / NOT IN| D["column IN (?, ?, ...)"]
    C -->|NULL 判断| E["column IS ? / IS NOT ?"]

    A --> P[prepareConditionString]
    B --> P
    D --> P
    E --> P

    P --> Bind[bind]
    Bind -->|单值| One[sqlite::bind 一次]
    Bind -->|IN 集合| Many[循环 sqlite::bind]
```

对应实现集中在 `PrepareInfo::prepareConditionString()` 和 `PrepareInfo::bind()`：

- `VALUES_IN` / `VALUES_NOT_IN` 根据集合大小生成多个 `?`。
- 其他条件生成单个 `?`。
- `bind()` 对 IN 集合逐个绑定，对普通条件绑定一个值。
- 如果 `m_value` 是 `monostate`，普通条件不会绑定值。

### 多条件和嵌套条件

```mermaid
flowchart TD
    A[ConditionWrapper] --> B{size}
    B -->|0| C[空字符串]
    B -->|1| D[直接生成唯一条件]
    B -->|大于 1| E[先写左括号]
    E --> F[生成第一个条件]
    F --> G[插入 AND 或 OR]
    G --> H[生成下一个条件]
    H --> I{还有条件?}
    I -->|是| G
    I -->|否| J[写右括号]

    K[嵌套 ConditionWrapper] --> H
```

多条件会被括号包起来，避免和外层 SQL 或嵌套条件发生优先级歧义。`operator&` 会切换为 AND 组合，`operator|` 会切换为 OR 组合。

### 与 SqliteUtil 的组合方式

```mermaid
sequenceDiagram
    participant Biz as 业务代码
    participant CW as ConditionWrapper
    participant SU as SqliteUtil
    participant ST as SQLiteStatement

    Biz->>CW: addCondition(column, condition, value)
    Biz->>SU: queryEntities(db, tableName, condition)
    SU->>CW: prepareConditionString()
    CW-->>SU: WHERE column op ?
    SU->>ST: SQLiteStatement(db, sql)
    SU->>CW: bind(stmt)
    CW->>ST: bind values by index
    SU->>ST: executeStep()
```

常见入口的拼接方式：

| 方法 | SQL 结构 | 条件处理 |
| --- | --- | --- |
| `queryEntities` | `SELECT * FROM table ... LIMIT ...` | 使用 `prepareConditionString()` + `bind()` |
| `updateEntities` 部分字段更新 | `UPDATE table SET col = ? ...` | 先绑定 SET 值，再绑定条件值 |
| `countEntities` | `SELECT COUNT(1) FROM table ...` | 使用 `prepareConditionString()` + `bind()` |
| `deleteEntities` | `DELETE FROM table ...` | 当前使用 `conditionString()` 直接拼值 |
| `createViewIfNotExist` | `CREATE VIEW ... AS SELECT ...` | 当前使用 `conditionString()` 直接拼值 |

需要注意：大多数查询和更新走参数绑定，但 `deleteEntities()` 和 `createViewIfNotExist()` 当前用的是 `conditionString()`，会把值直接拼进 SQL。业务输入如果来自用户，应优先改成 prepare + bind 的形式，或者保证上层输入已经受控。

### 条件合并优化

`ConditionWrapper::mergeOptimization()` 会在新增条件时尝试删除冗余条件。它主要处理同一字段上的同类比较：

```mermaid
flowchart TD
    A[新增 PrepareInfo] --> B[遍历已有普通条件]
    B --> C{字段名和条件类型相近?}
    C -->|否| D[保留两个条件]
    C -->|是| E{当前逻辑关系}
    E -->|AND| F[保留更严格的范围条件]
    E -->|OR| G[保留更宽松的范围条件]
    F --> H[删除被覆盖条件]
    G --> H
```

示例：

```cpp
condition.addCondition(table.progress, sqlite::Condition::GE, 10);
condition.addCondition(table.progress, sqlite::Condition::GE, 50);
```

在 AND 关系下，`progress >= 50` 已经覆盖 `progress >= 10`，因此可以删除较弱条件。

### 推荐用法

```cpp
auto& table = sqlite::TableStructInfo<DownloadingItem>::self();

sqlite::ConditionWrapper condition;
condition.addCondition(table.uniqueId, sqlite::Condition::EQUALS, guid);
condition.addCondition(table.status, sqlite::Condition::GE, DownloadStatus::Running);

auto items = storage->queryEntities<DownloadingItem>(0, 100, condition);
```

推荐规则：

- 查询、计数、更新优先使用 `prepareConditionString()` 和 `bind()` 的路径。
- 需要 OR 组合时，用单独的 `ConditionWrapper` 分组后再 `operator|`。
- `LIKE` 当前默认追加后缀 `%`，如果需要 `%keyword%` 这种包含匹配，需要传入前缀 `%`。
- `VALUES_IN` 的值集合不能为空，否则实现会记录 warning 并跳过生成有效条件。


### 线程和连接模型

```mermaid
flowchart TD
    Init[SqliteDBManager::init] --> Config[sqlite3_config<br/>SQLITE_CONFIG_MULTITHREAD]
    Init --> Dir[create dbPath directory]

    Request1[createDBWithMutexPtr(path)] --> Cache{path 是否已缓存}
    Request2[writeDBWithMutexPtr(path)] --> Cache
    Cache -->|否| Open[创建 SqliteDbWithMutex]
    Cache -->|是| Reuse[复用 cached db ptr]
    Open --> Pragma[PRAGMA journal_mode=WAL<br/>PRAGMA synchronous=OFF]
    Pragma --> Store[按绝对路径缓存]
    Reuse --> Lock[SqliteUtil 内部 lock_guard]
    Store --> Lock
    Lock --> ReadWrite[执行读写]
```

`SqliteDbWithMutex` 把互斥量和连接放在一起，`SqliteUtil` 在查询、插入、更新等入口统一加锁。这样业务代码不用直接处理 sqlite 连接的并发保护。

## 现有业务表

当前主业务数据库名为 `data.db`，由 `ClientUi/Storage/StorageManager` 初始化，默认包含以下表：

```mermaid
erDiagram
    DownloadedItem {
        string uniqueId "unique"
        int pluginId
        string filePath
        string coverPath
        string id
        string idType
        string title
        string author
        string url
        string option1
        string option2
        string option3
        int duration
        int fileType
        bool fileExist
        string fileExtension
    }

    DownloadingItem {
        string uniqueId "unique"
        int pluginId
        string filePath
        string coverPath
        string id
        string idType
        string title
        string author
        string url
        string option1
        string option2
        string option3
        double progress
        int duration
        int status
        int fileType
        string fileExtension
    }

    SearchHistory {
        string url "primary unique"
        int pluginId
        int64 timestamp
        int searchTimes
    }

    CookiesInfo {
        int pluginId "primary part"
        string domain "primary part"
        int64 updateTimestamp
        string cookie
        string expires
        string storedata
    }
```

### 业务表与 Storage 对应关系

```mermaid
flowchart LR
    subgraph dataDb["data.db"]
        T1[(DownloadedItem)]
        T2[(DownloadingItem)]
        T3[(SearchHistory)]
        T4[(CookiesInfo)]
    end

    S1[DownloadedItemStorage] --> T1
    S2[DownloadingItemStorage] --> T2
    S3[SearchHistoryStorage] --> T3
    S4[CookiesInfoStorage] --> T4

    M[StorageManager::instance] --> S1
    M --> S2
    M --> S3
    M --> S4

    S1 -.isDownload / lastItems / updateFileExist.-> UI1[下载完成列表]
    S2 -.updateStatus / isDownload / lastItems.-> UI2[下载中列表]
    S3 -.insertOrUpdate / allItems.-> UI3[搜索历史]
    S4 -.getCookiesInfo / insertOrUpdate.-> UI4[登录 Cookie]
```

`StorageManager` 还会为部分高频查询创建索引：

| 表 | 索引字段 |
| --- | --- |
| `DownloadedItem` | `filePath`、`id` |
| `DownloadingItem` | `filePath`、`id` |
| `CookiesInfo` | `updateTimestamp`、`expires` |

`SearchHistory` 会创建触发器：

- 插入后限制历史数量，最多保留 `SearchHistoryStorage::maxNum` 条。
- 更新后增加 `searchTimes`。

## 设计优点

```mermaid
flowchart TD
    Root[SQLite 封装收益]

    Root --> Safe[安全]
    Safe --> Safe1[RAII 管理 sqlite3 和 stmt 句柄]
    Safe --> Safe2[参数绑定减少注入风险]
    Safe --> Safe3[SQLiteException 统一错误信息]

    Root --> Maintain[维护]
    Maintain --> Maintain1[SQL 生成集中]
    Maintain --> Maintain2[表结构元信息统一]
    Maintain --> Maintain3[Storage 接口一致]

    Root --> Perf[性能]
    Perf --> Perf1[预编译语句]
    Perf --> Perf2[批量事务]
    Perf --> Perf3[WAL]
    Perf --> Perf4[高频字段索引]

    Root --> Extend[扩展]
    Extend --> Extend1[新表接入步骤固定]
    Extend --> Extend2[业务方法放在业务 Storage]
    Extend --> Extend3[底层封装可复用]

    Root --> Concurrent[并发]
    Concurrent --> Concurrent1[连接按路径缓存]
    Concurrent --> Concurrent2[连接级互斥]
    Concurrent --> Concurrent3[读写入口统一保护]
```

### 资源管理更安全

`SQLiteDatabase` 和 `SQLiteStatement` 使用 `unique_ptr` 加自定义 deleter 管理 `sqlite3*` 和 `sqlite3_stmt*`，避免调用方手动释放底层句柄。

### SQL 拼接集中化

建表、插入、更新和条件 SQL 主要由 `TableStructInfo`、`ColumnInfo`、`ConditionWrapper` 和 `SqliteUtil` 生成。业务代码不需要重复手写大量 `INSERT`、`UPDATE` 和 `WHERE` 字符串。

### 参数绑定减少注入风险

条件查询走 `prepareConditionString()` 生成占位符，再由 `ConditionWrapper::bind()` 绑定参数；实体插入和更新通过 `Entity::bind()` 绑定字段值。

### 读写接口统一

业务 Storage 继承 `BaseStorage` 后即可获得通用 CRUD 能力。业务类只需要补充领域相关方法，例如 `isDownload()`、`insertOrUpdate()`、`lastItems()`。

### 并发访问边界清晰

`SqliteDbWithMutex` 在数据库连接上挂载 `recursive_mutex`，`SqliteUtil` 在读写时统一加锁。`SqliteDBManager` 按路径缓存写连接，并在初始化时设置 SQLite multi-thread 模式和 WAL。

### 扩展新表成本低

新增表时主要完成三件事：

1. 定义实体结构体。
2. 实现 `bind()` 和 `setValue()`。
3. 使用 `TABLESTRUCTINFO_*` 宏注册字段。

之后就可以复用 `BaseStorage` 和 `SqliteUtil` 的通用逻辑。

### 业务语义更集中

每个业务表对应一个业务 Storage，通用 CRUD 留在 `BaseStorage`，领域逻辑放在具体 Storage。例如：

- `DownloadedItemStorage::isDownload()` 表达“是否已下载”。
- `DownloadingItemStorage::updateStatus()` 表达“更新下载状态”。
- `SearchHistoryStorage::insertOrUpdate()` 表达“搜索历史去重或更新”。
- `CookiesInfoStorage::getCookiesInfo()` 表达“按插件读取登录信息”。

这比在 UI 层散落 SQL 更容易维护，也更方便后续替换存储实现或调整表结构。

### 建表和字段变更更可追踪

字段定义集中在 `TABLESTRUCTINFO_COLUMN`，表结构 SQL 由 `TableStructInfo<Entity>::self().createSql()` 生成。开发者查看实体头文件即可知道：

- 字段名
- SQLite 字段类型
- 是否唯一
- 是否主键
- 是否自增

### 批量写入性能更好

`SqliteUtil::insertEntities()` 和 `SqliteUtil::updateEntities()` 在内部开启事务，循环复用同一个 `SQLiteStatement`。相比每条数据单独打开语句并提交事务，批量写入的 sqlite 开销更小。

### 调试路径清晰

`SQLiteStatement::expandedSQL()` 可查看绑定参数后的 SQL，适合定位条件生成、字段顺序、绑定值错误等问题。

## 设计取舍

```mermaid
flowchart LR
    Native[原生 sqlite3<br/>最灵活<br/>重复代码最多] --> Current[当前 Sqlite 模块<br/>封装 CRUD 和条件<br/>保留直接 SQL 能力]
    Current --> ORM[完整 ORM<br/>封装程度更高<br/>运行时规则更多]

    Raw[纯字符串 SQL 工具<br/>实现简单<br/>安全和维护压力大] --> Current

    Current --> Trade1[优点<br/>少写 prepare/bind/step/reset]
    Current --> Trade2[优点<br/>复杂查询仍可手写 SQL]
    Current --> Trade3[取舍<br/>不做自动迁移和关系映射]
    Current --> Trade4[取舍<br/>Entity 仍需手写 bind/setValue]
```

当前方案介于原生 sqlite3 和完整 ORM 之间：

- 比原生 sqlite3 少写大量重复 prepare、bind、step、reset 和事务代码。
- 比完整 ORM 更贴近 SQL，复杂查询仍可直接写 SQL。
- 不提供自动迁移、关系映射、懒加载等完整 ORM 能力。
- 字段映射靠显式宏和手写绑定，换来较低的运行时复杂度。

## 使用方式

### 1. 定义实体

实体字段应使用 `SqliteColumnValue` 支持的类型，例如整数、浮点数、`std::string`、`std::vector<uint8_t>`。

```cpp
struct SearchHistory
{
    std::string url;
    int pluginId;
    int64_t timestamp;
    int searchTimes{};

    int bind(sqlite::SQLiteStatement& stmt) const;
    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0);
};
```

### 2. 实现绑定和读取

`bind()` 的顺序必须和 `TABLESTRUCTINFO_COLUMN` 声明顺序一致。

```cpp
int SearchHistory::bind(sqlite::SQLiteStatement& stmt) const
{
    int index = 1;
    stmt.bind(index++, url);
    stmt.bind(index++, pluginId);
    stmt.bind(index++, timestamp);
    stmt.bind(index++, searchTimes);
    return index;
}

void SearchHistory::setValue(sqlite::SQLiteStatement& stmt, int startIndex)
{
    url = stmt.column(startIndex++).getString();
    pluginId = stmt.column(startIndex++).getInt();
    timestamp = stmt.column(startIndex++).getInt64();
    searchTimes = stmt.column(startIndex++).getInt();
}
```

### 3. 注册表结构

```cpp
TABLESTRUCTINFO_BEGIN(SearchHistory)
    TABLESTRUCTINFO_COLUMN(url, url, false, true, true)
    TABLESTRUCTINFO_COLUMN(pluginId)
    TABLESTRUCTINFO_COLUMN(timestamp)
    TABLESTRUCTINFO_COLUMN(searchTimes)
TABLESTRUCTINFO_END(SearchHistory)
```

宏参数含义：

```text
TABLESTRUCTINFO_COLUMN(member)
TABLESTRUCTINFO_COLUMN(member, columnName)
TABLESTRUCTINFO_COLUMN(member, columnName, autoIncrement)
TABLESTRUCTINFO_COLUMN(member, columnName, autoIncrement, unique)
TABLESTRUCTINFO_COLUMN(member, columnName, autoIncrement, unique, primaryKey)
```

### 4. 创建业务 Storage

```cpp
class SearchHistoryStorage : public sqlite::BaseStorage
{
public:
    using Entity = SearchHistory;
    using BaseStorage::BaseStorage;

    bool insertOrUpdate(const std::string& url, int pluginId);
    std::vector<std::string> allItems();
};
```

创建表和 Storage：

```cpp
auto readPtr = sqlite::SqliteDBManager::createDBWithMutexPtr(sqlite::dbPath + "/data.db");
auto writePtr = sqlite::SqliteDBManager::createDBWithMutexPtr(sqlite::dbPath + "/data.db");

auto& tableStruct = sqlite::TableStructInfo<SearchHistoryStorage::Entity>::self();
sqlite::SqliteUtil::createTableIfNotExists(writePtr, "SearchHistory", tableStruct);

auto storage = std::make_shared<SearchHistoryStorage>(readPtr, "SearchHistory", writePtr);
```

也可以使用项目中的 `StorageManager` 统一获取现有业务 Storage：

```cpp
auto storage = sqlite::StorageManager::instance().searchHistoryStorage();
```

### 5. 查询

```cpp
auto& table = sqlite::TableStructInfo<SearchHistory>::self();

sqlite::ConditionWrapper condition;
condition.addCondition(table.url, sqlite::Condition::LIKE, std::string("%bilibili%"));

auto histories = storage->queryEntities<SearchHistory>(0, 20, condition);
```

### 6. 插入

```cpp
SearchHistory history;
history.url = "https://example.com/video";
history.pluginId = 1;
history.timestamp = 1710000000;
history.searchTimes = 1;

storage->insertEntities<SearchHistory>({history});
```

### 7. 更新

按实体更新时，`TableStructInfo` 中标记为主键或自增的字段会用于生成 `WHERE` 条件。

```cpp
history.searchTimes = 2;
storage->updateEntities<SearchHistory>({history});
```

更新部分字段时可以直接传 `SqliteColumn`：

```cpp
auto& table = sqlite::TableStructInfo<DownloadingItem>::self();

sqlite::ConditionWrapper condition;
condition.addCondition(table.uniqueId, sqlite::Condition::EQUALS, guid);

sqlite::SqliteColumnValue value = static_cast<int64_t>(status);
sqlite::SqliteColumn columnValue(value, -1, table.status.columnName());
sqlite::SqliteUtil::updateEntities(
    writeDb,
    "DownloadingItem",
    {columnValue},
    condition);
```

### 8. 删除和计数

```cpp
sqlite::ConditionWrapper condition;
condition.addCondition(table.url, sqlite::Condition::EQUALS, url);

storage->deleteEntities(condition);
auto count = storage->countEntities({});
```

## 接入新表建议

```mermaid
flowchart LR
    A[定义 Entity] --> B[实现 bind / setValue]
    B --> C[用 TABLESTRUCTINFO 注册字段]
    C --> D[继承 BaseStorage]
    D --> E[在 StorageManager 或工厂中创建表]
    E --> F[按业务需要创建索引 / 触发器]
    F --> G[业务代码调用 Storage]
```

### 是否应该新增 Storage

```mermaid
flowchart TD
    A[需要持久化一类业务数据] --> B{是否已有对应表?}
    B -->|是| C[在现有 Storage 增加业务方法]
    B -->|否| D{数据是否有明确实体结构?}
    D -->|是| E[新增 Entity + TableStructInfo + Storage]
    D -->|否| F{是否只是一次性统计/临时查询?}
    F -->|是| G[在相关 Storage 内写专用 SQL]
    F -->|否| H[先整理实体模型再建表]
    E --> I{是否有高频查询字段?}
    I -->|是| J[创建索引]
    I -->|否| K[保持默认表结构]
    J --> L[接入 StorageManager]
    K --> L
```

建议遵循：

- `bind()`、`setValue()`、`TABLESTRUCTINFO_COLUMN` 三者字段顺序保持一致。
- 高频查询字段在创建 Storage 时建立索引。
- 批量插入和批量更新优先使用 `SqliteUtil` 的事务封装接口。
- 条件查询优先使用 `ConditionWrapper`，减少手写 SQL。
- 复杂排序、聚合或 join 可以使用 `SqliteUtil::queryEntities<Entity>(db, querySql)`，但 SQL 字符串应集中在对应业务 Storage 内，不要散落在 UI 代码中。

## 注意事项

- 当前封装依赖实体自己实现 `bind()` 和 `setValue()`，不会自动反射字段值。
- `SqliteUtil::updateEntities<Entity>` 依赖主键字段生成更新条件，没有主键的实体不适合直接使用该接口。
- `SqliteDBManager` 初始化时设置 `PRAGMA synchronous=OFF`，性能更高，但异常断电场景下持久化保证较弱。
- `createDBWithMutexPtr(path, true)` 会创建一个新连接，同时缓存默认写连接；需要复用写连接时可使用 `writeDBWithMutexPtr(path)`。
- `ConditionWrapper::conditionString()` 会直接拼值，调试可以用；业务查询更推荐 `prepareConditionString()` 加 `bind()`。
