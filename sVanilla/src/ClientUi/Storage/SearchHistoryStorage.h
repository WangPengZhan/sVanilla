#pragma once

#include "Sqlite/Storage/BaseStorage.h"
#include "Sqlite/Storage/StorageFactory.h"

struct SearchHistory
{
    std::string url;
    int pluginId;
    int64_t timestamp;
    int searchTimes{};

    // move to sqlite
    int bind(sqlite::SQLiteStatement& stmt) const;
    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0);
};

// clang-format off
TABLESTRUCTINFO_BEGIN(SearchHistory)
    TABLESTRUCTINFO_COLUMN(url, url, false, true, true)
    TABLESTRUCTINFO_COLUMN(pluginId)
    TABLESTRUCTINFO_COLUMN(timestamp)
    TABLESTRUCTINFO_COLUMN(searchTimes)
TABLESTRUCTINFO_END(SearchHistory)
// clang-format on

class SearchHistoryStorage : public sqlite::BaseStorage
{
public:
    using Entity = SearchHistory;
    using BaseStorage::BaseStorage;

    bool insertOrUpdate(const std::string& url, int pluginId);
    std::vector<std::string> allItems();

    static constexpr int maxNum = 20;

private:
    bool haveUrl(const std::string& url);
};
