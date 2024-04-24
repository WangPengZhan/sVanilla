#include <filesystem>

#include <sqlite3.h>

#include "SqliteDBManager.h"

namespace sqlite
{

std::once_flag SqliteDBManager::m_createFlag;
std::mutex SqliteDBManager::m_mutex;
std::unordered_map<std::string, std::mutex> SqliteDBManager::m_mutexs;
std::unordered_map<std::string, std::list<SqliteDBPtr>> SqliteDBManager::m_dbs;

SqliteDBPtr SqliteDBManager::createDBPtr(const std::string& path, bool createNew)
{
    init();
    std::string fullPath = std::filesystem::absolute(path).string();
    SqliteDBPtr db;
    if (createNew)
    {
        db = std::make_shared<SQLiteDatabase>(path);
    }

    std::lock_guard lk(m_mutex);
    if (m_dbs.find(fullPath) == m_dbs.end())
    {
        m_mutexs[fullPath];
        dbInit(db);
    }

    m_dbs[fullPath].push_back(db);
    return m_dbs[fullPath].front();
}

SqliteDBPtr SqliteDBManager::writeDBPtr(const std::string& path)
{
    std::string fullPath = std::filesystem::absolute(path).string();
    {
        std::lock_guard lk(m_mutex);
        if (m_dbs.find(fullPath) != m_dbs.end())
        {
            if (!m_dbs.at(fullPath).empty())
            {
                return m_dbs.at(fullPath).front();
            }
        }
    }

    return createDBPtr(fullPath);
}

void SqliteDBManager::init()
{
    std::call_once(m_createFlag, []() {
        sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        createDir();
    });
}

void SqliteDBManager::dbInit(SqliteDBPtr& db)
{
    if (db)
    {
        db->execute("PRAGMA journal_mode=WAL;");
        db->execute("PRAGMA synchronous=OFF;");
    }
}

void SqliteDBManager::createDir()
{
    if (std::filesystem::is_directory(dbPath))
    {
        try
        {
            std::filesystem::remove_all(dbPath);
        }
        catch (const std::exception& e)
        {
        }
    }
    std::filesystem::create_directory(dbPath);
}
}  // namespace sqlite
