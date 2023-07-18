#include <sqlite3.h>

#include "SQLiteDatabase.h"
#include "SQLiteLog.h"


namespace SQLite
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

auto ListCallback = [](void* ptr, int, char** data, char**) 
{
    static_cast<QStringList*>(ptr)->append(QString::fromUtf8(data[0]));
    return 0;
};


SQLiteDatabase::SQLiteDatabase(const std::string& path)
    : m_db(nullptr)
    , m_stmt(nullptr)
{
    if (sqlite3_initialize() != SQLITE_OK) 
    {
        SQLITE_LOG_ERROR("sqlite3_initialize error: {}", m_lastError.c_str());
        return;
    }

    SQLITE_LOG_INFO("sqlite3 path: {}", path.c_str());
    if (sqlite3_open(path.c_str(), &m_db) != SQLITE_OK) 
    {
        updateLastError();
        close();
    }
}

SQLiteDatabase::~SQLiteDatabase()
{
    finalize();
    close();
}

bool SQLiteDatabase::isOpen() const
{
    return m_db != nullptr;
}

QStringList SQLiteDatabase::tables()
{
    if (m_db == nullptr) 
    {
        return {};
    }

    QStringList list;
    char*       errmsg = nullptr;
    const int   rc = sqlite3_exec(m_db, ListTablesSql, ListCallback, &list, &errmsg);

    if (rc != SQLITE_OK) 
    {
        if (errmsg) 
        {
            m_lastError = errmsg;
            sqlite3_free(errmsg);
            SQLITE_LOG_ERROR("tables error: {}",
                             m_lastError.c_str());
        }

        return {};
    }

    return list;
}

QStringList SQLiteDatabase::views()
{
    if (m_db == nullptr) 
    {
        return {};
    }

    QStringList list;
    char*       errmsg = nullptr;
    const int   rc = sqlite3_exec(m_db, ListViewsSql, ListCallback, &list, &errmsg);

    if (rc != SQLITE_OK) 
    {
        if (errmsg) 
        {
            m_lastError = errmsg;
            sqlite3_free(errmsg);
            SQLITE_LOG_ERROR("views error: {}", m_lastError.c_str());
        }

        return {};
    }

    return list;
}

bool SQLiteDatabase::prepare(const std::string& sql)
{
    if (m_db == nullptr) 
    {
        return false;
    }

    if (m_stmt != nullptr) 
    {
        finalize();
    }

    m_lastError.clear();

    sqlite3_mutex_enter(sqlite3_db_mutex(m_db));
    const char* pzTail = nullptr;
    const int   res = sqlite3_prepare_v2(m_db, sql.data(),
                                         sql.size(), &m_stmt, &pzTail);
    sqlite3_mutex_leave(sqlite3_db_mutex(m_db));

    if (res != SQLITE_OK) 
    {
        updateLastError();
        finalize();
        return false;
    }

    if (pzTail && !std::string(pzTail).empty()) 
    {
        updateLastError();
        finalize();
        return false;
    }

    return true;
}

bool SQLiteDatabase::next()
{
    if (m_stmt == nullptr) 
    {
        return false;
    }

    sqlite3_mutex_enter(sqlite3_db_mutex(m_db));
    const int res = sqlite3_step(m_stmt);
    sqlite3_mutex_leave(sqlite3_db_mutex(m_db));

    switch (res) 
    {
    case SQLITE_ROW:
        return true;
    case SQLITE_DONE:
    case SQLITE_CONSTRAINT:
    case SQLITE_ERROR:
    case SQLITE_MISUSE:
    case SQLITE_BUSY:
    default:
        updateLastError();
    }

    return false;
}

bool SQLiteDatabase::execute(const std::string& sql)
{
    if (m_db == nullptr) 
    {
        return false;
    }

    m_lastError.clear();

    char*     errmsg = nullptr;
    const int rc = sqlite3_exec(m_db, sql.data(), nullptr, nullptr, &errmsg);

    if (rc != SQLITE_OK) 
    {
        if (errmsg) 
        {
            m_lastError = errmsg;
            sqlite3_free(errmsg);
            SQLITE_LOG_ERROR(
                "sqlite3_exec error: {}, sql:{} ", m_lastError.c_str(), sql.c_str());
        }
        return false;
    }

    return true;
}

std::any SQLiteDatabase::value(int index) const
{
    assert(index >= 0);

    if (index >= sqlite3_data_count(m_stmt)) 
    {
        SQLITE_LOG_WARN("value index out of range: {}", index);
        return std::any();
    }

    sqlite3_mutex_enter(sqlite3_db_mutex(m_db));
    const int type = sqlite3_column_type(m_stmt, index);
    std::any ret;

    switch (type) 
    {
    case SQLITE_INTEGER:
        ret = sqlite3_column_int64(m_stmt, index);
        break;
    case SQLITE_NULL:
        ret = std::string();
        break;
    default:
        ret = std::string((const char*)(sqlite3_column_text(m_stmt, index)));
        break;
    }
    sqlite3_mutex_leave(sqlite3_db_mutex(m_db));

    return ret;
}

bool SQLiteDatabase::bind(int index, int type, const std::any& value)
{
    assert(index >= 0);

    if (index >= sqlite3_bind_parameter_count(m_stmt))
    {
        SQLITE_LOG_WARN("bind index out of range: {}", index);
        return false;
    }

    if (type != SQLITE_NULL && !value.has_value()) 
    {
        SQLITE_LOG_WARN("bind value is empty, index: {}, type: {}", index, type);
        return false;
    }

    int nRet = SQLITE_ERROR;
    sqlite3_mutex_enter(sqlite3_db_mutex(m_db));
    switch (type) 
    {
    case SQLITE_INTEGER:
        nRet = sqlite3_bind_int64(m_stmt, index, std::any_cast<sqlite_int64>(value));
        break;
    case SQLITE_FLOAT:
        nRet = sqlite3_bind_double(m_stmt, index, std::any_cast<double>(value));
        break;
    case SQLITE_NULL: 
        nRet = sqlite3_bind_null(m_stmt, index);
        break;
    default:
        nRet = sqlite3_bind_text(m_stmt, index,
                                 std::any_cast<std::string>(value).c_str(), -1,
                                 SQLITE_TRANSIENT);
        break;
    }
    sqlite3_mutex_leave(sqlite3_db_mutex(m_db));

    return nRet == SQLITE_OK;
}

std::string SQLiteDatabase::lastError() const
{
    return m_lastError;
}

void SQLiteDatabase::close()
{
    sqlite3_close(m_db);
    m_db = nullptr;
}

void SQLiteDatabase::finalize()
{
    sqlite3_finalize(m_stmt);
    m_stmt = nullptr;
}

void SQLiteDatabase::updateLastError()
{
    if (m_db == nullptr) 
    {
        return;
    }

    m_lastError = sqlite3_errmsg(m_db);
    SQLITE_LOG_ERROR("sqlite3 errmsg: {}", m_lastError.c_str());
}

sqlite3* SQLiteDatabase::handle() const
{
    return m_db;
}

} // namespace SQLite

