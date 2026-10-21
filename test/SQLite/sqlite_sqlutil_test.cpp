#include "sqlite_fixture.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace
{
using Entity = SqliteTestEntity;
using Table = sqlite::TableStructInfo<Entity>;

int64_t scalarCount(sqlite::SqliteWithMutexPtr& db, const std::string& table)
{
    sqlite::SQLiteStatement stmt(*db, "SELECT COUNT(1) FROM " + table + ";");
    if (stmt.executeStep())
    {
        return stmt.column(0).getInt64();
    }
    return -1;
}

bool objectExists(sqlite::SqliteWithMutexPtr& db, const std::string& type, const std::string& name)
{
    sqlite::SQLiteStatement stmt(*db, "SELECT COUNT(1) FROM sqlite_master WHERE type = ? AND name = ?;");
    stmt.bind(1, type);
    stmt.bind(2, name);
    return stmt.executeStep() && stmt.column(0).getInt64() == 1;
}
}  // namespace

// TEST_F 会把夹具名拼进生成类名，因此不能用 "ns::Fixture" 这种限定写法，
// 必须把用例放在夹具所在的命名空间内。
namespace sqlite_test
{

// ---------------------------------------------------------------- 建表 / 改名

TEST_F(TempDatabaseTest, CreateTableIfNotExistsIsIdempotent)
{
    EXPECT_TRUE(sqlite::SqliteUtil::createTableIfNotExists(m_db, "entity", Table::self()));
    EXPECT_TRUE(m_db->tableExists("entity"));
    // 第二次调用说明表已存在，返回 false
    EXPECT_FALSE(sqlite::SqliteUtil::createTableIfNotExists(m_db, "entity", Table::self()));
}

TEST_F(TempDatabaseTest, CreateTableRejectsEmptyNameAndNullDb)
{
    EXPECT_FALSE(sqlite::SqliteUtil::createTableIfNotExists(m_db, "", Table::self()));
    sqlite::SqliteWithMutexPtr nullDb;
    EXPECT_FALSE(sqlite::SqliteUtil::createTableIfNotExists(nullDb, "entity", Table::self()));
}

// 已知缺陷：autoIncrement=true 的列在 ColumnInfo::columnSql() 里已经带上了
// "PRIMARY KEY AUTOINCREMENT"，而 BaseTableStructInfo::createSql() 又会追加
// 表级 "PRIMARY KEY(id)"，叠加后 SQLite 报 "more than one primary key"。
// 仓库内暂时没有实体使用 autoIncrement，因此该分支是潜在缺陷；这里把现状固定下来。
TEST_F(TempDatabaseTest, AutoIncrementColumnGeneratesDuplicatePrimaryKey)
{
    const auto& tableStruct = sqlite::TableStructInfo<SqliteAutoIncrementEntity>::self();
    const std::string createSql = tableStruct.createSql();

    EXPECT_NE(createSql.find("PRIMARY KEY AUTOINCREMENT"), std::string::npos);
    EXPECT_NE(createSql.find(", PRIMARY KEY("), std::string::npos);
    EXPECT_FALSE(sqlite::SqliteUtil::createTableIfNotExists(m_db, "autoinc", tableStruct));
    EXPECT_FALSE(m_db->tableExists("autoinc"));
}

// 回归：renameTable 曾经生成 "ALTER TABLE a RENAME b"（漏了 TO），语句无法执行。
TEST_F(TempDatabaseTest, RenameTableRenamesInPlace)
{
    createEntityTable("before");
    ASSERT_TRUE(m_db->execute("INSERT INTO before(id, name, score, payload) VALUES (1, 'a', 1.0, NULL);"));

    EXPECT_TRUE(sqlite::SqliteUtil::renameTable(m_db, "before", "after"));
    EXPECT_FALSE(m_db->tableExists("before"));
    EXPECT_TRUE(m_db->tableExists("after"));
    // 数据随表一起迁移
    EXPECT_EQ(scalarCount(m_db, "after"), 1);
}

TEST_F(TempDatabaseTest, RenameTableRejectsConflictsAndEmptyNames)
{
    createEntityTable("src");
    sqlite::SqliteUtil::createTableIfNotExists(m_db, "dst", Table::self());

    // 目标表已存在
    EXPECT_FALSE(sqlite::SqliteUtil::renameTable(m_db, "src", "dst"));
    EXPECT_FALSE(sqlite::SqliteUtil::renameTable(m_db, "", "dst"));
    EXPECT_FALSE(sqlite::SqliteUtil::renameTable(m_db, "src", ""));
    // 源表不存在
    EXPECT_FALSE(sqlite::SqliteUtil::renameTable(m_db, "nope", "other"));

    sqlite::SqliteWithMutexPtr nullDb;
    EXPECT_FALSE(sqlite::SqliteUtil::renameTable(nullDb, "src", "dst"));
}

TEST_F(TempDatabaseTest, DeleteTableDropsRowsButKeepsSchema)
{
    createEntityTable();
    ASSERT_TRUE(m_db->execute("INSERT INTO entity(id, name, score, payload) VALUES (1, 'a', 1.0, NULL);"));

    EXPECT_TRUE(sqlite::SqliteUtil::deleteTable(m_db, "entity"));
    EXPECT_EQ(scalarCount(m_db, "entity"), 0);
    EXPECT_TRUE(m_db->tableExists("entity"));
    EXPECT_FALSE(sqlite::SqliteUtil::deleteTable(m_db, "missing_table"));
}

TEST_F(TempDatabaseTest, DropTableIfExistsRemovesSchema)
{
    createEntityTable();

    EXPECT_TRUE(sqlite::SqliteUtil::dropTableIfExists(m_db, "entity"));
    EXPECT_FALSE(m_db->tableExists("entity"));
    // IF EXISTS：重复删除仍然是成功
    EXPECT_TRUE(sqlite::SqliteUtil::dropTableIfExists(m_db, "entity"));
    EXPECT_FALSE(sqlite::SqliteUtil::dropTableIfExists(m_db, ""));
}

// ---------------------------------------------------------------- 索引 / 视图

TEST(SqliteTest, IndexNameJoinsTagAndColumns)
{
    EXPECT_EQ(sqlite::SqliteUtil::indexName("tag", {"a", "b"}), "tag_a_b_index");
    EXPECT_EQ(sqlite::SqliteUtil::indexName("tag", {"only"}), "tag_only_index");
    // 无列名时中间留空
    EXPECT_EQ(sqlite::SqliteUtil::indexName("tag", {}), "tag__index");
}

TEST_F(TempDatabaseTest, CreateIndexIfNotExistsCreatesIndexOnce)
{
    createEntityTable();

    const auto name = sqlite::SqliteUtil::indexName("entity", {"name", "score"});
    EXPECT_TRUE(sqlite::SqliteUtil::createIndexIfNotExists(m_db, "entity", name, {"name", "score"}));
    EXPECT_TRUE(objectExists(m_db, "index", name));
    // IF NOT EXISTS：重复创建不报错
    EXPECT_TRUE(sqlite::SqliteUtil::createIndexIfNotExists(m_db, "entity", name, {"name", "score"}));

    // 参数不合法时直接返回 false，不落库
    EXPECT_FALSE(sqlite::SqliteUtil::createIndexIfNotExists(m_db, "entity", "idx", {}));
    EXPECT_FALSE(sqlite::SqliteUtil::createIndexIfNotExists(m_db, "", "idx", {"name"}));
    EXPECT_FALSE(sqlite::SqliteUtil::createIndexIfNotExists(m_db, "entity", "", {"name"}));
}

TEST_F(TempDatabaseTest, CreateAndDropView)
{
    createEntityTable();
    ASSERT_TRUE(m_db->execute("INSERT INTO entity(id, name, score, payload) VALUES (1, 'a', 1.0, NULL), (2, 'b', 2.0, NULL);"));

    sqlite::ConditionWrapper condition;
    condition.addCondition(Table::self().name, sqlite::Condition::EQUALS, std::string("a"));

    EXPECT_TRUE(sqlite::SqliteUtil::createViewIfNotExist(m_db, "view_a", "entity", condition));
    EXPECT_TRUE(objectExists(m_db, "view", "view_a"));
    EXPECT_EQ(m_db->views().count("view_a"), 1u);

    // 视图内容按条件过滤
    sqlite::SQLiteStatement select(*m_db, "SELECT name FROM view_a;");
    ASSERT_TRUE(select.executeStep());
    EXPECT_EQ(select.column(0).getString(), "a");
    EXPECT_FALSE(select.executeStep());

    EXPECT_TRUE(sqlite::SqliteUtil::dropViewIfExists(m_db, "view_a"));
    EXPECT_EQ(m_db->views().count("view_a"), 0u);
    EXPECT_FALSE(sqlite::SqliteUtil::createViewIfNotExist(m_db, "", "entity"));
}

TEST_F(TempDatabaseTest, ViewConditionEscapesQuotedValues)
{
    createEntityTable();
    ASSERT_TRUE(m_db->execute("INSERT INTO entity(id, name, score, payload) VALUES (1, 'O''Brien', 1.0, NULL), (2, 'other', 2.0, NULL);"));
    sqlite::ConditionWrapper condition;
    condition.addCondition(Table::self().name, sqlite::Condition::EQUALS, std::string("O'Brien"));
    ASSERT_TRUE(sqlite::SqliteUtil::createViewIfNotExist(m_db, "quoted_view", "entity", condition));
    EXPECT_EQ(scalarCount(m_db, "quoted_view"), 1);
}

// ---------------------------------------------------------------- 实体读写

TEST_F(TempDatabaseTest, InsertAndQueryEntities)
{
    createEntityTable();

    std::vector<Entity> entities{
        makeEntity(1, "alpha", 1.5, {0x00, 0x01}),
        makeEntity(2, "beta", 2.5, {0xFE}),
        makeEntity(3, "gamma", 3.5, {}),
    };

    EXPECT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "entity", entities), 3);
    EXPECT_EQ(scalarCount(m_db, "entity"), 3);

    auto all = sqlite::SqliteUtil::queryEntities<Entity>(m_db, "entity", sqlite::ConditionWrapper{});
    ASSERT_EQ(all.size(), 3u);
    for (const auto& loaded : all)
    {
        const auto expected = entities.at(static_cast<size_t>(loaded.id - 1));
        EXPECT_EQ(loaded.name, expected.name);
        EXPECT_DOUBLE_EQ(loaded.score, expected.score);
        EXPECT_EQ(loaded.payload, expected.payload);
    }
}

