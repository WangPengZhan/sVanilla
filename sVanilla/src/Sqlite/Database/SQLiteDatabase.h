#pragma once
#include <string>
#include <set>
#include <memory>
#include <mutex>

struct sqlite3;
struct sqlite3_stmt;

namespace sqlite
{

class SQLiteDatabase
{
private:
    SQLiteDatabase(const SQLiteDatabase& other) = delete;
    SQLiteDatabase& operator=(const SQLiteDatabase& other) = delete;

public:
    struct Deleter
    {
        void operator()(sqlite3* dbHandle);
    };
    using sqlitePtr = std::unique_ptr<sqlite3, Deleter>;
    using dbPtr = std::shared_ptr<SQLiteDatabase>;

    enum NextStatus
    {
        Row,
        Done,
        Error,
    };

    explicit SQLiteDatabase(const std::string& path);
    virtual ~SQLiteDatabase();

    bool isOpen() const;
    const std::string& fileName() const;

    std::set<std::string> tables();
    std::set<std::string> views();

    bool tableExists(const std::string& tableName);
    int64_t lastInsertRowid() const;

    int errorCode() const;
    int extendedErrorCode() const;
    std::string errorMsg() const;
    std::string lastError() const;

    bool execute(const std::string& sql);

    bool transaction();
    bool commit();
    bool rollback();

    sqlite3* handle() const;

protected:
    void updateLastError();

protected:
    sqlitePtr m_db;
    std::string m_fileName;
    std::string m_lastError;
};

using SqliteDBPtr = std::shared_ptr<SQLiteDatabase>;
class SqliteDbWithMutex : public SQLiteDatabase
{
public:
    using SQLiteDatabase::SQLiteDatabase;

public:
    std::recursive_mutex mutex;
};

using SqliteWithMutexPtr = std::shared_ptr<SqliteDbWithMutex>;

}  // namespace sqlite
