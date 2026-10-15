#include "sqlite_fixture.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace
{
using TableType = sqlite::TableStructInfo<SqliteTestEntity>;

sqlite::ColumnInfo columnOf(const std::string& name)
{
    return sqlite::ColumnInfo(name);
}

int64_t countWith(sqlite::SqliteWithMutexPtr& db, const std::string& table, const sqlite::ConditionWrapper& condition)
{
    return sqlite::SqliteUtil::countEntities(db, table, condition);
}
}  // namespace

// TEST_F 会把夹具名拼进生成类名，因此不能用 "ns::Fixture" 这种限定写法，
// 必须把用例放在夹具所在的命名空间内。
namespace sqlite_test
{

// ------------------------------------------------------------ 纯字符串层面

TEST(SqliteTest, EmptyConditionProducesNoWhereClause)
{
    const sqlite::ConditionWrapper condition;
    EXPECT_TRUE(condition.empty());
    EXPECT_EQ(condition.size(), 0u);
    EXPECT_TRUE(condition.prepareConditionString().empty());
    EXPECT_TRUE(condition.conditionString().empty());
    EXPECT_TRUE(condition.columnNames().empty());
}

TEST(SqliteTest, SingleConditionEmitsColumnOperatorPlaceholder)
{
    sqlite::ConditionWrapper condition;
    condition.addCondition(columnOf("name"), sqlite::Condition::EQUALS, std::string("a"));

    const std::string prepared = condition.prepareConditionString();
    EXPECT_EQ(prepared.rfind(" WHERE ", 0), 0u);
    EXPECT_NE(prepared.find("name"), std::string::npos);
    EXPECT_NE(prepared.find("=="), std::string::npos);
    EXPECT_NE(prepared.find('?'), std::string::npos);

    // conditionString() 内联字面量，供 CREATE VIEW 这类无法绑定参数的语句使用
    const std::string inlined = condition.conditionString();
    EXPECT_NE(inlined.find("'a'"), std::string::npos);
}

TEST(SqliteTest, ConditionOperatorsMapToSqlOperators)
{
    struct Case
    {
        sqlite::Condition condition;
        const char* sql;
    };
    const Case cases[] = {
        {sqlite::Condition::EQUALS,     "=="      },
        {sqlite::Condition::NOT_EQUALS, "<>"      },
        {sqlite::Condition::LE,         "<="      },
        {sqlite::Condition::GE,         ">="      },
        {sqlite::Condition::LT,         "<"       },
        {sqlite::Condition::GT,         ">"       },
        {sqlite::Condition::LIKE,       "LIKE"    },
        {sqlite::Condition::NOT_LIKE,   "NOT LIKE"},
    };

    for (const auto& testCase : cases)
    {
        sqlite::ConditionWrapper condition;
        const bool needsText = testCase.condition == sqlite::Condition::LIKE || testCase.condition == sqlite::Condition::NOT_LIKE;
        if (needsText)
        {
            condition.addCondition(columnOf("name"), testCase.condition, std::string("v"));
        }
        else
        {
            condition.addCondition(columnOf("score"), testCase.condition, 1.0);
        }

        EXPECT_NE(condition.prepareConditionString().find(testCase.sql), std::string::npos) << "operator: " << testCase.sql;
    }

    sqlite::ConditionWrapper inCondition;
    inCondition.addCondition(columnOf("id"), sqlite::Condition::VALUES_IN,
                             sqlite::ValueType{
                                 std::vector<sqlite::SqliteColumnValue>{int64_t{1}, int64_t{2}}
    });
    const std::string inSql = inCondition.prepareConditionString();
    EXPECT_NE(inSql.find("IN"), std::string::npos);
    // 两个值 -> 两个占位符
    EXPECT_EQ(std::count(inSql.begin(), inSql.end(), '?'), 2);
}

TEST(SqliteTest, InConditionPlaceholderCountMatchesValueCount)
{
    sqlite::ConditionWrapper condition;
    condition.addCondition(columnOf("id"), sqlite::Condition::VALUES_IN,
                           sqlite::ValueType{
                               std::vector<sqlite::SqliteColumnValue>{int64_t{1}, int64_t{2}, int64_t{3}}
    });

    const std::string sql = condition.prepareConditionString();
    EXPECT_EQ(std::count(sql.begin(), sql.end(), '?'), 3);
}

TEST(SqliteTest, MultipleConditionsAreParenthesisedWithRelation)
{
    sqlite::ConditionWrapper condition;
    condition.addCondition(columnOf("name"), sqlite::Condition::EQUALS, std::string("a"));
    condition.addCondition(columnOf("score"), sqlite::Condition::GT, 1.0);

    const std::string sql = condition.prepareConditionString();
    EXPECT_EQ(condition.size(), 2u);
    EXPECT_EQ(sql.find('('), sql.find(" WHERE ") + 7);
    EXPECT_NE(sql.find(" AND "), std::string::npos);
    EXPECT_EQ(sql.back(), ')');
}

TEST(SqliteTest, LikeConditionAppendsWildcard)
{
    sqlite::ConditionWrapper condition;
    condition.addCondition(columnOf("name"), sqlite::Condition::LIKE, std::string("ab"));

    // conditionString() 会带上 % 通配符
    EXPECT_NE(condition.conditionString().find("'ab%'"), std::string::npos);
}

// 回归：无值条件（IS NULL / IS NOT NULL）在 conditionString() 中曾经直接 std::get
// 抛 bad_variant_access。
TEST(SqliteTest, NullConditionsRenderLiteralNull)
{
    sqlite::ConditionWrapper isNull;
    isNull.addCondition(columnOf("name"), sqlite::Condition::IS_NULL);
    EXPECT_NO_THROW(isNull.conditionString());
    EXPECT_NE(isNull.conditionString().find("IS NULL"), std::string::npos);

    sqlite::ConditionWrapper isNotNull;
    isNotNull.addCondition(columnOf("name"), sqlite::Condition::IS_NOT_NULL);
    EXPECT_NE(isNotNull.conditionString().find("IS NOT NULL"), std::string::npos);
}

TEST(SqliteTest, ContainAndColumnNamesWalkNestedWrappers)
{
    sqlite::ConditionWrapper inner;
    inner.addCondition(columnOf("name"), sqlite::Condition::EQUALS, std::string("a"));
    inner.addCondition(columnOf("score"), sqlite::Condition::GT, 1.0);

    sqlite::ConditionWrapper outer;
    outer.addCondition(columnOf("id"), sqlite::Condition::EQUALS, int64_t{1});
    outer.addCondition(inner);

    EXPECT_TRUE(outer.contain("id"));
    EXPECT_TRUE(outer.contain("name"));
    EXPECT_TRUE(outer.contain(columnOf("score")));
    EXPECT_FALSE(outer.contain("absent"));

    // 回归：columnNames() 曾经直接返回空集合
    const auto names = outer.columnNames();
    EXPECT_EQ(names.size(), 3u);
    EXPECT_EQ(names.count("id"), 1u);
    EXPECT_EQ(names.count("name"), 1u);
    EXPECT_EQ(names.count("score"), 1u);
}

TEST(SqliteTest, PopConditionRemovesByColumnName)
{
    sqlite::ConditionWrapper condition;
    condition.addCondition(columnOf("a"), sqlite::Condition::EQUALS, int64_t{1});
    condition.addCondition(columnOf("b"), sqlite::Condition::EQUALS, int64_t{2});
    condition.addCondition(columnOf("c"), sqlite::Condition::EQUALS, int64_t{3});

    // 回归：删除首元素时旧实现执行 it--，越过 begin() 属于未定义行为
    condition.popCondition("a");
    EXPECT_EQ(condition.size(), 2u);
    EXPECT_FALSE(condition.contain("a"));
    EXPECT_TRUE(condition.contain("b"));
    EXPECT_TRUE(condition.contain("c"));

    condition.popCondition(columnOf("c"));
    EXPECT_EQ(condition.size(), 1u);
    EXPECT_TRUE(condition.contain("b"));

    // 删除不存在的列不应改变任何东西
    condition.popCondition("absent");
    EXPECT_EQ(condition.size(), 1u);
}

TEST(SqliteTest, PopConditionReachesNestedWrappers)
{
    sqlite::ConditionWrapper inner;
    inner.addCondition(columnOf("inner"), sqlite::Condition::EQUALS, int64_t{1});

    sqlite::ConditionWrapper outer;
    outer.addCondition(columnOf("other"), sqlite::Condition::EQUALS, int64_t{2});
    outer.addCondition(inner);

    outer.popCondition("inner");
    EXPECT_FALSE(outer.contain("inner"));
    EXPECT_TRUE(outer.contain("other"));
}

TEST(SqliteTest, ResetClearsEverything)
{
    sqlite::ConditionWrapper condition;
    condition.addCondition(columnOf("a"), sqlite::Condition::EQUALS, int64_t{1});
    condition.or_();
    condition.addCondition(columnOf("b"), sqlite::Condition::EQUALS, int64_t{2});
    ASSERT_FALSE(condition.empty());

    condition.reset();
    EXPECT_TRUE(condition.empty());
    EXPECT_EQ(condition.size(), 0u);
    EXPECT_TRUE(condition.prepareConditionString().empty());
}

// 回归：mergeOptimization 在合并候选位于首位时执行 erase(it) 后又 it--，
// 属于未定义行为（可能崩溃）。
TEST(SqliteTest, DuplicateConditionsCollapseWithoutCrash)
{
    sqlite::ConditionWrapper condition;
    condition.addCondition(columnOf("name"), sqlite::Condition::EQUALS, std::string("a"));
    for (int i = 0; i < 5; ++i)
    {
        condition.addCondition(columnOf("name"), sqlite::Condition::EQUALS, std::string("a"));
    }

    EXPECT_EQ(condition.size(), 1u);
}

TEST(SqliteTest, RangeConditionsMergeToOneTighterBound)
{
    // score >= 10 AND score >= 5 会被合并成单个条件，边界取较紧的 10
    sqlite::ConditionWrapper tightFirst;
    tightFirst.addCondition(columnOf("score"), sqlite::Condition::GE, 10.0);
    tightFirst.addCondition(columnOf("score"), sqlite::Condition::GE, 5.0);
    EXPECT_EQ(tightFirst.size(), 1u);

    // 反向顺序同样合并，边界仍取较紧的 10（而不是先写入的 5）
    sqlite::ConditionWrapper looseFirst;
    looseFirst.addCondition(columnOf("score"), sqlite::Condition::GE, 5.0);
    looseFirst.addCondition(columnOf("score"), sqlite::Condition::GE, 10.0);
    EXPECT_EQ(looseFirst.size(), 1u);

    EXPECT_NE(tightFirst.conditionString().find("10"), std::string::npos);
    EXPECT_NE(looseFirst.conditionString().find("10"), std::string::npos);
}

// ------------------------------------------------------------ 真实数据库行为

namespace
{
class ConditionBehaviourTest : public sqlite_test::TempDatabaseTest
{
protected:
    void SetUp() override
    {
        sqlite_test::TempDatabaseTest::SetUp();
        createEntityTable();
        ASSERT_TRUE(sqlite::SqliteUtil::insertEntities<SqliteTestEntity>(
                        m_db, "entity", {makeEntity(1, "alpha", 1.0), makeEntity(2, "beta", 2.0), makeEntity(3, "gamma", 3.0)}) == 3);
        // 额外插入一行 name 为 NULL 的记录，用于 IS NULL / IS NOT NULL
        ASSERT_TRUE(m_db->execute("INSERT INTO entity(id, name, score, payload) VALUES (4, NULL, 4.0, NULL);"));
    }
};
}  // namespace

TEST_F(ConditionBehaviourTest, EqualityAndRangeConditionsFilterRows)
{
    sqlite::ConditionWrapper equals;
    equals.addCondition(TableType::self().name, sqlite::Condition::EQUALS, std::string("alpha"));
    EXPECT_EQ(countWith(m_db, "entity", equals), 1);

    sqlite::ConditionWrapper notEquals;
    notEquals.addCondition(TableType::self().name, sqlite::Condition::NOT_EQUALS, std::string("alpha"));
    // NULL 不参与 <> 比较，因此只剩 beta / gamma
    EXPECT_EQ(countWith(m_db, "entity", notEquals), 2);

    sqlite::ConditionWrapper range;
    range.addCondition(TableType::self().score, sqlite::Condition::GE, 2.0);
    range.addCondition(TableType::self().score, sqlite::Condition::LT, 4.0);
    EXPECT_EQ(countWith(m_db, "entity", range), 2);
}

TEST_F(ConditionBehaviourTest, LikeAndInConditionsFilterRows)
{
    sqlite::ConditionWrapper like;
    like.addCondition(TableType::self().name, sqlite::Condition::LIKE, std::string("a"));
    EXPECT_EQ(countWith(m_db, "entity", like), 1);

    sqlite::ConditionWrapper in;
    in.addCondition(TableType::self().id, sqlite::Condition::VALUES_IN,
                    sqlite::ValueType{
                        std::vector<sqlite::SqliteColumnValue>{int64_t{1}, int64_t{3}}
    });
    EXPECT_EQ(countWith(m_db, "entity", in), 2);

    sqlite::ConditionWrapper notIn;
    notIn.addCondition(TableType::self().id, sqlite::Condition::VALUES_NOT_IN,
                       sqlite::ValueType{
                           std::vector<sqlite::SqliteColumnValue>{int64_t{1}, int64_t{3}}
    });
    EXPECT_EQ(countWith(m_db, "entity", notIn), 2);
}

TEST_F(ConditionBehaviourTest, NullConditionsFilterRows)
{
    sqlite::ConditionWrapper isNull;
    isNull.addCondition(TableType::self().name, sqlite::Condition::IS_NULL);
    EXPECT_EQ(countWith(m_db, "entity", isNull), 1);

    sqlite::ConditionWrapper isNotNull;
    isNotNull.addCondition(TableType::self().name, sqlite::Condition::IS_NOT_NULL);
    EXPECT_EQ(countWith(m_db, "entity", isNotNull), 3);
}

TEST_F(ConditionBehaviourTest, AndCombinesDifferentColumns)
{
    sqlite::ConditionWrapper byName;
    byName.addCondition(TableType::self().name, sqlite::Condition::EQUALS, std::string("beta"));

    sqlite::ConditionWrapper byScore;
    byScore.addCondition(TableType::self().score, sqlite::Condition::GT, 1.0);

    sqlite::ConditionWrapper combined = byName & byScore;
    EXPECT_EQ(countWith(m_db, "entity", combined), 1);

    sqlite::ConditionWrapper impossible = byName;
    impossible.addCondition(TableType::self().score, sqlite::Condition::GT, 100.0);
    EXPECT_EQ(countWith(m_db, "entity", impossible), 0);
}

TEST_F(ConditionBehaviourTest, OrCombinesConditions)
{
    sqlite::ConditionWrapper left;
    left.addCondition(TableType::self().name, sqlite::Condition::EQUALS, std::string("alpha"));

    sqlite::ConditionWrapper right;
    right.addCondition(TableType::self().name, sqlite::Condition::EQUALS, std::string("gamma"));

    sqlite::ConditionWrapper combined = left | right;
    EXPECT_EQ(combined.size(), 2u);
    EXPECT_NE(combined.prepareConditionString().find(" OR "), std::string::npos);
    EXPECT_EQ(countWith(m_db, "entity", combined), 2);
}

// 空值集合会静默丢弃整个条件（只记一条 warn 日志），调用方若期望"空集合 = 无匹配"
// 会得到"不过滤"的结果。这里把当前行为固定下来，便于将来收紧。
TEST(SqliteTest, EmptyInValueSetSilentlyDropsTheCondition)
{
    sqlite::ConditionWrapper condition;
    condition.addCondition(columnOf("id"), sqlite::Condition::VALUES_IN, sqlite::ValueType{std::vector<sqlite::SqliteColumnValue>{}});

    EXPECT_EQ(condition.size(), 1u);
    EXPECT_TRUE(condition.prepareConditionString().empty());
}

TEST_F(TempDatabaseTest, BindNumbersParametersSequentially)
{
    sqlite::ConditionWrapper condition;
    condition.addCondition(columnOf("a"), sqlite::Condition::EQUALS, int64_t{1});
    condition.addCondition(columnOf("b"), sqlite::Condition::VALUES_IN,
                           sqlite::ValueType{
                               std::vector<sqlite::SqliteColumnValue>{int64_t{2}, int64_t{3}}
    });

    // 1 个普通条件 + 2 个 IN 值 = 3 个参数，下一个可用序号是 4
    sqlite::SQLiteStatement stmt(*m_db, "SELECT ? AS p1, ? AS p2, ? AS p3;");
    EXPECT_EQ(condition.bind(stmt), 4);

    ASSERT_TRUE(stmt.executeStep());
    EXPECT_EQ(stmt.column(0).getInt64(), 1);
    EXPECT_EQ(stmt.column(1).getInt64(), 2);
    EXPECT_EQ(stmt.column(2).getInt64(), 3);
}

TEST_F(ConditionBehaviourTest, NestedWrapperKeepsOwnRelation)
{
    // 先用 OR 组成一组，再与另一个条件做 AND：嵌套包装器保留自己的 OR
    sqlite::ConditionWrapper inner;
    inner.addCondition(TableType::self().name, sqlite::Condition::EQUALS, std::string("alpha"));
    inner.addCondition(TableType::self().name, sqlite::Condition::EQUALS, std::string("beta"));
    inner.or_();

    sqlite::ConditionWrapper outer;
    outer.addCondition(TableType::self().score, sqlite::Condition::LT, 2.0);
    outer.addCondition(inner);

    const std::string sql = outer.prepareConditionString();
    EXPECT_NE(sql.find(" OR "), std::string::npos);
    EXPECT_NE(sql.find(" AND "), std::string::npos);
    // alpha(1.0) 命中；beta(2.0) 被 AND score < 2.0 排除
    EXPECT_EQ(countWith(m_db, "entity", outer), 1);
}

}  // namespace sqlite_test
