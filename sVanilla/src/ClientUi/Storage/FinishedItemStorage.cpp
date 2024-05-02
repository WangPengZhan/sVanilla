#include "FinishedItemStorage.h"

void FinishedItem::bind(sqlite::SQLiteStatement& stmt) const
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
}
