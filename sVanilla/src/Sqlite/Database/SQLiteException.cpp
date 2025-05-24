#include <iostream>
#include <cstring>

#include <sqlite3.h>

#include "SQLiteStatement.h"
#include "SQLiteDatabase.h"
#include "SQLiteException.h"

namespace sqlite
{
SQLiteException::SQLiteException(const char* message, int ret)
    : std::runtime_error(message)
    , m_errorCode(ret)
    , m_extendedErrorCode(-1)
{
}

SQLiteException::SQLiteException(const std::string& message, int ret)
    : std::runtime_error(message.c_str())
    , m_errorCode(ret)
    , m_extendedErrorCode(-1)
{
}

SQLiteException::SQLiteException(const char* message)
    : SQLiteException(message, -1)
{
}

SQLiteException::SQLiteException(const std::string& message)
    : SQLiteException(message.c_str(), -1)
{
}

SQLiteException::SQLiteException(sqlite3* apSQLite)
    : SQLiteException(sqlite3_errmsg(apSQLite), sqlite3_errcode(apSQLite))
{
    m_extendedErrorCode = sqlite3_extended_errcode(apSQLite);
}

SQLiteException::SQLiteException(sqlite3* pSQLite, int ret)
    : SQLiteException(sqlite3_errmsg(pSQLite), ret)
{
    m_extendedErrorCode = sqlite3_extended_errcode(pSQLite);
}

SQLiteException::SQLiteException(const SQLiteDatabase::sqlitePtr& spSQLite, int ret)
    : SQLiteException(sqlite3_errmsg(spSQLite.get()), ret)
{
}

SQLiteException::SQLiteException(const SQLiteDatabase::sqlitePtr& spSQLite)
    : SQLiteException(spSQLite.get())
{
}

int SQLiteException::errorCode() const noexcept
{
    return m_errorCode;
}

int SQLiteException::extendedErrorCode() const noexcept
{
    return m_extendedErrorCode;
}
std::string SQLiteException::getErrorString() const
{
    return sqlite3_errstr(m_errorCode);
}

}  // namespace sqlite
