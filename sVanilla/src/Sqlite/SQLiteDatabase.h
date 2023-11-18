#pragma once
#include <string>
#include <any>
#include <set>

#include "SQLiteUtility.h"

struct sqlite3;
struct sqlite3_stmt;

namespace SQLite
{

class SQLiteDatabase
{
private:
    SQLiteDatabase(const SQLiteDatabase& other) = delete;
    SQLiteDatabase& operator=(const SQLiteDatabase& other) = delete;

public:
    enum NextStatus
    {
        Row,
        Done,
        Error,
    };

    explicit SQLiteDatabase(const std::string& path);
    virtual ~SQLiteDatabase();

    bool isOpen() const;

    std::set<std::string> tables();
    std::set<std::string> views();

    bool prepare(const std::string& sql);
    NextStatus next();
    bool execute(const std::string& sql);

    bool prepare(const std::string& sql, SQLiteStmtPtr& stmt);
    NextStatus next(SQLiteStmtPtr stmt);

    bool transaction();
    bool commit();
    bool rollback();

    std::any value(int index) const;
    bool bind(int index, int type, const std::any& value);

    std::any value(int index, SQLiteStmtPtr stmt) const;
    bool bind(int index, int type, const std::any& value, SQLiteStmtPtr stmt);

    std::string lastError() const;

    sqlite3* handle() const;

protected:
    void close();
    void finalize();
    void updateLastError();

    sqlite3* m_db;
    sqlite3_stmt* m_stmt;
    std::string m_lastError;
};

}  // namespace SQLite
