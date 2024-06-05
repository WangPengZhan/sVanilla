#include "ClientUi/Storage/SearchHistoryStorage.h"

int SearchHistory::bind(sqlite::SQLiteStatement& stmt) const
{
    int index = 1;
    stmt.bind(index++, url);
    stmt.bind(index++, pluginType);
    stmt.bind(index++, timestamp);

    return index;
}

void SearchHistory::setValue(sqlite::SQLiteStatement& stmt, int startIndex)
{
    int index = startIndex;
    url = stmt.column(index++).getString();
    pluginType = stmt.column(index++);
    timestamp = stmt.column(index++);
}

bool SearchHistoryStorage::insertOrUpdate(const std::string& url, int pluginType)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    SearchHistory history;
    history.url = url;
    history.pluginType = pluginType;
    history.timestamp = now_time_t;

    if (haveUrl(url))
    {
        sqlite::SqliteUtil::updateEntities<SearchHistory>(m_writeDBPtr, tableName(), {history});
    }
    else
    {
        sqlite::SqliteUtil::insertEntities<SearchHistory>(m_writeDBPtr, tableName(), {history});
    }

    return true;
}

std::vector<std::string> SearchHistoryStorage::allItems()
{
    std::vector<SearchHistory> vectHistory;
    std::string sql = "SELECT * FROM " + tableName() + " ORDER BY timestamp DESC ";
    vectHistory = sqlite::SqliteUtil::queryEntities<SearchHistory>(m_readDBPtr, sql);

    std::vector<std::string> result;
    for (const auto& history : vectHistory)
    {
        result.push_back(history.url);
    }

    return result;
}

bool SearchHistoryStorage::haveUrl(const std::string& url)
{
    auto& tableStruct = sqlite::TableStructInfo<SearchHistory>::self();
    sqlite::ConditionWrapper condition;
    condition.addCondition(tableStruct.url, sqlite::Condition::EQUALS, url);
    int size = sqlite::SqliteUtil::queryEntities<SearchHistory>(m_readDBPtr, tableName(), condition).size();
    return size > 0;
}