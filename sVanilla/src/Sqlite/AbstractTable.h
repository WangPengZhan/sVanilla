#pragma once
#include "Sqlite/SQLiteDatabase.h"

namespace SQLite
{

class AbstractTable
{
public:
    AbstractTable(SQLiteDatabase& db, const std::string& nameTable);
    virtual ~AbstractTable() = default;

    virtual void initTable() = 0;

    const std::string name();
    const std::string& name() const;
    void setName(const std::string& name);

protected:
    SQLiteDatabase& m_db;
    std::string m_name;
};

}  // namespace SQLite

