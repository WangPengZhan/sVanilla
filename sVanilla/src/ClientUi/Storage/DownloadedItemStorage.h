#pragma once

#include "Sqlite/Storage/BaseStorage.h"
#include "Sqlite/Storage/StorageFactory.h"

struct DownloadedItem
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
    int duration;
    int fileType;
    bool fileExist;
    std::string fileExtension;

    // move to sqlite
    int bind(sqlite::SQLiteStatement& stmt) const;
    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0);
};

// clang-format off
TABLESTRUCTINFO_BEGIN(DownloadedItem)
    TABLESTRUCTINFO_COLUMN(uniqueId, uniqueId, false, true)
    TABLESTRUCTINFO_COLUMN(pluginId)
    TABLESTRUCTINFO_COLUMN(filePath)
    TABLESTRUCTINFO_COLUMN(coverPath)
    TABLESTRUCTINFO_COLUMN(id)
    TABLESTRUCTINFO_COLUMN(idType)
    TABLESTRUCTINFO_COLUMN(title)
    TABLESTRUCTINFO_COLUMN(author)
    TABLESTRUCTINFO_COLUMN(url)
    TABLESTRUCTINFO_COLUMN(option1)
    TABLESTRUCTINFO_COLUMN(option2)
    TABLESTRUCTINFO_COLUMN(option3)
    TABLESTRUCTINFO_COLUMN(duration)
    TABLESTRUCTINFO_COLUMN(fileType)
    TABLESTRUCTINFO_COLUMN(fileExist)
    TABLESTRUCTINFO_COLUMN(fileExtension)
TABLESTRUCTINFO_END(FinishedItem)
// clang-format on

class DownloadedItemStorage : public sqlite::BaseStorage
{
public:
    using Entity = DownloadedItem;
    using BaseStorage::BaseStorage;

    bool isDownload(const std::string& guid);

    std::vector<Entity> lastItems();

    void updateFileExist();
    void updateFileExist(bool exist, const std::string& guid);

private:
    static constexpr int maxQueryNum = 4000;
};
