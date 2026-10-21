#include <chrono>

#include "Storage/SearchHistoryStorage.h"

int SearchHistory::bind(sqlite::SQLiteStatement& stmt) const
{
    int index = 1;
    stmt.bind(index++, url);
    stmt.bind(index++, pluginId);
    stmt.bind(index++, timestamp);
    stmt.bind(index++, searchTimes);

    return index;
}

void SearchHistory::setValue(sqlite::SQLiteStatement& stmt, int startIndex)
{
    int index = startIndex;
    url = stmt.column(index++).getString();
    pluginId = stmt.column(index++);
    timestamp = stmt.column(index++);
    searchTimes = stmt.column(index++);
}

bool SearchHistoryStorage::insertOrUpdate(const std::string& url, int pluginId)
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    SearchHistory history;
    history.url = url;
    history.pluginId = pluginId;
    history.timestamp = ms;
    history.searchTimes = 1;

    if (haveUrl(url))
    {
        auto& table = sqlite::TableStructInfo<SearchHistory>::self();
        sqlite::ConditionWrapper condition;
        condition.addCondition(table.url, sqlite::Condition::EQUALS, url);
        const sqlite::SqliteColumn pluginIdValue(static_cast<int64_t>(pluginId), -1, table.pluginId.columnName());
        const sqlite::SqliteColumn timestampValue(static_cast<int64_t>(ms), -1, table.timestamp.columnName());
        return sqlite::SqliteUtil::updateEntities(m_writeDBPtr, tableName(), {pluginIdValue, timestampValue}, condition) > 0;
    }
    else
    {
        return sqlite::SqliteUtil::insertEntities<SearchHistory>(m_writeDBPtr, tableName(), {history}) > 0;
    }
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