TEST_F(TempDatabaseTest, InsertEntitiesSkipsEmptyInput)
{
    createEntityTable();
    EXPECT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "entity", {}), 0);
    EXPECT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "", {makeEntity(1, "a", 1.0)}), 0);

    sqlite::SqliteWithMutexPtr nullDb;
    EXPECT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(nullDb, "entity", {makeEntity(1, "a", 1.0)}), 0);
}

TEST_F(TempDatabaseTest, QueryEntitiesAppliesConditionAndPaging)
{
    createEntityTable();
    std::vector<Entity> entities;
    for (int i = 1; i <= 5; ++i)
    {
        entities.push_back(makeEntity(i, i % 2 == 0 ? "even" : "odd", static_cast<double>(i)));
    }
    ASSERT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "entity", entities), 5);

    sqlite::ConditionWrapper condition;
    condition.addCondition(Table::self().name, sqlite::Condition::EQUALS, std::string("even"));
    condition.addCondition(Table::self().score, sqlite::Condition::GT, 0.0);

    auto filtered = sqlite::SqliteUtil::queryEntities<Entity>(m_db, "entity", condition);
    ASSERT_EQ(filtered.size(), 2u);
    for (const auto& entity : filtered)
    {
        EXPECT_EQ(entity.name, "even");
    }

    // 空条件 + 分页
    auto page = sqlite::SqliteUtil::queryEntities<Entity>(m_db, "entity", 1, 2);
    EXPECT_EQ(page.size(), 2u);

    // 条件 + 分页
    auto pageWithCondition = sqlite::SqliteUtil::queryEntities<Entity>(m_db, "entity", 0, 1, condition);
    EXPECT_EQ(pageWithCondition.size(), 1u);

    // 非法入参
    EXPECT_TRUE(sqlite::SqliteUtil::queryEntities<Entity>(m_db, "", condition).empty());
    sqlite::SqliteWithMutexPtr nullDb;
    EXPECT_TRUE(sqlite::SqliteUtil::queryEntities<Entity>(nullDb, "entity", condition).empty());
}

