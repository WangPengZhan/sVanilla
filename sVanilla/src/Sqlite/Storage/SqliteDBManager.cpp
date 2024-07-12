#include <filesystem>

#include <sqlite3.h>

#include "SqliteDBManager.h"

namespace sqlite
{
std::string dbPath = ".db";
std::once_flag SqliteDBManager::m_createFlag;
std::mutex SqliteDBManager::m_mutex;
std::unordered_map<std::string, SqliteWithMutexPtr> SqliteDBManager::m_defaultWriteDbWtihMutexs;
std::unordered_map<std::string, SqliteDBPtr> SqliteDBManager::m_defaultWriteDbs;

void SqliteDBManager::setDbPath(const std::string& path)
{
    dbPath = path;
}

SqliteDBPtr SqliteDBManager::createDBPtr(const std::string& path, bool createNew)
{
    init();
    std::string fullPath = std::filesystem::absolute(std::filesystem::u8path(path)).string();
    SqliteDBPtr db;
    if (createNew)
    {
        db = std::make_shared<SQLiteDatabase>(path);
        std::lock_guard lk(m_mutex);
        if (m_defaultWriteDbs.find(fullPath) == m_defaultWriteDbs.end())
        {
            m_defaultWriteDbs.insert({fullPath, db});
            dbInit(db);
        }
    }
    else
    {
        db = writeDBPtr(path);
    }

    return db;
}

SqliteDBPtr SqliteDBManager::writeDBPtr(const std::string& path)
{
    std::string fullPath = std::filesystem::absolute(std::filesystem::u8path(path)).string();
    {
        std::lock_guard lk(m_mutex);
        if (m_defaultWriteDbs.find(fullPath) == m_defaultWriteDbs.end())
        {
            auto db = std::make_shared<SQLiteDatabase>(path);
            m_defaultWriteDbs.insert({fullPath, db});
            dbInit(db);
        }

        return m_defaultWriteDbs.at(fullPath);
    }
}

SqliteWithMutexPtr SqliteDBManager::createDBWithMutexPtr(const std::string& path, bool createNew)
{
    init();
    std::string fullPath = std::filesystem::absolute(std::filesystem::u8path(path)).string();
    SqliteWithMutexPtr db;
    if (createNew)
    {
        db = std::make_shared<SqliteDbWithMutex>(path);
        std::lock_guard lk(m_mutex);
        if (m_defaultWriteDbWtihMutexs.find(fullPath) == m_defaultWriteDbWtihMutexs.end())
        {
            m_defaultWriteDbWtihMutexs.insert({fullPath, std::make_shared<SqliteDbWithMutex>(path)});
            dbInit(db);
        }
    }
    else
    {
        db = writeDBWithMutexPtr(path);
    }

    return db;
}

SqliteWithMutexPtr SqliteDBManager::writeDBWithMutexPtr(const std::string& path)
{
    std::string fullPath = std::filesystem::absolute(std::filesystem::u8path(path)).string();
    {
        std::lock_guard lk(m_mutex);
        if (m_defaultWriteDbWtihMutexs.find(fullPath) == m_defaultWriteDbWtihMutexs.end())
        {
            auto db = std::make_shared<SqliteDbWithMutex>(path);
            m_defaultWriteDbWtihMutexs.insert({fullPath, db});
            dbInit(db);
        }

        return m_defaultWriteDbWtihMutexs.at(fullPath);
    }
}

void SqliteDBManager::init()
{
    std::call_once(m_createFlag, []() {
        sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        createDir();
    });
}

void SqliteDBManager::dbInit(SQLiteDatabase& db)
{
    db.execute("PRAGMA journal_mode=WAL;");
    db.execute("PRAGMA synchronous=OFF;");
}

void SqliteDBManager::dbInit(SqliteDBPtr& db)
{
    if (db)
    {
        dbInit(*(db.get()));
    }
}

void SqliteDBManager::dbInit(SqliteWithMutexPtr& db)
{
    if (db)
    {
        std::lock_guard lk(db->mutex);
        dbInit(*(db.get()));
    }
}

void SqliteDBManager::createDir()
{
    auto dbDir = std::filesystem::u8path(dbPath);
    if (!std::filesystem::is_directory(dbDir))
    {
        std::filesystem::create_directory(dbDir);
    }
}

}  // namespace sqlite
