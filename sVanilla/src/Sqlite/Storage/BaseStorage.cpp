#include "BaseStorage.h"

namespace sqlite
{
BaseStorage::BaseStorage(SqliteWithMutexPtr readDBPtr)
    : m_readDBPtr(readDBPtr)
{
}

BaseStorage::BaseStorage(SqliteWithMutexPtr readDBPtr, std::string tableName)
    : m_readDBPtr(readDBPtr)
    , m_tableName(std::move(tableName))
{
}

BaseStorage::BaseStorage(SqliteWithMutexPtr readDBPtr, std::string tableName, SqliteWithMutexPtr writeDBPtr)
    : m_readDBPtr(readDBPtr)
    , m_writeDBPtr(writeDBPtr)
    , m_tableName(std::move(tableName))
{
}

const std::string& BaseStorage::tableName() const
{
    return m_tableName;
}

bool BaseStorage::deleteEntities(const ConditionWrapper& condition)
{
    return SqliteUtil::deleteEntities(m_writeDBPtr, tableName(), condition);
}

int64_t BaseStorage::countEntities(const ConditionWrapper& condition)
{
    return SqliteUtil::countEntities(m_readDBPtr, tableName(), condition);
}

std::vector<std::string> BaseStorage::queryDistinctCol(const ColumnInfo& metaInfo, const ConditionWrapper& condition)
{
    return SqliteUtil::queryDistinctCol(m_readDBPtr, tableName(), metaInfo, condition);
}

}  // namespace  sqlite
