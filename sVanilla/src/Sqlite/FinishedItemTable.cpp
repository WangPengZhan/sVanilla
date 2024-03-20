#include <sqlite3.h>

#include "AbstractTable.h"
#include "FinishedItemTable.h"

namespace SQLite
{
FinishedItemTable::HeadType FinishedItemTable::m_finishedItemTableHeader{
    {"UniqueId",     Text   },
    {"FilePath",    Text   },
    {"Bvid",         Text   },
    {"Title",Text},
    {"Duration", Integer},
};

FinishedItemTable::FinishedItemTable(SQLiteDatabase& db, const std::string& nameTable)
    : AbstractTable(db, nameTable)
{
    setHeaders(m_finishedItemTableHeader);
    setPrimary({"UniqueId"});
}

void FinishedItemTable::bind(const download::FinishedItem& data)
{
    int col = 1;
    m_db.bind(col++, sqliteType(Text), data.uniqueId);
    m_db.bind(col++, sqliteType(Text), data.filePath);
    m_db.bind(col++, sqliteType(Text), data.bvid);
    m_db.bind(col++, sqliteType(Text), data.title);
    m_db.bind(col++, sqliteType(Integer), sqlite_int64(data.duration));
}

void FinishedItemTable::value(download::FinishedItem& data)
{
    int col = 0;
    data.uniqueId = std::any_cast<std::string>(m_db.value(col++));
    data.filePath = std::any_cast<std::string>(m_db.value(col++));
    data.bvid = std::any_cast<std::string>(m_db.value(col++));
    data.title = std::any_cast<std::string>(m_db.value(col++));
    data.duration = std::any_cast<sqlite_int64>(m_db.value(col++));
}

bool FinishedItemTable::readItems(int index, int nums, const std::string& condition, std::vector<download::FinishedItem>& datas)
{
    m_db.prepare(createTableSelectAllSql() + condition + "LIMIT " + std::to_string(index) + "," + std::to_string(nums));

    while (m_db.next() != SQLiteDatabase::Done)
    {
        download::FinishedItem data;
        value(data);
        datas.emplace_back(data);
    }

    return true;
}

void FinishedItemTable::insertItem(std::vector<download::FinishedItem> datas)
{
    m_db.transaction();
    m_db.prepare(createTableInsertSql());

    for (auto& data : datas)
    {
        bind(data);
        m_db.next();
        m_db.reset();
    }
    m_db.commit();
}

}  // namespace SQLite
