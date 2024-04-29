#pragma once

#include "Sqlite/Storage/BaseStorage.h"
#include "Sqlite/Storage/StorageFactory.h"

struct FinishedItem
{
    std::string uniqueId;
    int type;
    std::string filePath;
    std::string coverPath;
    std::string bvid;
    std::string title;
    int duration;

    // move to sqlite
    void bind(sqlite::SQLiteStatement& stmt) const;
    void setValue(sqlite::SQLiteStatement& stmt, int startIndex = 0);
};

// clang-format off
TABLESTRUCTINFO_BEGIN(FinishedItem)
    TABLESTRUCTINFO_COMLUNM(uniqueId, uniqueId, false, true)
    TABLESTRUCTINFO_COMLUNM(filePath)
    TABLESTRUCTINFO_COMLUNM(bvid)
    TABLESTRUCTINFO_COMLUNM(title)
    TABLESTRUCTINFO_COMLUNM(duration)
TABLESTRUCTINFO_END(FinishedItem)
// clang-format on

class FinishItemStorage : public sqlite::BaseStorage
{
public:
    using Entity = FinishedItem;
    using BaseStorage::BaseStorage;

private:
};
