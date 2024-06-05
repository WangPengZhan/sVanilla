#pragma once

#include "Sqlite/Storage/BaseStorage.h"
#include "Sqlite/Storage/StorageFactory.h"

struct SearchHistory
{
    std::string url;
    int pluginType;
    int64_t timestamp;

    // move to sqlite
    int bind(sqlite::SQLiteStatement& stmt) const;
    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0);
};

// clang-format off
TABLESTRUCTINFO_BEGIN(SearchHistory)
    TABLESTRUCTINFO_COMLUNM(url, url, false, true, true)
    TABLESTRUCTINFO_COMLUNM(pluginType)
    TABLESTRUCTINFO_COMLUNM(timestamp)
TABLESTRUCTINFO_END(SearchHistory)
// clang-format on

class SearchHistoryStorage : public sqlite::BaseStorage
{
public:
    using Entity = SearchHistory;
    using BaseStorage::BaseStorage;

    bool insertOrUpdate(const std::string& url, int pluginType);
    std::vector<std::string> allItems();

    static constexpr int maxNum = 10;

private:
    bool haveUrl(const std::string& url);
};
