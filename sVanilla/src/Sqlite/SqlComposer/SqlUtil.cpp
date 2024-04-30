#include <sstream>
#include <algorithm>

#include "SqlUtil.h"

namespace sqlite
{
bool SqliteUtil::createTableIfNotExists(SQLiteDatabase& db, const std::string& tableName, const BaseTableStructInfo& tableStruct)
{
    if (tableName.empty())
    {
        return false;
    }

    if (db.tableExists(tableName))
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
    return db.execute(sql);
}

bool SqliteUtil::createTableIfNotExists(const SqliteDBPtr& db, const std::string& tableName, const BaseTableStructInfo& tableStruct)
{
    if (!db)
    {
        return false;
    }

    return createTableIfNotExists(*db.get(), tableName, tableStruct);
}

bool SqliteUtil::createTableIfNotExists(const SqliteWithMutexPtr& db, const std::string& tableName, const BaseTableStructInfo& tableStruct)
{
    if (!db)
    {
        return false;
    }

    std::lock_guard lk(db->mutex);
    return createTableIfNotExists(*db.get(), tableName, tableStruct);
}

bool SqliteUtil::renameTable(const SqliteWithMutexPtr& db, const std::string& oldName, const std::string& newName)
{
    if (!db || oldName.empty() || newName.empty())
    {
        return false;
    }

    std::lock_guard lk(db->mutex);
    if (db->tableExists(newName))
    {
        return false;
    }

    std::string sql = "ALTER TABLE " + oldName + " RENAME" + newName;
    return db->execute(sql);
}

bool SqliteUtil::deleteTable(const SqliteWithMutexPtr& db, const std::string& tableName)
{
    if (!db || tableName.empty())
    {
        return false;
    }

    std::string sql = "DELETE FROM " + tableName;
    std::lock_guard lk(db->mutex);
    return db->execute(sql);
}

bool SqliteUtil::dropTableIfExists(const SqliteWithMutexPtr& db, const std::string& tableName)
{
    if (!db || tableName.empty())
    {
        return false;
    }

    std::string sql = "DROP TABLE IF EXISTS " + tableName;
    std::lock_guard lk(db->mutex);
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

bool SqliteUtil::createIndexIfNotExists(const SqliteWithMutexPtr& db, const std::string& tableName, const std::string& indexName,
                                        const std::vector<std::string>& colNames)
{
    if (!db || tableName.empty() || indexName.empty() || colNames.empty())
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
    std::lock_guard lk(db->mutex);
    return db->execute(sql);
}

bool SqliteUtil::createViewIfNotExist(const SqliteWithMutexPtr& db, const std::string& viewName, const std::string& tableName,
                                      const ConditionWrapper& condition, const std::string& indexed)
{
    if (!db || viewName.empty() || tableName.empty())
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
    std::lock_guard lk(db->mutex);
    return db->execute(sql);
}

bool SqliteUtil::dropViewIfExists(const SqliteWithMutexPtr& db, const std::string& viewName)
{
    if (!db || viewName.empty())
    {
        return false;
    }

    std::string sql = "DROP VIEW IF EXISTS " + viewName;
    std::lock_guard lk(db->mutex);
    return db->execute(sql);
}

bool SqliteUtil::deleteEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const ConditionWrapper& condition)
{
    if (!db || tableName.empty())
    {
        return false;
    }

    std::string sql = "DELETE FROM " + tableName + " " + condition.conditionString();
    std::lock_guard lk(db->mutex);
    return db->execute(sql);
}

int64_t SqliteUtil::countEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const ConditionWrapper& condition)
{
    if (!db || tableName.empty())
    {
        return -1;
    }
    std::string sql = "SELECT COUNT(1) FROM " + tableName + " ";
    sql += condition.prepareConditionString();

    std::lock_guard lk(db->mutex);
    SQLiteStatement stmt(*db, sql);
    condition.bind(stmt);
    if (stmt.executeStep())
    {
        return stmt.column(0);
    }

    return -1;
}

std::vector<std::string> SqliteUtil::queryDistinctCol(const SqliteWithMutexPtr& db, const std::string& tableName, const ColumnInfo& metaInfo,
                                                      const ConditionWrapper& condition)
{
    if (!db || tableName.empty() || metaInfo.colunmName().empty())
    {
        return {};
    }

    auto colNames = condition.colunmNames();
    colNames.insert(metaInfo.colunmName());

    std::stringstream ss;
    ss << "SELECT DISTINCT " << metaInfo.colunmName();
    ss << " FROM ( SELECT " << stringJoin(colNames.begin(), colNames.end(), ", ");
    ss << " FROM " << tableName << ")";
    ss << condition.prepareConditionString();

    std::string sql = ss.str();
    std::vector<std::string> res;

    std::lock_guard lk(db->mutex);
    SQLiteStatement stmt(*db, sql);
    condition.bind(stmt);
    while (stmt.executeStep())
    {
        res.emplace_back(stmt.column(0).getString());
    }

    return res;
}

}  // namespace sqlite
