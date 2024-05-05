#include <iostream>
#include <sqlite3.h>

#include "SQLiteDatabase.h"
#include "SQLiteStatement.h"
#include "Sqlite/SQLiteLog.h"

namespace sqlite
{
constexpr char ListTablesSql[] = "SELECT name"
                                 "  FROM"
                                 "    (SELECT * FROM sqlite_master UNION ALL"
                                 "    SELECT * FROM sqlite_temp_master)"
                                 "  WHERE type='table'"
                                 "  ORDER BY name";
constexpr char ListViewsSql[] = "SELECT name"
                                "  FROM"
                                "    (SELECT * FROM sqlite_master UNION ALL"
                                "    SELECT * FROM sqlite_temp_master)"
                                "  WHERE type='view'"
                                "  ORDER BY name";

auto ListCallback = [](void* ptr, int, char** data, char**) {
    static_cast<std::set<std::string>*>(ptr)->insert(data[0]);
    return 0;
};

void SQLiteDatabase::Deleter::operator()(sqlite3* dbHandle)
{
    if (dbHandle)
    {
        sqlite3_close(dbHandle);
    }
}

SQLiteDatabase::SQLiteDatabase(const std::string& path)
    : m_db(nullptr)
    , m_fileName(path)
{
    if (sqlite3_initialize() != SQLITE_OK)
    {
        updateLastError();
        SQLITE_LOG_ERROR("sqlite3_initialize error: {}", m_lastError);
        return;
    }

    SQLITE_LOG_INFO("sqlite3 path: {}", path);
    sqlite3* db = nullptr;
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK)
    {
        updateLastError();
    }

    m_db.reset(db);
}

SQLiteDatabase::~SQLiteDatabase()
{
}

bool SQLiteDatabase::isOpen() const
{
    return m_db != nullptr;
}

const std::string& SQLiteDatabase::fileName() const
{
    return m_fileName;
}

std::set<std::string> SQLiteDatabase::tables()
{
    if (m_db == nullptr)
    {
        return {};
    }

    std::set<std::string> list;
    char* errmsg = nullptr;
    const int rc = sqlite3_exec(handle(), ListTablesSql, ListCallback, &list, &errmsg);

    if (rc != SQLITE_OK)
    {
        if (errmsg)
        {
            m_lastError = errmsg;
            sqlite3_free(errmsg);
            SQLITE_LOG_ERROR("tables error: {}", m_lastError);
        }

        return {};
    }

    return list;
}

std::set<std::string> SQLiteDatabase::views()
{
    if (m_db == nullptr)
    {
        return {};
    }

    std::set<std::string> list;
    char* errmsg = nullptr;
    const int rc = sqlite3_exec(handle(), ListViewsSql, ListCallback, &list, &errmsg);

    if (rc != SQLITE_OK)
    {
        if (errmsg)
        {
            m_lastError = errmsg;
            sqlite3_free(errmsg);
            SQLITE_LOG_ERROR("views error: {}", m_lastError);
        }

        return {};
    }

    return list;
}

bool SQLiteDatabase::tableExists(const std::string& tableName)
{
    SQLiteStatement stmt(*this, "SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?");
    stmt.bind(1, tableName);
    stmt.executeStep();
    return (1 == stmt.column(0).getInt());
}

int64_t SQLiteDatabase::lastInsertRowid() const
{
    return sqlite3_last_insert_rowid(handle());
}

int SQLiteDatabase::errorCode() const
{
    return sqlite3_errcode(handle());
}

int SQLiteDatabase::extendedErrorCode() const
{
    return sqlite3_extended_errcode(handle());
}

std::string SQLiteDatabase::errorMsg() const
{
    return sqlite3_errmsg(handle());
}

bool SQLiteDatabase::execute(const std::string& sql)
{
    if (m_db == nullptr)
    {
        return false;
    }

    m_lastError.clear();

    char* errmsg = nullptr;
    const int rc = sqlite3_exec(handle(), sql.data(), nullptr, nullptr, &errmsg);

    if (rc != SQLITE_OK)
    {
        if (errmsg)
        {
            m_lastError = errmsg;
            sqlite3_free(errmsg);
            SQLITE_LOG_ERROR("sqlite3_exec error: {}, sql:{} ", m_lastError, sql);
        }
        return false;
    }

    return true;
}

bool SQLiteDatabase::transaction()
{
    return execute("BEGIN;");
}

bool SQLiteDatabase::commit()
{
    return execute("COMMIT;");
}

bool SQLiteDatabase::rollback()
{
    return execute("ROLLBACK;");
}

std::string SQLiteDatabase::lastError() const
{
    return m_lastError;
}

void SQLiteDatabase::updateLastError()
{
    if (m_db == nullptr)
    {
        return;
    }

    m_lastError = sqlite3_errmsg(m_db.get());
    SQLITE_LOG_ERROR("sqlite3 errmsg: {}", m_lastError);
}

sqlite3* SQLiteDatabase::handle() const
{
    return m_db.get();
}

}  // namespace sqlite