TEST_F(TempDatabaseTest, QueryEntitiesAcceptsRawSql)
{
    createEntityTable();
    ASSERT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "entity", {makeEntity(1, "a", 1.0), makeEntity(2, "bb", 2.0)}), 2);

    auto entities = sqlite::SqliteUtil::queryEntities<Entity>(m_db, "SELECT * FROM entity WHERE name = 'bb';");
    ASSERT_EQ(entities.size(), 1u);
    EXPECT_EQ(entities.front().name, "bb");

    EXPECT_TRUE(sqlite::SqliteUtil::queryEntities<Entity>(m_db, "").empty());
}

TEST_F(TempDatabaseTest, UpdateEntitiesRoundTrip)
{
    createEntityTable();
    ASSERT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "entity", {makeEntity(1, "a", 1.0), makeEntity(2, "b", 2.0)}), 2);

    auto modified = makeEntity(1, "renamed", 9.5, {7, 7});
    EXPECT_EQ(sqlite::SqliteUtil::updateEntities<Entity>(m_db, "entity", {modified}), 1);

    sqlite::ConditionWrapper condition;
    condition.addCondition(Table::self().id, sqlite::Condition::EQUALS, int64_t{1});
    auto loaded = sqlite::SqliteUtil::queryEntities<Entity>(m_db, "entity", condition);
    ASSERT_EQ(loaded.size(), 1u);
    EXPECT_EQ(loaded.front().name, "renamed");
    EXPECT_DOUBLE_EQ(loaded.front().score, 9.5);
    EXPECT_EQ(loaded.front().payload, (std::vector<uint8_t>{7, 7}));

    // 另一行不受影响
    condition.reset();
    condition.addCondition(Table::self().id, sqlite::Condition::EQUALS, int64_t{2});
    auto untouched = sqlite::SqliteUtil::queryEntities<Entity>(m_db, "entity", condition);
    ASSERT_EQ(untouched.size(), 1u);
    EXPECT_EQ(untouched.front().name, "b");
}

