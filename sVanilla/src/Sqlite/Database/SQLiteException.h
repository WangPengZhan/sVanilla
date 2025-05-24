#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_map>

#include "SQLiteColunm.h"
#include "SQLiteDatabase.h"

struct sqlite3;
struct sqlite3_stmt;

namespace sqlite
{

class SQLiteException : public std::runtime_error
{
public:
    explicit SQLiteException(const char* message, int ret);
    explicit SQLiteException(const std::string& message, int ret);
    explicit SQLiteException(const char* message);
    explicit SQLiteException(const std::string& message);
    explicit SQLiteException(sqlite3* pSQLite);
    explicit SQLiteException(sqlite3* pSQLite, int ret);
    explicit SQLiteException(const SQLiteDatabase::sqlitePtr& spSQLite, int ret);
    explicit SQLiteException(const SQLiteDatabase::sqlitePtr& spSQLite);

    virtual ~SQLiteException() noexcept = default;

    int errorCode() const noexcept;
    int extendedErrorCode() const noexcept;
    std::string getErrorString() const;

private:
    int m_errorCode;
    int m_extendedErrorCode;
};

}  // namespace sqlite
