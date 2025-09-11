#pragma once

#include "Sqlite/Storage/BaseStorage.h"
#include "Sqlite/Storage/StorageFactory.h"

struct DownloadingItem
{
    std::string uniqueId;
    int pluginId;
    std::string filePath;
    std::string coverPath;
    std::string id;
    std::string idType;
    std::string title;
    std::string author;
    std::string url;
    std::string option1;
    std::string option2;
    std::string option3;
    std::string fileExtension;
    double progress;
    int duration;
    int status;
    int fileType;

    // move to sqlite
    int bind(sqlite::SQLiteStatement& stmt) const;
    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0);
};

// clang-format off
TABLESTRUCTINFO_BEGIN(DownloadingItem)
    TABLESTRUCTINFO_COLUMN(uniqueId, uniqueId, false, true)
    TABLESTRUCTINFO_COLUMN(pluginId)
    TABLESTRUCTINFO_COLUMN(filePath)
    TABLESTRUCTINFO_COLUMN(coverPath)
    TABLESTRUCTINFO_COLUMN(id)
    TABLESTRUCTINFO_COLUMN(title)
    TABLESTRUCTINFO_COLUMN(author)
    TABLESTRUCTINFO_COLUMN(url)
    TABLESTRUCTINFO_COLUMN(option1)
    TABLESTRUCTINFO_COLUMN(option2)
    TABLESTRUCTINFO_COLUMN(option3)
    TABLESTRUCTINFO_COLUMN(progress)
    TABLESTRUCTINFO_COLUMN(duration)
    TABLESTRUCTINFO_COLUMN(status)
    TABLESTRUCTINFO_COLUMN(fileType)
    TABLESTRUCTINFO_COLUMN(fileExtension)
TABLESTRUCTINFO_END(DownloadingItem)
// clang-format on

class DownloadingItemStorage : public sqlite::BaseStorage
{
public:
    using Entity = DownloadingItem;
    using BaseStorage::BaseStorage;

    void updateStatus(int status, const sqlite::ConditionWrapper& condition);
    bool isDownload(const std::string& guid) const;

    std::vector<Entity> lastItems();

private:
    static constexpr int maxQueryNum = 4000;
};