TEST_F(TempDatabaseTest, UpdateEntitiesRejectsEmptyInput)
{
    createEntityTable();
    EXPECT_EQ(sqlite::SqliteUtil::updateEntities<Entity>(m_db, "entity", {}), 0);
    EXPECT_EQ(sqlite::SqliteUtil::updateEntities<Entity>(m_db, "", {makeEntity(1, "a", 1.0)}), 0);
}

// 回归：updateEntitiesWithOutTrans 曾经误调用 insertEntitiesCore，
// 对 UPDATE 语句只绑定了 SET 部分、WHERE 参数保持 NULL，结果"改 0 行但返回成功"。
TEST_F(TempDatabaseTest, UpdateEntitiesWithOutTransActuallyUpdatesRows)
{
    createEntityTable();
    ASSERT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "entity", {makeEntity(1, "a", 1.0), makeEntity(2, "b", 2.0)}), 2);

    EXPECT_EQ(sqlite::SqliteUtil::updateEntitiesWithOutTrans<Entity>(m_db, "entity", {makeEntity(1, "patched", 4.0)}), 1);

    sqlite::ConditionWrapper condition;
    condition.addCondition(Table::self().id, sqlite::Condition::EQUALS, int64_t{1});
    auto loaded = sqlite::SqliteUtil::queryEntities<Entity>(m_db, "entity", condition);
    ASSERT_EQ(loaded.size(), 1u);
    EXPECT_EQ(loaded.front().name, "patched");
    EXPECT_DOUBLE_EQ(loaded.front().score, 4.0);
}

