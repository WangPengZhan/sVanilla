#pragma once

#include "Sqlite/Storage/BaseStorage.h"
#include "Sqlite/Storage/StorageFactory.h"

struct CookiesInfo
{
    int pluginType{};
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
    TABLESTRUCTINFO_COMLUNM(pluginType, pluginType, false, false, true)
    TABLESTRUCTINFO_COMLUNM(domain, domain, false, false, true)
    TABLESTRUCTINFO_COMLUNM(updateTimestamp)
    TABLESTRUCTINFO_COMLUNM(cookie)
    TABLESTRUCTINFO_COMLUNM(expires)
    TABLESTRUCTINFO_COMLUNM(storedata)
TABLESTRUCTINFO_END(CookiesInfo)
// clang-format on

class CookiesInfoStorage : public sqlite::BaseStorage
{
public:
    using Entity = CookiesInfo;
    using BaseStorage::BaseStorage;

    Entity getCookiesInfo(int pluginId);

    bool insertOrUpdate(const Entity& entity);

    bool havePlugin(int pluginType, const std::string& domain);

    std::vector<Entity> allItems();
};
