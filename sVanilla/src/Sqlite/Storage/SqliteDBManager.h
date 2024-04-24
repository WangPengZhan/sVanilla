#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>

#include "SQLite/Database/SQLiteDatabase.h"

namespace sqlite
{
static const std::string dbPath = ".db";

class SqliteDBManager
{
public:
    static SqliteDBPtr createDBPtr(const std::string& path, bool createNew = true);
    static SqliteDBPtr writeDBPtr(const std::string& path);

private:
    static void init();
    static void dbInit(SqliteDBPtr& db);
    static void createDir();

private:
    static std::once_flag m_createFlag;
    static std::mutex m_mutex;
    static std::unordered_map<std::string, std::mutex> m_mutexs;
    static std::unordered_map<std::string, std::list<SqliteDBPtr>> m_dbs;
};

}  // namespace sqlite
