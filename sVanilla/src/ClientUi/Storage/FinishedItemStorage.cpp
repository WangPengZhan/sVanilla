#include "FinishedItemStorage.h"

void FinishedItem::bind(sqlite::SQLiteStatement& stmt) const
{
    int index = 1;
    stmt.bind(index++, uniqueId);
    stmt.bind(index++, filePath);
    stmt.bind(index++, bvid);
    stmt.bind(index++, title);
    stmt.bind(index++, duration);
}

void FinishedItem::setValue(sqlite::SQLiteStatement& stmt, int startIndex)
{
    int index = startIndex;
    uniqueId = stmt.column(index++).getString();
    filePath = stmt.column(index++).getString();
    bvid = stmt.column(index++).getString();
    title = stmt.column(index++).getString();
    duration = stmt.column(index++);
}