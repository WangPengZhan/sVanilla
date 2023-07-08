#pragma once

#include "Sqlite/SQLiteDatabase.h"

namespace SQLite
{

class SQLiteManager
{
public:
    static SQLiteManager& getInstance();

private:
    SQLiteManager();
    ~SQLiteManager();

    SQLiteManager(const SQLiteManager& other) = delete;
    SQLiteManager& operator=(const SQLiteManager& other) = delete;
    SQLiteManager(SQLiteManager&& other) = delete;
    SQLiteManager& operator=(SQLiteManager&& other) = delete;

    SQLiteDatabase m_db;
};

} // namespace SQLite

