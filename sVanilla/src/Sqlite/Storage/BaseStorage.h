#pragma once
#include <vector>
#include <string>

#include "Sqlite/Database/SQLiteDatabase.h"
#include "Sqlite/SqlComposer/ConditionWrapper.h"
#include "Sqlite/SqlComposer/SqlUtil.h"

namespace sqlite
{

class BaseStorage
{
public:
    BaseStorage() = default;
    BaseStorage(SqliteWithMutexPtr readDBPtr);
    BaseStorage(SqliteWithMutexPtr readDBPtr, std::string tableName);
    BaseStorage(SqliteWithMutexPtr readDBPtr, std::string tableName, SqliteWithMutexPtr writeDBPtr);
    virtual ~BaseStorage() = default;

    const std::string& tableName() const;
    void setTableName(const std::string& tableName);

    template <typename Entity>
    std::vector<Entity> queryEntities(int index, int nums, const ConditionWrapper& condition);

    template <typename Entity>
    int64_t insertEntities(const std::vector<Entity>& entities);

    template <typename Entity>
    int64_t updateEntities(const std::vector<Entity>& entities);

    bool deleteEntities(const ConditionWrapper& condition = {});

    int64_t countEntities(const ConditionWrapper& condition = {});
    std::vector<std::string> queryDistinctCol(const ColumnInfo& metaInfo, const ConditionWrapper& condition = {});

protected:
    SqliteWithMutexPtr m_readDBPtr;
    SqliteWithMutexPtr m_writeDBPtr;

    std::string m_tableName;
};

template <typename Entity>
inline std::vector<Entity> BaseStorage::queryEntities(int index, int nums, const ConditionWrapper& condition)
{
    return SqliteUtil::queryEntities<Entity>(m_readDBPtr, tableName(), index, nums, condition);
}

template <typename Entity>
inline int64_t BaseStorage::insertEntities(const std::vector<Entity>& entities)
{
    return SqliteUtil::insertEntities(m_writeDBPtr, tableName(), entities);
}

template <typename Entity>
inline int64_t BaseStorage::updateEntities(const std::vector<Entity>& entities)
{
    return SqliteUtil::updateEntities(m_writeDBPtr, tableName(), entities);
}

}  // namespace sqlite