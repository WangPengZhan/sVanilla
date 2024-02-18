#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>

#include "SQLiteDatabase.h"
#include "FinishedItemTable.h"

namespace SQLite
{

constexpr char dbDir[] = ".db";

class SQLiteManager
{
public:
    static SQLiteManager& getInstance();

    SQLiteManager(const SQLiteManager& other) = delete;
    SQLiteManager& operator=(const SQLiteManager& other) = delete;
    SQLiteManager(SQLiteManager&& other) = delete;
    SQLiteManager& operator=(SQLiteManager&& other) = delete;

    FinishedItemTable& finishedItemTable();

private:
    SQLiteManager();
    ~SQLiteManager();

    void initSqlite();

private:
    static std::once_flag m_createFlag;
    SQLiteDatabase m_db;
    FinishedItemTable m_finishedItemTable;
};

}  // namespace SQLite
