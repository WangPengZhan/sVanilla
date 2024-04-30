#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <any>

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

#include <sqlite3.h>

#include "Sqlite/SQLiteDatabase.h"
#include "Sqlite/SQLiteUtility.h"
#include "Sqlite/AbstractTable.h"

namespace sqlite
{

struct TestStruct
{
    int nTestInt;
    std::string strTest;
    double dTest;
    bool bTest;
    std::vector<uint8_t> bBlob;

    bool operator==(const TestStruct& other) const
    {
        return nTestInt == other.nTestInt && strTest == other.strTest && dTest == other.dTest && bTest == other.bTest && bBlob == other.bBlob;
    }
};

class TestTable : public AbstractTable
{
public:
    TestTable(SQLiteDatabase& db, const std::string& nameTable)
        : AbstractTable(db, nameTable)
    {
        setHeaders(m_testHeader);
        setPrimary({"FieldInteger"});
    }

    void bind(TestStruct* test)
    {
        int col = 1;
        m_db.bind(col++, sqliteType(Integer), sqlite_int64(test->nTestInt));
        m_db.bind(col++, sqliteType(Text), test->strTest);
        m_db.bind(col++, sqliteType(Double), test->dTest);
        m_db.bind(col++, sqliteType(Boolean), test->bTest);
        m_db.bind(col++, sqliteType(Blob), std::string((const char*)test->bBlob.data(), test->bBlob.size()));
    }

    void value(TestStruct* test)
    {
        int col = 0;
        test->nTestInt = std::any_cast<sqlite_int64>(m_db.value(col++));
        test->strTest = std::any_cast<std::string>(m_db.value(col++));
        test->dTest = std::any_cast<double>(m_db.value(col++));
        test->bTest = std::any_cast<sqlite_int64>(m_db.value(col++));
        std::string buff = std::any_cast<std::string>(m_db.value(col++));
        test->bBlob.resize(buff.size());
        memcpy(test->bBlob.data(), buff.data(), buff.size());
    }

    void initTable() override
    {
        m_db.execute(createTableSql());
    }

    void insert(const std::vector<TestStruct>& datas)
    {
        m_db.transaction();
        m_db.prepare(createTableInsertSql());

        for (auto& data : datas)
        {
            bind((TestStruct*)(&data));
            m_db.next();
            m_db.reset();
        }
        m_db.commit();
    }

    void read(std::vector<TestStruct>& datas)
    {
        TestStruct data;

        m_db.prepare(createTableSelectAllSql());

        while (m_db.next() != SQLiteDatabase::Done)
        {
            value(&data);
            datas.emplace_back(data);
        }
    }

private:
    static HeadType m_testHeader;
};

TestTable::HeadType TestTable::m_testHeader = {
    {"FieldInteger", Integer},
    {"FieldText",    Text   },
    {"FieldDouble",  Double },
    {"FieldBoolean", Boolean},
    {"FieldBlob",    Blob   },
};

}  // namespace sqlite

TEST(SQLiteDatabaseTest, Open)
{
    spdlog::rotating_logger_mt<spdlog::async_factory>("SQLite", "log/SQLite.log", 1024 * 1024 * 10, 100);

    sqlite::SQLiteDatabase db("test.db");
    EXPECT_TRUE(db.isOpen());

    sqlite::TestTable test(db, "test");
    test.initTable();
    std::vector<sqlite::TestStruct> testDatas;
    for (int i = 0; i < 100; ++i)
    {
        sqlite::TestStruct testData;
        testData.nTestInt = i;
        testData.strTest = std::to_string(i);
        testData.dTest = i / 4.0;
        testData.bTest = (i % 2) == 0;
        for (int j = 0; j < i; ++j)
        {
            testData.bBlob.push_back(j);
        }

        testDatas.push_back(testData);
    }

    test.insert(testDatas);

    std::vector<sqlite::TestStruct> testReadDatas;

    test.read(testReadDatas);

    EXPECT_EQ(testDatas, testReadDatas);
}
