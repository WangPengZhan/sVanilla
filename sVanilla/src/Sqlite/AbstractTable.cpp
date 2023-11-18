#include "AbstractTable.h"

namespace SQLite
{

std::string AbstractTable::fieldType(FieldType eType)
{
    switch (eType)
    {
    case Integer:
        return "INTEGER";
    case Text:
        return "TEXT";
    case Double:
        return "DOUBLE";
    case Boolean:
        return "BOOLEAN";
    case Blob:
        return "BLOB";
    default:
        break;
    }

    return std::string(" ");
}

int AbstractTable::sqliteType(FieldType eType)
{    
    switch (eType)
    {
    case Integer:
        return 1;
    case Text:
        return 3;
    case Double:
        return 2;
    case Boolean:
        return 1;
    case Blob:
        return 4;
    default:
        break;
    }

    return -1;
}

AbstractTable::AbstractTable(SQLiteDatabase &db, const std::string &nameTable)
    : m_db(db)
    , m_name(nameTable)
{
}

const std::string& AbstractTable::name() const
{
    return m_name;
}

void AbstractTable::setName(const std::string& name)
{
    m_name = name;
}

void AbstractTable::setHeaders(const HeadType &headers)
{
    m_headers = headers;
}

const AbstractTable::HeadType &AbstractTable::headers() const
{
    return m_headers;
};

void AbstractTable::setPrimary(const std::vector<std::string> &primary)
{
    m_primary = primary;
}

const std::vector<std::string> &AbstractTable::primary() const
{
    return m_primary;
}

std::string AbstractTable::createTableSql()
{
    if (m_name.empty() || m_headers.empty() || m_primary.empty())
    {
        return std::string();
    }

    std::string strSql = "CREATE TABLE IF NOT EXISTS " + m_name + "(";
    for (const auto& header : m_headers)
    {
        strSql += (header.first + " " + fieldType(header.second) + ", ");
    }

    strSql += "PRIMARY KEY(";
    for (const auto& primary : m_primary)
    {
        strSql += primary;
        if (primary != m_primary.back())
        {
            strSql += ", ";
        }
    }
    strSql += ")";
    strSql += ")";

    return strSql;
}

std::string AbstractTable::createTableInsertSql()
{
    if (m_name.empty() || m_headers.empty() || m_primary.empty())
    {
        return std::string();
    }

    std::string strSql = "INSERT INTO " + m_name + "(";
    std::string strBind;
    for (const auto& header : m_headers)
    {
        strSql += header.first;
        strBind += "?";

        if (m_headers.back() != header)
        {
            strSql += ", ";
            strBind += ", ";
        }
    }

    strSql += ") values (";
    strSql += strBind;
    strSql += ")";

    return strSql;
}

std::string AbstractTable::createTableSelectAllSql()
{
    if (m_name.empty() || m_headers.empty() || m_primary.empty())
    {
        return std::string();
    }

    return "SELECT * FROM " + m_name;
}

}  // namespace SQLite
