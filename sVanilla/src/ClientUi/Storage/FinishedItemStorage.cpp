#include <filesystem>

#include "ClientUi/Storage/FinishedItemStorage.h"

int FinishedItem::bind(sqlite::SQLiteStatement& stmt) const
{
    int index = 1;
    stmt.bind(index++, uniqueId);
    stmt.bind(index++, pluginType);
    stmt.bind(index++, filePath);
    stmt.bind(index++, coverPath);
    stmt.bind(index++, bvid);
    stmt.bind(index++, title);
    stmt.bind(index++, auther);
    stmt.bind(index++, url);
    stmt.bind(index++, duration);
    stmt.bind(index++, type);
    stmt.bind(index++, fileExist);

    return index;
}

void FinishedItem::setValue(sqlite::SQLiteStatement& stmt, int startIndex)
{
    int index = startIndex;
    uniqueId = stmt.column(index++).getString();
    pluginType = stmt.column(index++);
    filePath = stmt.column(index++).getString();
    coverPath = stmt.column(index++).getString();
    bvid = stmt.column(index++).getString();
    title = stmt.column(index++).getString();
    auther = stmt.column(index++).getString();
    url = stmt.column(index++).getString();
    duration = stmt.column(index++);
    type = stmt.column(index++);
    fileExist = stmt.column(index++).getInt() != 0;
}

bool FinishItemStorage::isDownload(const std::string& guid)
{
    updateFileExist();

    auto uniqueIdCol = sqlite::TableStructInfo<Entity>::self().uniqueId;
    auto fileExist = sqlite::TableStructInfo<Entity>::self().fileExist;
    sqlite::ConditionWrapper condition;
    condition.addCondition(uniqueIdCol, sqlite::Condition::EQUALS, guid);
    condition.addCondition(fileExist, sqlite::Condition::EQUALS, static_cast<int64_t>(true));

    std::stringstream ss;
    ss << "SELECT count(*) FROM " << tableName();
    ss << " " << condition.prepareConditionString();

    std::string sql = ss.str();
    sqlite::SQLiteStatement stmt(*m_readDBPtr, sql);
    condition.bind(stmt);
    sql = stmt.expandedSQL();
    stmt.executeStep();
    return (1 == stmt.column(0).getInt());
}

std::vector<FinishItemStorage::Entity> FinishItemStorage::lastItems()
{
    return queryEntities<Entity>(0, maxQueryNum, {});
}

void FinishItemStorage::updateFileExist()
{
    auto finishItems = queryEntities<Entity>(0, maxQueryNum, {});
    for (const auto& finishItem : finishItems)
    {
        std::filesystem::path path = std::filesystem::u8path(finishItem.filePath);
        bool isExist = std::filesystem::exists(path);
        updateFileExist(isExist, finishItem.uniqueId);
    }
}

void FinishItemStorage::updateFileExist(bool exist, const std::string& guid)
{
    auto& table = sqlite::TableStructInfo<Entity>::self();
    sqlite::ConditionWrapper condition;
    condition.addCondition(table.uniqueId, sqlite::Condition::EQUALS, guid);

    sqlite::SqliteColumnValue value = static_cast<int64_t>(exist);
    auto fileExistName = table.fileExist.colunmName();
    sqlite::SqliteColumn colunmValue(value, -1, fileExistName);
    sqlite::SqliteUtil::updateEntities(m_writeDBPtr, tableName(), {colunmValue}, condition);
}
