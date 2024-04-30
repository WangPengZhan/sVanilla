#pragma once
#include <variant>
#include <vector>
#include <string>

#include "BaseInfo.h"
#include "Sqlite/Database/SQLiteDatabase.h"
#include "Sqlite/SqlComposer/ConditionWrapper.h"

namespace sqlite
{

template <typename InputIter>
std::string stringJoin(InputIter begin, InputIter end, const std::string& joined = " ")
{
    std::string result;
    for (auto it = begin; it != end; ++it)
    {
        if (it != begin)
        {
            result += joined;
            result += *it;
        }
        else
        {
            result += *it;
        }
    }

    return result;
}

class SqliteUtil
{
public:
    static bool createTableIfNotExists(SQLiteDatabase& db, const std::string& tableName, const BaseTableStructInfo& tableStruct);
    static bool createTableIfNotExists(const SqliteDBPtr& db, const std::string& tableName, const BaseTableStructInfo& tableStruct);
    static bool createTableIfNotExists(const SqliteWithMutexPtr& db, const std::string& tableName, const BaseTableStructInfo& tableStruct);
    static bool renameTable(const SqliteWithMutexPtr& db, const std::string& oldName, const std::string& newName);
    static bool deleteTable(const SqliteWithMutexPtr& db, const std::string& tableName);
    static bool dropTableIfExists(const SqliteWithMutexPtr& db, const std::string& tableName);

    static std::string indexName(const std::string& tag, const std::vector<std::string>& colNames);
    static bool createIndexIfNotExists(const SqliteWithMutexPtr& db, const std::string& tableName, const std::string& indexName,
                                       const std::vector<std::string>& colNames);

    static bool createViewIfNotExist(const SqliteWithMutexPtr& db, const std::string& viewName, const std::string& tableName,
                                     const ConditionWrapper& condition = {}, const std::string& indexed = {});
    static bool dropViewIfExists(const SqliteWithMutexPtr& db, const std::string& tableName);

    template <typename Entity>
    static std::vector<Entity> queryEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const ConditionWrapper& condition = {});
    template <typename Entity>
    static std::vector<Entity> queryEntities(const SqliteWithMutexPtr& db, const std::string& tableName, int index, int nums);
    template <typename Entity>
    static std::vector<Entity> queryEntities(const SqliteWithMutexPtr& db, const std::string& tableName, int index, int nums,
                                             const ConditionWrapper& condition);
    template <typename Entity>
    static std::vector<Entity> queryEntities(const SqliteWithMutexPtr& db, const std::string& querySql);

    template <typename Entity>
    static int64_t insertEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const std::vector<Entity>& entities);
    template <typename Entity>
    static int64_t insertEntitiesWithOutTrans(const SqliteWithMutexPtr& db, const std::string& tableName, const std::vector<Entity>& entities);

    static bool deleteEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const ConditionWrapper& condition = {});

    static int64_t countEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const ConditionWrapper& condition = {});

    static std::vector<std::string> queryDistinctCol(const SqliteWithMutexPtr& db, const std::string& tableName, const ColumnInfo& metaInfo,
                                                     const ConditionWrapper& condition = {});

private:
    template <typename T>
    static void insertEntitiesCore(SQLiteStatement& stmt, const std::vector<T>& entities);
};

template <typename Entity>
inline std::vector<Entity> SqliteUtil::queryEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const ConditionWrapper& condition)
{
    if (!db || tableName.empty())
    {
        return {};
    }

    std::stringstream ss;
    ss << "SELECT * FROM " << tableName;
    ss << " " << condition.prepareConditionString();

    std::vector<Entity> entities;
    std::string querySql = ss.str();

    SQLiteStatement stmt(*db, querySql);
    condition.bind(stmt);
    while (stmt.executeStep())
    {
        Entity entity;
        entity.setValue(stmt);
        entities.emplace_back(entity);
    }
    return entities;
}

template <typename Entity>
inline std::vector<Entity> SqliteUtil::queryEntities(const SqliteWithMutexPtr& db, const std::string& tableName, int index, int nums)
{
    if (!db || tableName.empty())
    {
        return {};
    }

    std::stringstream ss;
    ss << "SELECT * FROM " << tableName;
    ss << " LIMIT " << std::to_string(index) + "," + std::to_string(nums);

    std::vector<Entity> entities;
    std::string querySql = ss.str();

    SQLiteStatement stmt(*db, querySql);
    while (stmt.executeStep())
    {
        Entity entity;
        entity.setValue(stmt);
        entities.emplace_back(entity);
    }
    return entities;
}

template <typename Entity>
inline std::vector<Entity> SqliteUtil::queryEntities(const SqliteWithMutexPtr& db, const std::string& tableName, int index, int nums,
                                                     const ConditionWrapper& condition)
{
    if (!db || tableName.empty())
    {
        return {};
    }

    std::stringstream ss;
    ss << "SELECT * FROM " + tableName;
    ss << " " << condition.prepareConditionString();
    ss << " LIMIT " << index << "," << nums;

    std::vector<Entity> entities;
    std::string querySql = ss.str();

    SQLiteStatement stmt(*db, querySql);
    condition.bind(stmt);
    while (stmt.executeStep())
    {
        Entity entity;
        entity.setValue(stmt);
        entities.emplace_back(entity);
    }
    return entities;
}

template <typename Entity>
inline std::vector<Entity> SqliteUtil::queryEntities(const SqliteWithMutexPtr& db, const std::string& querySql)
{
    if (!db || querySql.empty())
    {
        return {};
    }

    std::vector<Entity> entities;
    SQLiteStatement stmt(*db, querySql);
    while (stmt.executeStep())
    {
        Entity entity;
        entity.setValue(stmt);
        entities.emplace_back(entity);
    }

    return entities;
}

template <typename Entity>
inline int64_t SqliteUtil::insertEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const std::vector<Entity>& entities)
{
    if (entities.empty() || !db || tableName.empty())
    {
        return 0;
    }

    std::string insertSql = "INSERT INTO " + tableName + " ";
    insertSql += TableStructInfo<Entity>::self().insertPrepareSql();

    SQLiteStatement stmt(*db, insertSql);
    db->transaction();
    try
    {
        insertEntitiesCore<Entity>(stmt, entities);
    }
    catch (const std::exception& e)
    {
        db->rollback();
        return 0;
    }
    db->commit();
    return entities.size();
}

template <typename Entity>
inline int64_t SqliteUtil::insertEntitiesWithOutTrans(const SqliteWithMutexPtr& db, const std::string& tableName, const std::vector<Entity>& entities)
{
    if (entities.empty() || !db || tableName.empty())
    {
        return 0;
    }

    std::string insertSql = "INSERT INTO " + tableName + " ";
    insertSql += TableStructInfo<Entity>::self().insertPrepareSql();

    SQLiteStatement stmt(*db, insertSql);
    insertEntitiesCore<Entity>(stmt, entities);

    return entities.size();
}

template <typename T>
inline void SqliteUtil::insertEntitiesCore(SQLiteStatement& stmt, const std::vector<T>& entities)
{
    for (const auto& entity : entities)
    {
        entity.bind(stmt);
        stmt.executeStep();
        stmt.reset();
    }
}

}  // namespace sqlite
