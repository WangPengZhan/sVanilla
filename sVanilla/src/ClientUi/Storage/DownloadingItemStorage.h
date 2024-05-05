#pragma once

#include "Sqlite/Storage/BaseStorage.h"
#include "Sqlite/Storage/StorageFactory.h"

struct DownloadingItem
{
    std::string uniqueId;
    int pluginType;
    std::string filePath;
    std::string coverPath;
    std::string bvid;
    std::string title;
    std::string auther;
    std::string url;
    double progress;
    int duration;
    int status;
    int type;

    // move to sqlite
    int bind(sqlite::SQLiteStatement& stmt) const;
    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0);
};

// clang-format off
TABLESTRUCTINFO_BEGIN(DownloadingItem)
    TABLESTRUCTINFO_COMLUNM(uniqueId, uniqueId, false, true)
    TABLESTRUCTINFO_COMLUNM(pluginType)
    TABLESTRUCTINFO_COMLUNM(filePath)
    TABLESTRUCTINFO_COMLUNM(coverPath)
    TABLESTRUCTINFO_COMLUNM(bvid)
    TABLESTRUCTINFO_COMLUNM(title)
    TABLESTRUCTINFO_COMLUNM(auther)
    TABLESTRUCTINFO_COMLUNM(url)
    TABLESTRUCTINFO_COMLUNM(progress)
    TABLESTRUCTINFO_COMLUNM(duration)
    TABLESTRUCTINFO_COMLUNM(status)
    TABLESTRUCTINFO_COMLUNM(type)
TABLESTRUCTINFO_END(DownloadingItem)
// clang-format on

class DownloadingItemStorage : public sqlite::BaseStorage
{
public:
    using Entity = DownloadingItem;
    using BaseStorage::BaseStorage;

    void updateStatus(int status, const sqlite::ConditionWrapper& condition);
    bool isDownload(const std::string& guid) const;

private:
};
