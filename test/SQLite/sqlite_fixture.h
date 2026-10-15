#pragma once

// Sqlite 测试共用的夹具与示例实体。
//
// 之所以单独建一个"实体"而不是直接用 DownloadingItem 之类的真实存储实体：
// 后端存储类都带 Qt 依赖，而 Sqlite 模块本身与 Qt 无关。这里用最小实体
// 覆盖到 SqliteColumn / SQLiteStatement / SqliteUtil 的全部值类型分支。

#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

#include <gtest/gtest.h>

#include "Sqlite/Database/SQLiteDatabase.h"
#include "Sqlite/Database/SQLiteStatement.h"
#include "Sqlite/SqlComposer/BaseInfo.h"
#include "Sqlite/SqlComposer/SqlUtil.h"

// 实体必须位于全局命名空间：TABLESTRUCTINFO_BEGIN 展开为
// template<> class TableStructInfo<::type>。
struct SqliteTestEntity
{
    int64_t id{0};
    std::string name;
    double score{0.0};
    std::vector<uint8_t> payload;

    int bind(sqlite::SQLiteStatement& stmt) const
    {
        int index = 1;
        stmt.bind(index++, id);
        stmt.bind(index++, name);
        stmt.bind(index++, score);
        stmt.bind(index++, payload);
        return index;
    }

    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0)
    {
        int index = startIndex;
        id = stmt.column(index++).getInt64();
        name = stmt.column(index++).getString();
        score = stmt.column(index++).getDouble();
        payload = stmt.column(index++).getBlob();
    }
};

TABLESTRUCTINFO_BEGIN(SqliteTestEntity)
TABLESTRUCTINFO_COLUMN(id, id, false, false, true)
TABLESTRUCTINFO_COLUMN(name, name, false, false, false)
TABLESTRUCTINFO_COLUMN(score, score, false, false, false)
TABLESTRUCTINFO_COLUMN(payload, payload, false, false, false)
TABLESTRUCTINFO_END(SqliteTestEntity)

// 专门用于验证 autoIncrement 建表 SQL 的实体（当前实现下组合有缺陷）。
struct SqliteAutoIncrementEntity
{
    int64_t id{0};

    int bind(sqlite::SQLiteStatement&) const
    {
        return 1;
    }
    void setValue(sqlite::SQLiteStatement&, int = 0)
    {
    }
};

TABLESTRUCTINFO_BEGIN(SqliteAutoIncrementEntity)
TABLESTRUCTINFO_COLUMN(id, id, true, false, true)
TABLESTRUCTINFO_END(SqliteAutoIncrementEntity)
// clang-format on

namespace sqlite_test
{

// 用例独占的临时目录，析构时整体删除（含 SQLite 的 -wal / -shm 文件）。
class ScopedTempDir
{
public:
    explicit ScopedTempDir(std::string tag)
    {
        // runId 保证不同次运行不会复用同一目录：上一轮可能残留没删干净的文件
        // （Windows 上句柄未释放时 remove_all 会失败，SqliteDBManager 的缓存连接就属于这种情况）。
        static const std::string runId = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        static std::atomic<unsigned> sequence{0};
        m_path = std::filesystem::temp_directory_path() / ("svanilla_" + tag + "_" + runId + "_" + std::to_string(sequence++));
        std::filesystem::create_directories(m_path);
    }

    ScopedTempDir(const ScopedTempDir&) = delete;
    ScopedTempDir& operator=(const ScopedTempDir&) = delete;

    ~ScopedTempDir()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    const std::filesystem::path& path() const
    {
        return m_path;
    }

    std::string file(const std::string& name) const
    {
        return (m_path / name).string();
    }

private:
    std::filesystem::path m_path;
};

// 提供一个真实磁盘上的临时数据库（需要 SqliteDBManager 之外的连接语义时使用）。
class TempDatabaseTest : public ::testing::Test
{
protected:
    // 声明顺序保证析构顺序为 m_db -> m_temp，Windows 上才能删掉目录。
    ScopedTempDir m_temp{"sqlite"};
    sqlite::SqliteWithMutexPtr m_db;

    void SetUp() override
    {
        m_db = std::make_shared<sqlite::SqliteDbWithMutex>(m_temp.file("svanilla_test.db"));
        ASSERT_TRUE(m_db->isOpen());
    }

    void TearDown() override
    {
        m_db.reset();
    }

    // 建表并返回表名。
    std::string createEntityTable(const std::string& tableName = "entity")
    {
        EXPECT_TRUE(sqlite::SqliteUtil::createTableIfNotExists(m_db, tableName, sqlite::TableStructInfo<SqliteTestEntity>::self()));
        return tableName;
    }

    static SqliteTestEntity makeEntity(int64_t id, const std::string& name, double score, const std::vector<uint8_t>& payload = {})
    {
        SqliteTestEntity entity;
        entity.id = id;
        entity.name = name;
        entity.score = score;
        entity.payload = payload;
        return entity;
    }
};

}  // namespace sqlite_test
