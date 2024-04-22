#pragma once
#include <variant>
#include <vector>
#include <string>

#include "BaseInfo.h"
#include "Sqlite/Database/SQLiteDatabase.h"
#include "Sqlite/SqlComposer/ConditionWrapper.h"

namespace sqlite
{
class SqliteUtil
{
    static bool createIndexIfNotExists(const SqliteDBPtr& db, const std::string& tableName, const BaseTableStructInfo& tableStruct);
    static bool renameTable(const SqliteDBPtr& db, const std::string& oldName, const std::string& newName);
    static bool deleteTable(const SqliteDBPtr& db, const std::string& tableName);
    static bool dropTableIfExists(const SqliteDBPtr& db, const std::string& tableName);

    static std::string indexName(const std::string& tag, const std::vector<std::string>& colNames);
    static bool createIndexIfNotExists(const SqliteDBPtr& db, const std::string& tableName, const std::string& indexName,
                                       const std::vector<std::string>& colNames);

    static bool createViewIfNotExist(const SqliteDBPtr& db, const std::string& viewName, const std::string& tableName,
                                     const ConditionWrapper& queryWrapper = {}, const std::string& indexed = {});
    static bool dropViewIfExists(const SqliteDBPtr& db, const std::string& tableName);

    static std::string getQuerySql(const std::string& tableName, const ConditionWrapper& condition = {});
    static std::string getQuerySql(const std::string& tableName, int index, int num, const ConditionWrapper& condition = {});
};

}  // namespace sqlite
