#include "ClientUi/Storage/DownloadingItemStorage.h"
#include "DownloadingItemStorage.h"

int DownloadingItem::bind(sqlite::SQLiteStatement& stmt) const
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
    stmt.bind(index++, progress);
    stmt.bind(index++, duration);
    stmt.bind(index++, status);
    stmt.bind(index++, type);

    return index;
}

void DownloadingItem::setValue(sqlite::SQLiteStatement& stmt, int startIndex)
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
    progress = stmt.column(index++);
    duration = stmt.column(index++);
    status = stmt.column(index++);
    type = stmt.column(index++);
}

void DownloadingItemStorage::updateStatus(int status, const sqlite::ConditionWrapper& condition)
{
#if 0
    auto statusName = sqlite::TableStructInfo<Entity>::self().status.colunmName();
    std::stringstream ss;
    ss << "UPDATE " << tableName();
    ss << " SET " << statusName << " = "
       << "?";
    ss << condition.prepareConditionString();

    std::string sql = ss.str();
    sqlite::SQLiteStatement stmt(*(m_writeDBPtr.get()), sql);
    int start = 1;
    stmt.bind(start++, status);
    condition.bind(stmt, start);

    sql = stmt.expandedSQL();
    stmt.executeStep();
#else
    sqlite::SqliteColumnValue value = static_cast<int64_t>(status);
    auto statusName = sqlite::TableStructInfo<Entity>::self().status.colunmName();
    sqlite::SqliteColumn colunmValue(value, -1, statusName);
    sqlite::SqliteUtil::updateEntities(m_writeDBPtr, tableName(), {colunmValue}, condition);
#endif
}

bool DownloadingItemStorage::isDownload(const std::string& guid) const
{
    auto uniqueIdCol = sqlite::TableStructInfo<Entity>::self().uniqueId;
    sqlite::ConditionWrapper condition;
    condition.addCondition(uniqueIdCol, sqlite::Condition::EQUALS, guid);

    std::stringstream ss;
    ss << "SELECT count(*) FROM " << tableName();
    ss << " " << condition.prepareConditionString();

    std::string sql = ss.str();
    sqlite::SQLiteStatement stmt(*m_readDBPtr, sql);
    condition.bind(stmt);
    stmt.executeStep();
    return (1 == stmt.column(0).getInt());
}