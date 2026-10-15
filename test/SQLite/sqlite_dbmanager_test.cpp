#include "sqlite_fixture.h"

#include <memory>
#include <string>

#include "Sqlite/Storage/SqliteDBManager.h"

namespace
{

// SqliteDBManager 的连接缓存是进程级全局状态，且不提供释放接口。
// 因此每个用例都用互不相同的库文件名，避免相互干扰。
class DBManagerTest : public ::testing::Test
{
protected:
    sqlite_test::ScopedTempDir m_temp{"sqlite_mgr"};

    void SetUp() override
    {
        // 让 SqliteDBManager::createDir() 落到临时目录而不是工作目录
        sqlite::SqliteDBManager::setDbPath(m_temp.path().string());
    }
};

}  // namespace

TEST_F(DBManagerTest, SetDbPathUpdatesGlobal)
{
    const std::string custom = m_temp.file("custom_dir");
    sqlite::SqliteDBManager::setDbPath(custom);
    EXPECT_EQ(sqlite::dbPath, custom);

    // 还原成临时目录，避免影响后续用例
    sqlite::SqliteDBManager::setDbPath(m_temp.path().string());
}

TEST_F(DBManagerTest, CreateDBPtrReturnsUsableConnection)
{
    auto db = sqlite::SqliteDBManager::createDBPtr(m_temp.file("created.db"));
    ASSERT_TRUE(db != nullptr);
    EXPECT_TRUE(db->isOpen());
    EXPECT_TRUE(db->execute("CREATE TABLE t(id INTEGER);"));
}

TEST_F(DBManagerTest, WriteDBPtrIsCached)
{
    const std::string path = m_temp.file("cached.db");
    auto first = sqlite::SqliteDBManager::writeDBPtr(path);
    auto second = sqlite::SqliteDBManager::writeDBPtr(path);

    ASSERT_TRUE(first != nullptr);
    EXPECT_EQ(first.get(), second.get());
}

TEST_F(DBManagerTest, WriteDBWithMutexPtrIsCached)
{
    const std::string path = m_temp.file("cached_mutex.db");
    auto first = sqlite::SqliteDBManager::writeDBWithMutexPtr(path);
    auto second = sqlite::SqliteDBManager::writeDBWithMutexPtr(path);

    ASSERT_TRUE(first != nullptr);
    EXPECT_EQ(first.get(), second.get());
}

TEST_F(DBManagerTest, CreateDBWithMutexPtrCreatesInitialisedIndependentConnections)
{
    const std::string path = m_temp.file("independent_mutex.db");

    auto cached = sqlite::SqliteDBManager::writeDBWithMutexPtr(path);
    auto first = sqlite::SqliteDBManager::createDBWithMutexPtr(path, true);
    auto second = sqlite::SqliteDBManager::createDBWithMutexPtr(path, true);

    ASSERT_TRUE(cached != nullptr);
    ASSERT_TRUE(first != nullptr);
    ASSERT_TRUE(second != nullptr);
    EXPECT_NE(cached.get(), first.get());
    EXPECT_NE(cached.get(), second.get());
    EXPECT_NE(first.get(), second.get());
    EXPECT_TRUE(first->isOpen());
    EXPECT_TRUE(second->isOpen());

    sqlite::SQLiteStatement synchronous(*second, "PRAGMA synchronous;");
    ASSERT_TRUE(synchronous.executeStep());
    EXPECT_EQ(synchronous.column(0).getInt(), 1);
}

TEST_F(DBManagerTest, CreateDBPtrCreatesInitialisedIndependentConnections)
{
    const std::string path = m_temp.file("independent.db");

    auto cached = sqlite::SqliteDBManager::writeDBPtr(path);
    auto first = sqlite::SqliteDBManager::createDBPtr(path, true);
    auto second = sqlite::SqliteDBManager::createDBPtr(path, true);

    ASSERT_TRUE(cached != nullptr);
    ASSERT_TRUE(first != nullptr);
    ASSERT_TRUE(second != nullptr);
    EXPECT_NE(cached.get(), first.get());
    EXPECT_NE(cached.get(), second.get());
    EXPECT_NE(first.get(), second.get());
    EXPECT_TRUE(first->isOpen());
    EXPECT_TRUE(second->isOpen());

    sqlite::SQLiteStatement synchronous(*second, "PRAGMA synchronous;");
    ASSERT_TRUE(synchronous.executeStep());
    EXPECT_EQ(synchronous.column(0).getInt(), 1);
}

TEST_F(DBManagerTest, CreateWithCreateNewFalseFallsBackToCache)
{
    const std::string path = m_temp.file("reuse.db");

    auto written = sqlite::SqliteDBManager::createDBPtr(path, false);
    auto cachedAgain = sqlite::SqliteDBManager::createDBPtr(path, false);
    EXPECT_EQ(written.get(), cachedAgain.get());

    const std::string mutexPath = m_temp.file("reuse_mutex.db");
    auto mutexWritten = sqlite::SqliteDBManager::createDBWithMutexPtr(mutexPath, false);
    auto mutexAgain = sqlite::SqliteDBManager::createDBWithMutexPtr(mutexPath, false);
    EXPECT_EQ(mutexWritten.get(), mutexAgain.get());
}

TEST_F(DBManagerTest, ConnectionsAreInitialisedWithWalAndNormalSynchronous)
{
    auto db = sqlite::SqliteDBManager::createDBWithMutexPtr(m_temp.file("pragmas.db"));

    sqlite::SQLiteStatement journal(*db, "PRAGMA journal_mode;");
    ASSERT_TRUE(journal.executeStep());
    EXPECT_EQ(journal.column(0).getString(), "wal");

    sqlite::SQLiteStatement synchronous(*db, "PRAGMA synchronous;");
    ASSERT_TRUE(synchronous.executeStep());
    // NORMAL = 1（原实现是 OFF = 0，掉电易损坏）
    EXPECT_EQ(synchronous.column(0).getInt(), 1);
}
