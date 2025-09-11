#pragma once

#include "Sqlite/Storage/BaseStorage.h"
#include "Sqlite/Storage/StorageFactory.h"

struct CookiesInfo
{
    int pluginId{};
    std::string domain;
    int64_t updateTimestamp{};
    std::string cookie;
    std::string expires;
    std::string storedata;

    // move to sqlite
    int bind(sqlite::SQLiteStatement& stmt) const;
    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0);
};

// clang-format off
TABLESTRUCTINFO_BEGIN(CookiesInfo)
    TABLESTRUCTINFO_COLUMN(pluginId, pluginId, false, false, true)
    TABLESTRUCTINFO_COLUMN(domain, domain, false, false, true)
    TABLESTRUCTINFO_COLUMN(updateTimestamp)
    TABLESTRUCTINFO_COLUMN(cookie)
    TABLESTRUCTINFO_COLUMN(expires)
    TABLESTRUCTINFO_COLUMN(storedata)
TABLESTRUCTINFO_END(CookiesInfo)
// clang-format on

class CookiesInfoStorage : public sqlite::BaseStorage
{
public:
    using Entity = CookiesInfo;
    using BaseStorage::BaseStorage;

    Entity getCookiesInfo(int pluginId);

    bool insertOrUpdate(const Entity& entity);

    bool havePlugin(int pluginId, const std::string& domain);

    std::vector<Entity> allItems();
};
