#include <sqlite3.h>

#include <filesystem>

#include "SQLiteManager.h"

namespace SQLite
{

std::once_flag SQLiteManager::m_createFlag{};

SQLiteManager& SQLiteManager::getInstance()
{
    static SQLiteManager sqliteManager;
    return sqliteManager;
}

SQLiteManager::SQLiteManager()
    : m_db("data.db")
{
    initSqlite();
}

SQLiteManager::~SQLiteManager()
{

}

void SQLiteManager::initSqlite()
{
    std::call_once(m_createFlag, []() {
        sqlite3_config(SQLITE_CONFIG_MULTITHREAD);

        if (std::filesystem::is_directory(dbDir))
        {
            std::filesystem::create_directory(dbDir);
        }
    });
}

}  // namespace SQLite