TEST_F(TempDatabaseTest, InsertEntitiesWithOutTransWritesWithoutExplicitTransaction)
{
    createEntityTable();
    const std::vector<Entity> entities{makeEntity(1, "a", 1.0), makeEntity(2, "b", 2.0)};

    EXPECT_EQ(sqlite::SqliteUtil::insertEntitiesWithOutTrans<Entity>(m_db, "entity", entities), 2);
    EXPECT_EQ(scalarCount(m_db, "entity"), 2);
}

TEST_F(TempDatabaseTest, UpdateSelectedColumnsByCondition)
{
    createEntityTable();
    ASSERT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "entity", {makeEntity(1, "a", 1.0), makeEntity(2, "b", 2.0), makeEntity(3, "c", 3.0)}), 3);

    const std::vector<sqlite::SqliteColumn> newValues{sqlite::SqliteColumn(std::string("bulk"), 0, "name")};
    sqlite::ConditionWrapper condition;
    condition.addCondition(Table::self().score, sqlite::Condition::GE, 2.0);

    EXPECT_EQ(sqlite::SqliteUtil::updateEntities(m_db, "entity", newValues, condition), 2);

    condition.reset();
    condition.addCondition(Table::self().name, sqlite::Condition::EQUALS, std::string("bulk"));
    EXPECT_EQ(sqlite::SqliteUtil::countEntities(m_db, "entity", condition), 2);
}

TEST(SqliteTest, PrepareSqlBuildsAssignmentList)
{
    const std::vector<sqlite::SqliteColumn> columns{
        sqlite::SqliteColumn(int64_t{1}, 0, "a"),
        sqlite::SqliteColumn(std::string("x"), 1, "b"),
    };
    EXPECT_EQ(sqlite::prepareSql(columns), "a = ?,b = ?");
    EXPECT_TRUE(sqlite::prepareSql({}).empty());
}

TEST(SqliteTest, StringJoinUsesGivenSeparator)
{
    const std::vector<std::string> items{"a", "b", "c"};
    EXPECT_EQ(sqlite::stringJoin(items.begin(), items.end()), "a b c");
    EXPECT_EQ(sqlite::stringJoin(items.begin(), items.end(), ", "), "a, b, c");
    EXPECT_EQ(sqlite::stringJoin(items.begin(), items.begin()), "");
}

// ---------------------------------------------------------------- 删除 / 计数 / 去重

TEST_F(TempDatabaseTest, DeleteEntitiesHonoursCondition)
{
    createEntityTable();
    ASSERT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "entity", {makeEntity(1, "a", 1.0), makeEntity(2, "b", 2.0), makeEntity(3, "b", 3.0)}), 3);

    sqlite::ConditionWrapper condition;
    condition.addCondition(Table::self().name, sqlite::Condition::EQUALS, std::string("b"));

    EXPECT_TRUE(sqlite::SqliteUtil::deleteEntities(m_db, "entity", condition));
    EXPECT_EQ(scalarCount(m_db, "entity"), 1);

    // 空条件等价于清空
    EXPECT_TRUE(sqlite::SqliteUtil::deleteEntities(m_db, "entity"));
    EXPECT_EQ(scalarCount(m_db, "entity"), 0);
}

TEST_F(TempDatabaseTest, DeleteEntitiesRejectsBadArguments)
{
    createEntityTable();
    EXPECT_FALSE(sqlite::SqliteUtil::deleteEntities(m_db, ""));
    sqlite::SqliteWithMutexPtr nullDb;
    EXPECT_FALSE(sqlite::SqliteUtil::deleteEntities(nullDb, "entity"));
}

