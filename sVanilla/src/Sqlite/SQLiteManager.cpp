#include "SQLiteManager.h"

namespace SQLite
{
SQLiteManager& SQLiteManager::getInstance()
{
    static SQLiteManager sqliteManager;
    return sqliteManager;
}

SQLiteManager::SQLiteManager()
    : m_db("data.db")
{
}
SQLiteManager::~SQLiteManager()
{
}

} // namespace SQLite

