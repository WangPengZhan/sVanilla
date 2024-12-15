#pragma once

#include "Sqlite/Storage/BaseStorage.h"
#include "Sqlite/Storage/StorageFactory.h"

struct CookiesInfo
{
    int pluginType{};
    int64_t timestamp{};
    int64_t cookieTimestamp{};
    std::string cookie;
    std::string storedata;

    // move to sqlite
    int bind(sqlite::SQLiteStatement& stmt) const;
    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0);
};

// clang-format off
TABLESTRUCTINFO_BEGIN(CookiesInfo)
    TABLESTRUCTINFO_COMLUNM(pluginType, pluginType, false, true, true)
    TABLESTRUCTINFO_COMLUNM(timestamp)
    TABLESTRUCTINFO_COMLUNM(cookieTimestamp)
    TABLESTRUCTINFO_COMLUNM(cookie)
    TABLESTRUCTINFO_COMLUNM(storedata)
TABLESTRUCTINFO_END(CookiesInfo)
// clang-format on

class CookiesInfoStorage : public sqlite::BaseStorage
{
public:
    using Entity = CookiesInfo;
    using BaseStorage::BaseStorage;

    std::vector<Entity> allItems();
};
