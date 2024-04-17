#pragma once
#include <variant>
#include <vector>
#include <string>

#include "BaseInfo.h"
#include "Sqlite/Database/SQLiteColunm.h"
#include "Sqlite/Database/SQLiteStatement.h"

namespace SQLite
{
class SQLiteStatement;
enum class QueryCondition
{
    EQUALS,
    NOT_EQUALS,
    LE,
    GE,
    LT,
    GT,
    VALUES_IN,
    VALUES_NOT_IN,
    LIKE,
    NOT_LIKE,
    IS_NOT_NULL,
    IS_NULL
};

enum class ConditionLogicRelation
{
    AND,
    OR
};

static std::string getQueryConditionName(QueryCondition condition);
static std::string getConditionRelationName(ConditionLogicRelation relation);

using ValueType = std::variant<std::monostate, SqliteColumnValue, std::vector<SqliteColumnValue>>;
struct PrepareInfo
{
    std::string m_columnName;
    QueryCondition m_condition;
    ValueType m_value;

    static bool valueSetEqual(const std::vector<SqliteColumnValue>& lv, const std::vector<SqliteColumnValue>& rv);
    bool operator==(const PrepareInfo& other) const;

    bool like(const PrepareInfo& other) const;

    std::stringstream& prepareConditionString(std::stringstream& ss) const;
    int bind(SQLiteStatement& stmt, int startIndex = 0) const;
    std::stringstream& conditionString(std::stringstream& ss) const;
};

class QueryWrapper;
using ConditionInfo = std::variant<PrepareInfo, QueryWrapper>;
class QueryWrapper
{
public:
    QueryWrapper() = default;
    ~QueryWrapper() = default;

    QueryWrapper& and_();
    QueryWrapper& or_();
    QueryWrapper& operator|(const QueryWrapper& other);
    QueryWrapper& operator&(const QueryWrapper& other);
    QueryWrapper& addCondition(const ColumnInfo& column, QueryCondition condition, ValueType value = std::monostate());
    QueryWrapper& addCondition(const QueryWrapper& other);
    QueryWrapper& popCondition(const std::string& colName);
    QueryWrapper& popCondition(const ColumnInfo& colInfo);

    bool contain(const std::string& colName) const;
    bool contain(const ColumnInfo& col) const;
    bool empty() const;
    size_t size() const;
    void reset();

    std::string prepareConditionString() const;
    std::string conditionString() const;
    int bind(SQLiteStatement& stament, int startIndex = 0) const;

    std::stringstream& prepareConditionString(std::stringstream& ss) const;
    std::stringstream& conditionString(std::stringstream& ss) const;

private:
    void mergeOptimization(PrepareInfo& newPrepareInfo);

private:
    ConditionLogicRelation m_logicRelation{ConditionLogicRelation::AND};
    std::vector<ConditionInfo> m_conditionInfos;
};

}  // namespace SQLite
