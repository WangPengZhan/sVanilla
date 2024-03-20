#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include "AbstractTable.h"
#include "Download/BiliDownloader.h"

namespace SQLite
{

class FinishedItemTable : public AbstractTable
{
public:
    FinishedItemTable(SQLiteDatabase& db, const std::string& nameTable);

    void bind(const download::FinishedItem& data);
    void value(download::FinishedItem& data);

    bool readItems(int index, int nums, const std::string& condition, std::vector<download::FinishedItem>& datas);
    void insertItem(std::vector<download::FinishedItem> data);

    static HeadType m_finishedItemTableHeader;
};

}  // namespace SQLite
