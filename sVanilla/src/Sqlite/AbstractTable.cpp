#include "AbstractTable.h"

namespace SQLite
{

AbstractTable::AbstractTable(SQLiteDatabase& db, const std::string& nameTable)
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

void AbstractTable::setHeaders(const HeadType& headers)
{
    m_headers = headers;
}

const AbstractTable::HeadType& AbstractTable::headers() const
{
    return m_headers;
};

}  // namespace SQLite
