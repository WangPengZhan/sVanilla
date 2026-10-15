#include "sqlite_fixture.h"

#include <memory>
#include <string>

namespace
{

// SQLiteDatabase 需要独占文件句柄，单独提供一个"先销毁句柄再删目录"的夹具。
class DatabaseTest : public ::testing::Test
{
protected:
    sqlite_test::ScopedTempDir m_temp{"sqlite_db"};
};

}  // namespace

TEST_F(DatabaseTest, OpensFileAndReportsName)
{
    const std::string path = m_temp.file("open.db");
    sqlite::SQLiteDatabase db(path);

    EXPECT_TRUE(db.isOpen());
    EXPECT_EQ(db.fileName(), path);
    EXPECT_NE(db.handle(), nullptr);
}

// 回归：早期实现只要 sqlite3_open 返回了句柄就认为"已打开"，
// 即使返回码是失败（比如路径不可写）也会误报 isOpen() == true。
TEST_F(DatabaseTest, ReportsClosedWhenPathIsNotADatabase)
{
    // 指向一个已存在的目录：sqlite3_open 必然失败
    sqlite::SQLiteDatabase db(m_temp.path().string());

    EXPECT_FALSE(db.isOpen());
    EXPECT_EQ(db.handle(), nullptr);
    EXPECT_FALSE(db.lastError().empty());
}

TEST_F(DatabaseTest, ExecuteRunsDdlAndDml)
{
    sqlite::SQLiteDatabase db(m_temp.file("exec.db"));
    ASSERT_TRUE(db.isOpen());

    EXPECT_TRUE(db.execute("CREATE TABLE t(id INTEGER PRIMARY KEY, name TEXT);"));
    EXPECT_TRUE(db.tableExists("t"));
    EXPECT_FALSE(db.tableExists("missing"));

    EXPECT_TRUE(db.execute("INSERT INTO t(id, name) VALUES (1, 'a');"));
    EXPECT_EQ(db.lastInsertRowid(), 1);

    EXPECT_TRUE(db.execute("INSERT INTO t(id, name) VALUES (2, 'b');"));
    EXPECT_EQ(db.lastInsertRowid(), 2);
}

TEST_F(DatabaseTest, ExecuteReportsFailureAndKeepsLastError)
{
    sqlite::SQLiteDatabase db(m_temp.file("bad.db"));
    ASSERT_TRUE(db.isOpen());

    EXPECT_FALSE(db.execute("THIS IS NOT SQL;"));
    EXPECT_FALSE(db.lastError().empty());
    EXPECT_FALSE(db.errorMsg().empty());
    EXPECT_NE(db.errorCode(), 0);
}

TEST_F(DatabaseTest, ListsTablesAndViewsSeparately)
{
    sqlite::SQLiteDatabase db(m_temp.file("list.db"));
    ASSERT_TRUE(db.isOpen());

    EXPECT_TRUE(db.tables().empty());
    EXPECT_TRUE(db.views().empty());

    ASSERT_TRUE(db.execute("CREATE TABLE t(id INTEGER);"));
    ASSERT_TRUE(db.execute("CREATE VIEW v AS SELECT id FROM t;"));

    const auto tables = db.tables();
    const auto views = db.views();

    EXPECT_EQ(tables.count("t"), 1u);
    EXPECT_EQ(tables.count("v"), 0u);
    EXPECT_EQ(views.count("v"), 1u);
    EXPECT_EQ(views.count("t"), 0u);
}

TEST_F(DatabaseTest, CommitPersistsAndRollbackDiscards)
{
    const std::string path = m_temp.file("tx.db");
    {
        sqlite::SQLiteDatabase db(path);
        ASSERT_TRUE(db.isOpen());
        ASSERT_TRUE(db.execute("CREATE TABLE t(id INTEGER PRIMARY KEY, name TEXT);"));

        ASSERT_TRUE(db.transaction());
        ASSERT_TRUE(db.execute("INSERT INTO t VALUES (1, 'kept');"));
        ASSERT_TRUE(db.commit());

        ASSERT_TRUE(db.transaction());
        ASSERT_TRUE(db.execute("INSERT INTO t VALUES (2, 'dropped');"));
        ASSERT_TRUE(db.rollback());
    }

    sqlite::SQLiteDatabase reopened(path);
    ASSERT_TRUE(reopened.isOpen());
    sqlite::SQLiteStatement count(reopened, "SELECT COUNT(1) FROM t;");
    ASSERT_TRUE(count.executeStep());
    EXPECT_EQ(count.column(0).getInt64(), 1);
}

TEST_F(DatabaseTest, RollbackWithoutTransactionFails)
{
    sqlite::SQLiteDatabase db(m_temp.file("norollback.db"));
    ASSERT_TRUE(db.isOpen());

    // 没有 BEGIN 就 ROLLBACK，sqlite3_exec 会报错
    EXPECT_FALSE(db.rollback());
    EXPECT_FALSE(db.lastError().empty());
}

TEST_F(DatabaseTest, DestructorReleasesFileHandle)
{
    const std::string path = m_temp.file("release.db");
    {
        sqlite::SQLiteDatabase db(path);
        ASSERT_TRUE(db.isOpen());
        ASSERT_TRUE(db.execute("CREATE TABLE t(id INTEGER);"));
    }

    // 句柄已释放，应能重新打开并看到表
    sqlite::SQLiteDatabase reopened(path);
    EXPECT_TRUE(reopened.isOpen());
    EXPECT_TRUE(reopened.tableExists("t"));
}
