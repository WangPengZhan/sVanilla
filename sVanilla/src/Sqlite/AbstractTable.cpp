#include "AbstractTable.h"

namespace SQLite
{

AbstractTable::AbstractTable(SQLiteDatabase& db, const std::string& nameTable)
    : m_db(db)
    , m_name(nameTable)
{
}

const std::string AbstractTable::name()
{
    return m_name;
}

const std::string& AbstractTable::name() const
{
    return m_name;
}

void AbstractTable::setName(const std::string& name)
{
    m_name = name;
}

}  // namespace SQLite