TEST_F(TempDatabaseTest, DeleteEntitiesBindsQuotedValues)
{
    createEntityTable();
    ASSERT_TRUE(m_db->execute("INSERT INTO entity(id, name, score, payload) VALUES (1, 'O''Brien', 1.0, NULL), (2, 'other', 2.0, NULL);"));
    sqlite::ConditionWrapper condition;
    condition.addCondition(Table::self().name, sqlite::Condition::EQUALS, std::string("O'Brien"));
    ASSERT_TRUE(sqlite::SqliteUtil::deleteEntities(m_db, "entity", condition));
    EXPECT_EQ(scalarCount(m_db, "entity"), 1);
}

TEST_F(TempDatabaseTest, CountEntitiesUsesCondition)
{
    createEntityTable();
    EXPECT_EQ(sqlite::SqliteUtil::countEntities(m_db, "entity"), 0);

    ASSERT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "entity", {makeEntity(1, "a", 1.0), makeEntity(2, "b", 2.0), makeEntity(3, "b", 3.0)}), 3);
    EXPECT_EQ(sqlite::SqliteUtil::countEntities(m_db, "entity"), 3);

    sqlite::ConditionWrapper condition;
    condition.addCondition(Table::self().name, sqlite::Condition::EQUALS, std::string("b"));
    EXPECT_EQ(sqlite::SqliteUtil::countEntities(m_db, "entity", condition), 2);
}

TEST_F(TempDatabaseTest, CountEntitiesReturnsMinusOneForBadArguments)
{
    EXPECT_EQ(sqlite::SqliteUtil::countEntities(m_db, ""), -1);
    sqlite::SqliteWithMutexPtr nullDb;
    EXPECT_EQ(sqlite::SqliteUtil::countEntities(nullDb, "entity"), -1);
}

// 回归：ConditionWrapper::columnNames() 曾经直接返回空集合，
// 导致这里生成的子查询缺少条件引用的列，SQL 报 "no such column"。
TEST_F(TempDatabaseTest, QueryDistinctColSupportsConditionsOnOtherColumns)
{
    createEntityTable();
    ASSERT_EQ(sqlite::SqliteUtil::insertEntities<Entity>(m_db, "entity", {makeEntity(1, "a", 1.0), makeEntity(2, "b", 2.0), makeEntity(3, "a", 3.0)}), 3);

    const sqlite::ColumnInfo nameColumn("name");

    auto all = sqlite::SqliteUtil::queryDistinctCol(m_db, "entity", nameColumn);
    EXPECT_EQ(all.size(), 2u);

    sqlite::ConditionWrapper condition;
    condition.addCondition(Table::self().score, sqlite::Condition::GE, 2.0);

    auto filtered = sqlite::SqliteUtil::queryDistinctCol(m_db, "entity", nameColumn, condition);
    ASSERT_EQ(filtered.size(), 2u);
    EXPECT_NE(std::find(filtered.begin(), filtered.end(), "a"), filtered.end());
    EXPECT_NE(std::find(filtered.begin(), filtered.end(), "b"), filtered.end());
}

TEST_F(TempDatabaseTest, QueryDistinctColRejectsBadArguments)
{
    createEntityTable();
    const sqlite::ColumnInfo nameColumn("name");

    EXPECT_TRUE(sqlite::SqliteUtil::queryDistinctCol(m_db, "", nameColumn).empty());
    EXPECT_TRUE(sqlite::SqliteUtil::queryDistinctCol(m_db, "entity", sqlite::ColumnInfo("")).empty());

    sqlite::SqliteWithMutexPtr nullDb;
    EXPECT_TRUE(sqlite::SqliteUtil::queryDistinctCol(nullDb, "entity", nameColumn).empty());
}

}  // namespace sqlite_test
