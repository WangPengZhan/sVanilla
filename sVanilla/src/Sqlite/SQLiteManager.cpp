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

FinishedItemTable& SQLiteManager::finishedItemTable()
{
    return m_finishedItemTable;
}

SQLiteManager::SQLiteManager()
    : m_db(std::string(dbDir) + "/data.db")
    , m_finishedItemTable(m_db, "FinishedItem")
{
    initSqlite();
    m_finishedItemTable.initTable();
}

SQLiteManager::~SQLiteManager()
{
}

void SQLiteManager::initSqlite()
{
    std::call_once(m_createFlag, []() {
        sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        if (!std::filesystem::is_directory(std::filesystem::u8path(dbDir)))
        {
            std::filesystem::create_directory(dbDir);
        }
    });
}

}  // namespace SQLite
