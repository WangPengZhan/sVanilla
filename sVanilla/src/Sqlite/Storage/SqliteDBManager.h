#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>

#include "Sqlite/Database/SQLiteDatabase.h"

namespace sqlite
{
extern std::string dbPath;

class SqliteDBManager
{
public:
    static void setDbPath(const std::string& path);
    static SqliteDBPtr createDBPtr(const std::string& path, bool createNew = true);
    static SqliteDBPtr writeDBPtr(const std::string& path);

    static SqliteWithMutexPtr createDBWithMutexPtr(const std::string& path, bool createNew = true);
    static SqliteWithMutexPtr writeDBWithMutexPtr(const std::string& path);

private:
    static void init();
    static void dbInit(SQLiteDatabase& db);
    static void dbInit(SqliteDBPtr& db);
    static void dbInit(SqliteWithMutexPtr& db);

    static void createDir();

private:
    static std::once_flag m_createFlag;
    static std::mutex m_mutex;
    static std::unordered_map<std::string, SqliteWithMutexPtr> m_defaultWriteDbWtihMutexs;
    static std::unordered_map<std::string, SqliteDBPtr> m_defaultWriteDbs;
};

}  // namespace sqlite
