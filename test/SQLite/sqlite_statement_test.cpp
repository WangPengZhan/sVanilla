#include "sqlite_fixture.h"

#include <cstdint>
#include <string>
#include <vector>

#include "Sqlite/Database/SQLiteException.h"

namespace
{
// 刻意不设主键/唯一约束：多数用例要重复写入相同的一组值。
void createScalarTable(sqlite::SqliteWithMutexPtr& db)
{
    ASSERT_TRUE(db->execute("CREATE TABLE scalars(i INTEGER, f REAL, t TEXT, b BLOB);"));
}
}  // namespace

// TEST_F 会把夹具名拼进生成类名，因此不能用 "ns::Fixture" 这种限定写法，
// 必须把用例放在夹具所在的命名空间内。
namespace sqlite_test
{

TEST_F(TempDatabaseTest, BindsAndReadsBackEveryScalarType)
{
    createScalarTable(m_db);

    sqlite::SQLiteStatement insert(*m_db, "INSERT INTO scalars VALUES (?, ?, ?, ?);");
    insert.bind(1, int64_t{7});
    insert.bind(2, 1.5);
    insert.bind(3, std::string("text"));
    insert.bind(4, std::vector<uint8_t>{1, 2, 3});
    insert.executeStep();

    sqlite::SQLiteStatement select(*m_db, "SELECT i, f, t, b FROM scalars;");
    ASSERT_TRUE(select.executeStep());

    EXPECT_EQ(select.columnCount(), 4);
    EXPECT_EQ(select.column(0).getInt64(), 7);
    EXPECT_DOUBLE_EQ(select.column(1).getDouble(), 1.5);
    EXPECT_EQ(select.column(2).getString(), "text");
    EXPECT_EQ(select.column(3).getBlob(), (std::vector<uint8_t>{1, 2, 3}));

    // 按列名取列
    EXPECT_EQ(select.column("i").getInt64(), 7);
    EXPECT_EQ(select.column("t").getString(), "text");

    EXPECT_FALSE(select.executeStep());
    EXPECT_TRUE(select.isDone());
}

TEST_F(TempDatabaseTest, BindsInt32AndUnsignedVariants)
{
    createScalarTable(m_db);

    sqlite::SQLiteStatement insert(*m_db, "INSERT INTO scalars VALUES (?, ?, ?, NULL);");
    insert.bind(1, static_cast<int32_t>(-5));
    insert.bind(2, static_cast<uint32_t>(4000000000u));
    insert.bind(3, std::string("s"));
    insert.executeStep();

    sqlite::SQLiteStatement select(*m_db, "SELECT i, f, t FROM scalars;");
    ASSERT_TRUE(select.executeStep());
    EXPECT_EQ(select.column(0).getInt64(), -5);
    EXPECT_EQ(select.column(1).getInt64(), 4000000000LL);
    EXPECT_EQ(select.column(2).getString(), "s");
}

TEST_F(TempDatabaseTest, BindsNoCopyVariants)
{
    createScalarTable(m_db);

    const std::string text = "no-copy";
    const std::vector<uint8_t> blob{4, 5, 6};

    // 注意：bindNoCopy 只有 string / const char* / blob 三种重载，
    // 整数没有对应版本，必须用 bind()。
    sqlite::SQLiteStatement insert(*m_db, "INSERT INTO scalars VALUES (?, ?, ?, ?);");
    insert.bind(1, int64_t{1});
    insert.bindNoCopy(2, text);
    insert.bindNoCopy(3, text.c_str());
    insert.bindNoCopy(4, blob);
    insert.executeStep();

    sqlite::SQLiteStatement select(*m_db, "SELECT f, t, b FROM scalars;");
    ASSERT_TRUE(select.executeStep());
    EXPECT_EQ(select.column(0).getString(), text);
    EXPECT_EQ(select.column(1).getString(), text);
    EXPECT_EQ(select.column(2).getBlob(), blob);
}

// 回归：BLOB 读取曾经使用 memccpy(..., 0, ...)，遇到第一个 0x00 就停止拷贝，
// 二进制内容被静默截断且被当作"成功"。这里用"首字节就是 0x00"的数据钉死该行为。
TEST_F(TempDatabaseTest, BlobWithEmbeddedNulSurvivesRoundTrip)
{
    createScalarTable(m_db);

    const std::vector<uint8_t> raw{0x00, 0x01, 0x00, 0xFF, 0x00, 0x7F, 0x00};
    sqlite::SQLiteStatement insert(*m_db, "INSERT INTO scalars VALUES (1, 1.0, 'x', ?);");
    insert.bind(1, raw);
    insert.executeStep();

    sqlite::SQLiteStatement select(*m_db, "SELECT b FROM scalars WHERE i = 1;");
    ASSERT_TRUE(select.executeStep());

    const auto restored = select.column(0).getBlob();
    EXPECT_EQ(restored.size(), raw.size());
    EXPECT_EQ(restored, raw);
    EXPECT_EQ(select.column(0).getBytes(), static_cast<int>(raw.size()));
}

TEST_F(TempDatabaseTest, NullColumnsAreReportedAsNull)
{
    createScalarTable(m_db);

    sqlite::SQLiteStatement insert(*m_db, "INSERT INTO scalars VALUES (?, ?, ?, NULL);");
    insert.bind(1, int64_t{1});
    insert.bind(2);  // NULL
    insert.bind(3);  // NULL
    insert.executeStep();

    sqlite::SQLiteStatement select(*m_db, "SELECT f, t FROM scalars;");
    ASSERT_TRUE(select.executeStep());

    EXPECT_TRUE(select.isColumnNull(0));
    EXPECT_TRUE(select.isColumnNull("t"));
    EXPECT_TRUE(select.column(0).isNull());
    EXPECT_EQ(select.column(0).getType(), sqlite::FieldType::Null);
    EXPECT_EQ(select.column(0).getString(), "");
    EXPECT_EQ(select.column(0).getInt64(), 0);
}

// 记录当前语义：空 vector 的 data() 为 nullptr，而 sqlite3_bind_blob 收到空指针时
// 等价于 sqlite3_bind_null，所以"空 BLOB"实际落库为 NULL，读回来是 Null 而不是
// 零长度 Blob —— 这套封装里 NULL 与空 BLOB 不可区分。
TEST_F(TempDatabaseTest, EmptyBlobIsStoredAsNull)
{
    createScalarTable(m_db);

    sqlite::SQLiteStatement insert(*m_db, "INSERT INTO scalars VALUES (1, 1.0, 'x', ?);");
    insert.bind(1, std::vector<uint8_t>{});
    insert.executeStep();

    sqlite::SQLiteStatement select(*m_db, "SELECT b FROM scalars;");
    ASSERT_TRUE(select.executeStep());

    EXPECT_TRUE(select.isColumnNull(0));
    EXPECT_EQ(select.column(0).getType(), sqlite::FieldType::Null);
    EXPECT_TRUE(select.column(0).getBlob().empty());
    EXPECT_TRUE(select.column(0).getString().empty());
}

TEST_F(TempDatabaseTest, ColumnMetadataIsAvailable)
{
    createScalarTable(m_db);
    ASSERT_TRUE(m_db->execute("INSERT INTO scalars VALUES (1, 1.0, 'a', NULL);"));

    sqlite::SQLiteStatement select(*m_db, "SELECT i AS alias, t FROM scalars;");
    select.executeStep();

    EXPECT_EQ(select.columnCount(), 2);
    EXPECT_EQ(select.columnName(0), "alias");
    EXPECT_EQ(select.columnName(1), "t");
    EXPECT_EQ(select.columnIndex("alias"), 0);
    EXPECT_EQ(select.columnIndex("t"), 1);
    // 未出现在结果集中的列名返回 -1，而不是抛异常
    EXPECT_EQ(select.columnIndex("nope"), -1);

    // sqlite3_column_origin_name 只有在库编译时打开了 SQLITE_ENABLE_COLUMN_METADATA
    // 才返回非空；否则返回 nullptr，而 SQLiteStatement::originColumnName() 没判空，
    // 直接 std::string(nullptr) 属于未定义行为。因此这里先探测编译选项再断言。
    if (sqlite3_compileoption_used("SQLITE_ENABLE_COLUMN_METADATA"))
    {
        EXPECT_EQ(select.originColumnName(1), "t");
        EXPECT_EQ(select.column(1).getOriginName(), "t");
    }
}

TEST_F(TempDatabaseTest, StepReportsRowDoneAndStaysFalseAfterDone)
{
    createScalarTable(m_db);
    ASSERT_TRUE(m_db->execute("INSERT INTO scalars VALUES (1, 1.0, 'a', NULL), (2, 2.0, 'b', NULL);"));

    sqlite::SQLiteStatement select(*m_db, "SELECT i FROM scalars ORDER BY i;");
    EXPECT_FALSE(select.isDone());

    ASSERT_TRUE(select.executeStep());
    EXPECT_FALSE(select.isDone());
    ASSERT_TRUE(select.executeStep());

    ASSERT_FALSE(select.executeStep());
    EXPECT_TRUE(select.isDone());
    EXPECT_FALSE(select.hasRow());
    // 已结束的语句再 step 不会重新产出数据
    EXPECT_FALSE(select.executeStep());
}

TEST_F(TempDatabaseTest, ResetAllowsReExecution)
{
    createScalarTable(m_db);

    sqlite::SQLiteStatement insert(*m_db, "INSERT INTO scalars VALUES (?, 1.0, ?, NULL);");
    for (int i = 0; i < 3; ++i)
    {
        insert.bind(1, int64_t{i + 1});
        insert.bind(2, std::string("name-") + std::to_string(i));
        insert.executeStep();
        EXPECT_TRUE(insert.reset());
    }

    sqlite::SQLiteStatement count(*m_db, "SELECT COUNT(1) FROM scalars;");
    ASSERT_TRUE(count.executeStep());
    EXPECT_EQ(count.column(0).getInt64(), 3);
}

TEST_F(TempDatabaseTest, ClearBindingsDropsPreviouslyBoundValues)
{
    createScalarTable(m_db);

    sqlite::SQLiteStatement insert(*m_db, "INSERT INTO scalars VALUES (1, 1.0, ?, NULL);");
    insert.bind(1, std::string("kept"));
    insert.executeStep();
    insert.reset();

    insert.clearBindings();
    insert.executeStep();

    sqlite::SQLiteStatement select(*m_db, "SELECT t FROM scalars;");
    ASSERT_TRUE(select.executeStep());
    EXPECT_EQ(select.column(0).getString(), "kept");
    ASSERT_TRUE(select.executeStep());
    EXPECT_TRUE(select.isColumnNull(0));
}

TEST_F(TempDatabaseTest, ReportsBindParameterCountAndNamedIndex)
{
    createScalarTable(m_db);

    sqlite::SQLiteStatement stmt(*m_db, "SELECT i FROM scalars WHERE i = :id AND t = :name;");
    EXPECT_EQ(stmt.getBindParameterCount(), 2);
    EXPECT_EQ(stmt.getIndex(":id"), 1);
    EXPECT_EQ(stmt.getIndex(":name"), 2);
    // 不存在的参数名返回 0
    EXPECT_EQ(stmt.getIndex(":nope"), 0);

    // 具名绑定等价于按序号绑定
    stmt.bind(":id", int64_t{7});
    stmt.bind(":name", std::string("n"));
    const std::string expanded = stmt.expandedSQL();
    EXPECT_NE(expanded.find("7"), std::string::npos);
}

TEST_F(TempDatabaseTest, ExpandedSqlInlinesBoundValues)
{
    createScalarTable(m_db);

    sqlite::SQLiteStatement stmt(*m_db, "SELECT * FROM scalars WHERE i = ? AND t = ?;");
    stmt.bind(1, int64_t{3});
    stmt.bind(2, std::string("abc"));

    const std::string expanded = stmt.expandedSQL();
    EXPECT_NE(expanded.find("3"), std::string::npos);
    EXPECT_NE(expanded.find("abc"), std::string::npos);
}

TEST_F(TempDatabaseTest, ChangesCountsAffectedRows)
{
    createScalarTable(m_db);
    ASSERT_TRUE(m_db->execute("INSERT INTO scalars VALUES (1, 1.0, 'a', NULL), (2, 2.0, 'b', NULL), (3, 3.0, 'c', NULL);"));

    sqlite::SQLiteStatement update(*m_db, "UPDATE scalars SET f = 9.0 WHERE i >= ?;");
    update.bind(1, int64_t{2});
    update.executeStep();

    EXPECT_EQ(update.getChanges(), 2);
}

TEST_F(TempDatabaseTest, InvalidSqlThrowsOnPrepare)
{
    EXPECT_THROW(sqlite::SQLiteStatement(*m_db, "SELCT 1;"), sqlite::SQLiteException);
    // 引用不存在的表：SQLite 在 prepare 阶段就会解析表名
    EXPECT_THROW(sqlite::SQLiteStatement(*m_db, "SELECT * FROM table_that_does_not_exist;"), sqlite::SQLiteException);
}

TEST_F(TempDatabaseTest, StepTimeSchemaErrorThrowsSqliteException)
{
    ASSERT_TRUE(m_db->execute("CREATE TABLE gone(x INTEGER);"));
    sqlite::SQLiteStatement stmt(*m_db, "SELECT * FROM gone;");
    // 语句编译好之后表被删掉，step 时才会报 SQLITE_ERROR
    ASSERT_TRUE(m_db->execute("DROP TABLE gone;"));

    EXPECT_THROW(stmt.executeStep(), sqlite::SQLiteException);
}

TEST_F(TempDatabaseTest, HandleAccessorsExposeDatabase)
{
    sqlite::SQLiteStatement stmt(*m_db, "SELECT 1;");
    EXPECT_NE(stmt.handle(), nullptr);
    EXPECT_EQ(stmt.dbHandle(), m_db->handle());
    EXPECT_EQ(&stmt.db(), static_cast<const sqlite::SQLiteDatabase*>(m_db.get()));
}

}  // namespace sqlite_test
