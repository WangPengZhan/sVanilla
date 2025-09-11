#include "Storage/CookiesInfoStorage.h"

int CookiesInfo::bind(sqlite::SQLiteStatement& stmt) const
{
    int index = 1;
    stmt.bind(index++, pluginId);
    stmt.bind(index++, domain);
    stmt.bind(index++, updateTimestamp);
    stmt.bind(index++, cookie);
    stmt.bind(index++, expires);
    stmt.bind(index++, storedata);

    return index;
}

void CookiesInfo::setValue(sqlite::SQLiteStatement& stmt, int startIndex)
{
    int index = startIndex;
    pluginId = stmt.column(index++);
    domain = stmt.column(index++).getString();
    updateTimestamp = stmt.column(index++);
    cookie = stmt.column(index++).getString();
    expires = stmt.column(index++).getString();
    storedata = stmt.column(index++).getString();
}

CookiesInfoStorage::Entity CookiesInfoStorage::getCookiesInfo(int pluginId)
{
    auto& tableStruct = sqlite::TableStructInfo<typename CookiesInfoStorage::Entity>::self();
    sqlite::ConditionWrapper condition;
    condition.addCondition(tableStruct.pluginId, sqlite::Condition::EQUALS, pluginId);
    auto entities = queryEntities<Entity>(0, 10, condition);
    if (entities.empty())
    {
        return {};
    }

    return entities.front();
}

std::vector<CookiesInfoStorage::Entity> CookiesInfoStorage::allItems()
{
    std::vector<Entity> vectHistory;
    std::string sql = "SELECT * FROM " + tableName() + " ORDER BY timestamp DESC ";
    vectHistory = sqlite::SqliteUtil::queryEntities<Entity>(m_readDBPtr, sql);
    return vectHistory;
}

bool CookiesInfoStorage::insertOrUpdate(const Entity& entity)
{
    if (havePlugin(entity.pluginId, entity.domain))
    {
        sqlite::SqliteUtil::updateEntities<Entity>(m_writeDBPtr, tableName(), {entity});
    }
    else
    {
        sqlite::SqliteUtil::insertEntities<Entity>(m_writeDBPtr, tableName(), {entity});
    }

    return true;
}

bool CookiesInfoStorage::havePlugin(int pluginId, const std::string& domain)
{
    auto& tableStruct = sqlite::TableStructInfo<Entity>::self();
    sqlite::ConditionWrapper condition;
    condition.addCondition(tableStruct.pluginId, sqlite::Condition::EQUALS, pluginId);
    condition.addCondition(tableStruct.domain, sqlite::Condition::EQUALS, domain);
    int size = sqlite::SqliteUtil::queryEntities<Entity>(m_readDBPtr, tableName(), condition).size();
    return size > 0;
}
