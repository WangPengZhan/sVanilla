#pragma once
#include <unordered_map>

#include "Sqlite/SQLiteDatabase.h"

namespace SQLite
{
class AbstractTable
{
public:
    enum FieldType
    {
        Integer,
        Text,
        Double,
        Boolean,
    };

    using HeadType = std::unordered_map<std::string, FieldType>;
    AbstractTable(SQLiteDatabase& db, const std::string& nameTable);
    virtual ~AbstractTable() = default;

    virtual void initTable() = 0;

    bool createTable(SQLiteDatabase& db);

    const std::string& name() const;
    void setName(const std::string& name);

    void setHeaders(const HeadType& headers);
    const HeadType& headers() const;

protected:
    SQLiteDatabase& m_db;
    std::string m_name;
    HeadType m_headers;
};

}  // namespace SQLite
