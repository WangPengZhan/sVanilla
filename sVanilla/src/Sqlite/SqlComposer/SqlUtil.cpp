#include <sstream>
#include <algorithm>

#include "SqlUtil.h"

namespace sqlite
{
bool SqliteUtil::createIndexIfNotExists(const SqliteDBPtr& db, const std::string& tableName, const BaseTableStructInfo& tableStruct)
{
    if (!db)
    {
        return false;
    }

    if (db->tableExists(tableName))
    {
        return false;
    }

    auto tableSql = tableStruct.createSql();
    if (tableSql.empty())
    {
        return false;
    }

    std::stringstream ss;
    ss << "CREATE TABLE IF NOT EXISTS " << tableName;
    ss << "(" << tableSql << ")";

    std::string sql = ss.str();
    return db->execute(sql);
}

bool SqliteUtil::renameTable(const SqliteDBPtr& db, const std::string& oldName, const std::string& newName)
{
    if (!db)
    {
        return false;
    }

    if (db->tableExists(newName))
    {
        return false;
    }

    std::string sql = "ALTER TABLE " + oldName + " RENAME" + newName;
    return db->execute(sql);
}

bool SqliteUtil::deleteTable(const SqliteDBPtr& db, const std::string& tableName)
{
    if (!db)
    {
        return false;
    }

    std::string sql = "DELETE FROM " + tableName;
    return db->execute(sql);
}

bool SqliteUtil::dropTableIfExists(const SqliteDBPtr& db, const std::string& tableName)
{
    if (!db)
    {
        return false;
    }

    std::string sql = "DROP TABLE IF EXISTS " + tableName;
    return db->execute(sql);
}

std::string SqliteUtil::indexName(const std::string& tag, const std::vector<std::string>& colNames)
{
    std::stringstream ss;
    ss << tag << "_";
    if (!colNames.empty())
    {
        ss << colNames[0];
        for (int i = 1; i < colNames.size(); ++i)
        {
            ss << "_" << colNames[i];
        }
    }

    ss << "_index";

    return ss.str();
}

bool SqliteUtil::createIndexIfNotExists(const SqliteDBPtr& db, const std::string& tableName, const std::string& indexName,
                                        const std::vector<std::string>& colNames)
{
    if (!db)
    {
        return false;
    }

    if (colNames.empty())
    {
        return false;
    }

    std::stringstream ss;
    ss << "CREATE INDEX IF NOT EXISTS " << indexName;
    ss << " ON " << tableName;
    ss << "(" << colNames[0];
    for (int i = 1; i < colNames.size(); ++i)
    {
        ss << ", " << colNames[i];
    }
    ss << ")";

    std::string sql = ss.str();
    return db->execute(sql);
}

bool SqliteUtil::createViewIfNotExist(const SqliteDBPtr& db, const std::string& viewName, const std::string& tableName, const ConditionWrapper& condition,
                                      const std::string& indexed)
{
    if (!db)
    {
        return false;
    }

    std::stringstream ss;

    ss << "CREATE VIEW IF NOT EXISTS " << viewName;
    ss << " AS SELECT * FROM " << tableName;
    if (!indexed.empty())
    {
        ss << " INDEXED BY " << indexed;
    }
    ss << " " << condition.conditionString();

    std::string sql = ss.str();
    return db->execute(sql);
}

bool SqliteUtil::dropViewIfExists(const SqliteDBPtr& db, const std::string& viewName)
{
    if (!db)
    {
        return false;
    }

    std::string sql = "DROP VIEW IF EXISTS " + viewName;
    return db->execute(sql);
}

std::string SqliteUtil::getQuerySql(const std::string& tableName, const ConditionWrapper& condition)
{
    std::stringstream ss;
    ss << "SELECT * FROM " << tableName;
    ss << " " << condition.conditionString();

    return ss.str();
}

std::string SqliteUtil::getQuerySql(const std::string& tableName, int index, int num, const ConditionWrapper& condition)
{
    std::stringstream ss;
    ss << "SELECT * FROM " << tableName;
    ss << " " << condition.conditionString();
    ss << " LIMIT " << index << " , " << num;

    return ss.str();
}

}  // namespace sqlite
