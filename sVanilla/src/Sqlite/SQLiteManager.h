#pragma once

#include "SQLiteDatabase.h"

namespace SQLite
{

class SQLiteManager
{
public:
    static SQLiteManager& getInstance();
    SQLiteManager(const SQLiteManager& other) = delete;
    SQLiteManager& operator=(const SQLiteManager& other) = delete;
    SQLiteManager(SQLiteManager&& other) = delete;
    SQLiteManager& operator=(SQLiteManager&& other) = delete;

private:
    SQLiteManager();
    ~SQLiteManager();


    SQLiteDatabase m_db;
};

}  // namespace SQLite
