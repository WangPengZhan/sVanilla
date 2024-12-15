#include "Storage/CookiesStorage.h"

int CookiesInfo::bind(sqlite::SQLiteStatement& stmt) const
{
    int index = 1;
    stmt.bind(index++, pluginType);
    stmt.bind(index++, timestamp);
    stmt.bind(index++, cookieTimestamp);
    stmt.bind(index++, cookie);
    stmt.bind(index++, storedata);

    return index;
}

void CookiesInfo::setValue(sqlite::SQLiteStatement& stmt, int startIndex)
{
    int index = startIndex;
    pluginType = stmt.column(index++);
    timestamp = stmt.column(index++);
    cookieTimestamp = stmt.column(index++);
    cookie = stmt.column(index++).getString();
    storedata = stmt.column(index++).getString();
}

std::vector<CookiesInfoStorage::Entity> CookiesInfoStorage::allItems()
{
    std::vector<Entity> vectHistory;
    std::string sql = "SELECT * FROM " + tableName() + " ORDER BY timestamp DESC ";
    vectHistory = sqlite::SqliteUtil::queryEntities<Entity>(m_readDBPtr, sql);

    return vectHistory;
}
