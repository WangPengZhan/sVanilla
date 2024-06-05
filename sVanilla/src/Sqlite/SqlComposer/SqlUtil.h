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

std::string prepareSql(const std::vector<SqliteColumn>& newValues);

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
    static std::vector<Entity> queryEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const ConditionWrapper& condition);
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

    static int64_t updateEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const std::vector<SqliteColumn>& newValues,
                                  const ConditionWrapper& condition);
    template <typename Entity>
    static int64_t updateEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const std::vector<Entity>& entities);
    template <typename Entity>
    static int64_t updateEntitiesWithOutTrans(const SqliteWithMutexPtr& db, const std::string& tableName, const std::vector<Entity>& entities);

    static bool deleteEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const ConditionWrapper& condition = {});

    static int64_t countEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const ConditionWrapper& condition = {});

    static std::vector<std::string> queryDistinctCol(const SqliteWithMutexPtr& db, const std::string& tableName, const ColumnInfo& metaInfo,
                                                     const ConditionWrapper& condition = {});

private:
    template <typename Entity>
    static void insertEntitiesCore(SQLiteStatement& stmt, const std::vector<Entity>& entities);
    template <typename Entity>
    static void updateEntitiesCore(SQLiteStatement& stmt, const std::vector<Entity>& entities);
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
    querySql = stmt.expandedSQL();
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
    querySql = stmt.expandedSQL();
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

template <typename Entity>
inline int64_t SqliteUtil::updateEntities(const SqliteWithMutexPtr& db, const std::string& tableName, const std::vector<Entity>& entities)
{
    if (entities.empty() || !db || tableName.empty())
    {
        return 0;
    }

    std::string sql = "UPDATE " + tableName + " SET ";
    sql += TableStructInfo<Entity>::self().updatePrepareSql();

    ConditionWrapper conditionWrapper;
    const auto& uniqueCols = TableStructInfo<Entity>::self().primaryColumnInfos();
    for (const auto& uniqueCol : uniqueCols)
    {
        conditionWrapper.addCondition(*uniqueCol, Condition::EQUALS);
    }
    sql += conditionWrapper.prepareConditionString();

    SQLiteStatement stmt(*db, sql);

    db->transaction();
    try
    {
        updateEntitiesCore<Entity>(stmt, entities);
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
inline int64_t SqliteUtil::updateEntitiesWithOutTrans(const SqliteWithMutexPtr& db, const std::string& tableName, const std::vector<Entity>& entities)
{
    if (entities.empty() || !db || tableName.empty())
    {
        return 0;
    }

    std::string sql = "UPDATE " + tableName + " SET ";
    sql += TableStructInfo<Entity>::self().updatePrepareSql();

    ConditionWrapper conditionWrapper;
    const auto& uniqueCols = TableStructInfo<Entity>::self().primaryColumnInfos();
    for (const auto& uniqueCol : uniqueCols)
    {
        conditionWrapper.addCondition(*uniqueCol, Condition::EQUALS);
    }
    sql += conditionWrapper.prepareConditionString();

    SQLiteStatement stmt(*db, sql);
    insertEntitiesCore<Entity>(stmt, entities);

    return entities.size();
}

template <typename Entity>
inline void SqliteUtil::insertEntitiesCore(SQLiteStatement& stmt, const std::vector<Entity>& entities)
{
    for (const auto& entity : entities)
    {
        entity.bind(stmt);
        stmt.executeStep();
        stmt.reset();
    }
}

template <typename Entity>
inline void SqliteUtil::updateEntitiesCore(SQLiteStatement& stmt, const std::vector<Entity>& entities)
{
    const auto& uniqueCols = TableStructInfo<Entity>::self().primaryColumnInfos();
    for (const auto& entity : entities)
    {
        int index = entity.bind(stmt);
        for (const auto& uniqueCol : uniqueCols)
        {
            indexToType(uniqueCol->valueTypeIndex(), [&](auto&& elem) {
                using ValueType = std::remove_cvref_t<decltype(elem)>;
                auto memberPtr = (*uniqueCol).template memberPtr<Entity, ValueType>();
                auto value = entity.*(*memberPtr);
                if constexpr (std::is_same_v<ValueType, bool> || std::is_same_v<ValueType, uint8_t> || std::is_same_v<ValueType, int8_t> ||
                              std::is_same_v<ValueType, uint16_t> || std::is_same_v<ValueType, int16_t> || std::is_same_v<ValueType, uint64_t>)
                {
                    stmt.bind(index++, int32_t(value));
                }
                else if constexpr (std::is_same_v<ValueType, float> || std::is_same_v<ValueType, long double>)
                {
                    stmt.bind(index++, static_cast<double>(value));
                }
                else
                {
                    stmt.bind(index++, value);
                }
            });
        }

        stmt.executeStep();
        stmt.reset();
    }
}

}  // namespace sqlite
