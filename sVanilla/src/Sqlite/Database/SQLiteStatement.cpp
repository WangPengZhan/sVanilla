#include <iostream>
#include <sqlite3.h>

#include "SQLiteStatement.h"
#include "SQLiteDatabase.h"

namespace sqlite
{

void SQLiteStatement::Deleter::operator()(sqlite3_stmt* stmt)
{
    if (stmt)
    {
        sqlite3_finalize(stmt);
    }
}

SQLiteStatement::SQLiteStatement(SQLiteDatabase& db, const std::string& query)
    : m_db(db)
    , m_sql(query)
{
    prepareStatement();
    m_colunmCount = sqlite3_column_count(handle());
}

void SQLiteStatement::clearBindings()
{
    const int ret = sqlite3_clear_bindings(handle());
}

bool SQLiteStatement::executeStep()
{
    const int ret = tryExecuteStep();
    return m_hasRow;
}

bool SQLiteStatement::reset()
{
    m_hasRow = false;
    m_done = false;
    const int ret = sqlite3_reset(handle());
    return ret == SQLITE_OK;
}

bool SQLiteStatement::hasRow() const
{
    return m_hasRow;
}

bool SQLiteStatement::isDone() const
{
    return m_done;
}

int SQLiteStatement::getIndex(const std::string& colunmName)
{
    return sqlite3_bind_parameter_index(handle(), colunmName.c_str());
}

void SQLiteStatement::bind(int index, int32_t value)
{
    const int ret = sqlite3_bind_int(handle(), index, value);
}

void SQLiteStatement::bind(int index, uint32_t value)
{
    const int ret = sqlite3_bind_int64(handle(), index, value);
}

void SQLiteStatement::bind(int index, int64_t value)
{
    const int ret = sqlite3_bind_int64(handle(), index, value);
}

void SQLiteStatement::bind(int index, double value)
{
    const int ret = sqlite3_bind_double(handle(), index, value);
}

void SQLiteStatement::bind(int index, const std::string& value)
{
    const int ret = sqlite3_bind_text(handle(), index, value.c_str(), -1, SQLITE_TRANSIENT);
}

void SQLiteStatement::bind(int index, const char* value)
{
    const int ret = sqlite3_bind_text(handle(), index, value, -1, SQLITE_TRANSIENT);
}

void SQLiteStatement::bind(int index, const void* value, const int size)
{
    const int ret = sqlite3_bind_blob(handle(), index, value, size, SQLITE_TRANSIENT);
}

void SQLiteStatement::bind(int index, const std::vector<uint8_t>& blob)
{
    const int ret = sqlite3_bind_blob(handle(), index, blob.data(), blob.size(), SQLITE_TRANSIENT);
}

void SQLiteStatement::bindNoCopy(int index, const std::string& value)
{
    const int ret = sqlite3_bind_text(handle(), index, value.c_str(), -1, SQLITE_STATIC);
}

void SQLiteStatement::bindNoCopy(int index, const char* value)
{
    const int ret = sqlite3_bind_text(handle(), index, value, -1, SQLITE_STATIC);
}

void SQLiteStatement::bindNoCopy(int index, const void* value, const int size)
{
    const int ret = sqlite3_bind_blob(handle(), index, value, size, SQLITE_STATIC);
}

void SQLiteStatement::bindNoCopy(int index, const std::vector<uint8_t>& blob)
{
    const int ret = sqlite3_bind_blob(handle(), index, blob.data(), blob.size(), SQLITE_STATIC);
}

void SQLiteStatement::bind(int index)
{
    const int ret = sqlite3_bind_null(handle(), index);
}

void SQLiteStatement::bind(const std::string& columnName, int32_t value)
{
    bind(getIndex(columnName), value);
}

void SQLiteStatement::bind(const std::string& columnName, uint32_t value)
{
    bind(getIndex(columnName), value);
}

void SQLiteStatement::bind(const std::string& columnName, int64_t value)
{
    bind(getIndex(columnName), value);
}

void SQLiteStatement::bind(const std::string& columnName, double value)
{
    bind(getIndex(columnName), value);
}

void SQLiteStatement::bind(const std::string& columnName, const std::string& value)
{
    bind(getIndex(columnName), value);
}

void SQLiteStatement::bind(const std::string& columnName, const char* value)
{
    bind(getIndex(columnName), value);
}

void SQLiteStatement::bind(const std::string& columnName, const void* value, int size)
{
    bind(getIndex(columnName), value, size);
}

void SQLiteStatement::bind(const std::string& columnName, const std::vector<uint8_t>& blob)
{
    bind(getIndex(columnName), blob);
}

void SQLiteStatement::bindNoCopy(const std::string& columnName, const std::string& value)
{
    bindNoCopy(getIndex(columnName), value);
}

void SQLiteStatement::bindNoCopy(const std::string& columnName, const char* value)
{
    bindNoCopy(getIndex(columnName), value);
}

void SQLiteStatement::bindNoCopy(const std::string& columnName, const void* value, int size)
{
    bindNoCopy(getIndex(columnName), value, size);
}

void SQLiteStatement::bindNoCopy(const std::string& columnName, const std::vector<uint8_t>& blob)
{
    bindNoCopy(getIndex(columnName), blob);
}

void SQLiteStatement::bind(const std::string& columnName)
{
    bind(getIndex(columnName));
}

std::string SQLiteStatement::expandedSQL() const
{
    char* expanded = sqlite3_expanded_sql(handle());
    std::string strExpanded(expanded);
    sqlite3_free(expanded);
    return strExpanded;
}

int SQLiteStatement::columnCount() const
{
    return m_colunmCount;
}

SqliteColumn SQLiteStatement::column(int col) const
{
    if (m_colunmsMap.find(col) == m_colunmsMap.end())
    {
        m_colunmsMap[col] = getColunmFromStmt(col);
    }

    return m_colunmsMap.at(col);
}

SqliteColumn SQLiteStatement::column(const std::string& colName) const
{
    return column(columnIndex(colName));
}

bool SQLiteStatement::isColunmNull(int col) const
{
    return (SQLITE_NULL == sqlite3_column_type(handle(), col));
}

bool SQLiteStatement::isColunmNull(const std::string& colName) const
{
    return isColunmNull(columnIndex(colName));
}

std::string SQLiteStatement::colunmName(int col) const
{
    return sqlite3_column_name(handle(), col);
}

std::string SQLiteStatement::originColunmName(int col) const
{
    return sqlite3_column_origin_name(handle(), col);
}

int SQLiteStatement::columnIndex(const std::string& colName) const
{
    if (m_colunmIndexMap.empty())
    {
        for (int i = 0; i < m_colunmCount; ++i)
        {
            std::string colName = sqlite3_column_name(handle(), i);
            m_colunmIndexMap[colName] = i;
        }
    }

    if (m_colunmIndexMap.find(colName) == m_colunmIndexMap.end())
    {
        return -1;
    }

    return m_colunmIndexMap.at(colName);
}

int SQLiteStatement::getBindParameterCount() const
{
    return sqlite3_bind_parameter_count(handle());
}

int SQLiteStatement::errorCode() const
{
    return db().errorCode();
}

int SQLiteStatement::extendedErrorCode() const
{
    return db().extendedErrorCode();
}

std::string SQLiteStatement::errorMsg() const
{
    return db().errorMsg();
}

const SQLiteDatabase& SQLiteStatement::db() const
{
    return m_db;
}

sqlite3* SQLiteStatement::dbHandle() const
{
    return m_db.handle();
}

sqlite3_stmt* SQLiteStatement::handle() const
{
    return m_stmt.get();
}

void SQLiteStatement::prepareStatement()
{
    sqlite3_stmt* stmt = nullptr;
    const char* pzTail = nullptr;
    const int ret = sqlite3_prepare_v2(dbHandle(), m_sql.c_str(), m_sql.size(), &stmt, &pzTail);
    m_stmt.reset(stmt);
}

int SQLiteStatement::tryExecuteStep() noexcept
{
    if (m_done)
    {
        return SQLITE_MISUSE;
    }

    const int ret = sqlite3_step(handle());
    if (SQLITE_ROW == ret)
    {
        m_hasRow = true;
    }
    else
    {
        m_hasRow = false;
        m_done = SQLITE_DONE == ret;
    }

    return ret;
}

SqliteColumn SQLiteStatement::getColunmFromStmt(int col) const
{
    std::string colName = sqlite3_column_name(handle(), col);
    std::string colOriginName = sqlite3_column_origin_name(handle(), col);
    SqliteColumnValue value;

    int type = sqlite3_column_type(handle(), col);
    switch (type)
    {
    case SQLITE_INTEGER:
        value = sqlite3_column_int64(handle(), col);
        break;
    case SQLITE_FLOAT:
        value = sqlite3_column_double(handle(), col);
        break;
    case SQLITE_TEXT:
    {
        auto data = (const char*)(sqlite3_column_text(handle(), col));
        value = std::string(data);
        break;
    }
    case SQLITE_BLOB:
    {
        auto len = sqlite3_column_bytes(handle(), col);
        auto data = static_cast<const uint8_t*>(sqlite3_column_blob(handle(), col));
        std::vector<uint8_t> blob;
        blob.resize(len);
        memccpy(blob.data(), data, 0, len);
        value = blob;
        break;
    }
    case SQLITE_NULL:
        break;
    default:
    {
        int len = sqlite3_column_bytes(handle(), col);
        auto data = static_cast<const char*>(sqlite3_column_blob(handle(), col));
        value = std::string(data, len);
        break;
    }
    }
    return SqliteColumn(value, col, colName, colOriginName);
}

}  // namespace sqlite
