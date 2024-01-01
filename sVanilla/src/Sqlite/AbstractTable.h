#pragma once
#include <unordered_map>
#include <string>
#include <vector>

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
        Blob,
    };
    static std::string fieldType(FieldType eType);
    static int sqliteType(FieldType eType);

    using HeadType = std::vector<std::pair<std::string, FieldType>>;
    AbstractTable(SQLiteDatabase& db, const std::string& nameTable);
    virtual ~AbstractTable() = default;

    virtual void initTable() = 0;

    bool createTable(SQLiteDatabase& db);

    const std::string& name() const;
    void setName(const std::string& name);

    void setHeaders(const HeadType& headers);
    const HeadType& headers() const;

    void setPrimary(const std::vector<std::string>& primary);
    const std::vector<std::string>& primary() const;

    std::string createTableSql();
    std::string createTableInsertSql();
    std::string createTableSelectAllSql();

protected:
    SQLiteDatabase& m_db;
    std::string m_name;
    HeadType m_headers;
    std::vector<std::string> m_primary;
};

}  // namespace SQLite
