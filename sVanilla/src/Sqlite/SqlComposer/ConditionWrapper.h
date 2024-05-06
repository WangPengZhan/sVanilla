#pragma once
#include <variant>
#include <vector>
#include <string>
#include <unordered_set>

#include "BaseInfo.h"
#include "Sqlite/Database/SQLiteColunm.h"
#include "Sqlite/Database/SQLiteStatement.h"

namespace sqlite
{
class SQLiteStatement;
enum class Condition
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

static std::string getQueryConditionName(Condition condition);
static std::string getConditionRelationName(ConditionLogicRelation relation);

using ValueType = std::variant<std::monostate, SqliteColumnValue, std::vector<SqliteColumnValue>>;
struct PrepareInfo
{
    std::string m_columnName;
    Condition m_condition;
    ValueType m_value;

    static bool valueSetEqual(const std::vector<SqliteColumnValue>& lv, const std::vector<SqliteColumnValue>& rv);
    bool operator==(const PrepareInfo& other) const;

    bool like(const PrepareInfo& other) const;

    std::stringstream& prepareConditionString(std::stringstream& ss) const;
    int bind(SQLiteStatement& stmt, int startIndex = 1) const;
    std::stringstream& conditionString(std::stringstream& ss) const;
};

class ConditionWrapper;
using ConditionInfo = std::variant<PrepareInfo, ConditionWrapper>;
class ConditionWrapper
{
public:
    ConditionWrapper() = default;
    ~ConditionWrapper() = default;

    ConditionWrapper& and_();
    ConditionWrapper& or_();
    ConditionWrapper& operator|(const ConditionWrapper& other);
    ConditionWrapper& operator&(const ConditionWrapper& other);
    ConditionWrapper& addCondition(const ColumnInfo& column, Condition condition, ValueType value = std::monostate());
    ConditionWrapper& addCondition(const ConditionWrapper& other);
    ConditionWrapper& popCondition(const std::string& colName);
    ConditionWrapper& popCondition(const ColumnInfo& colInfo);

    bool contain(const std::string& colName) const;
    bool contain(const ColumnInfo& col) const;
    std::unordered_set<std::string> colunmNames() const;

    bool empty() const;
    size_t size() const;
    void reset();

    std::string prepareConditionString() const;
    std::string conditionString() const;
    int bind(SQLiteStatement& stament, int startIndex = 1) const;

    std::stringstream& prepareConditionString(std::stringstream& ss) const;
    std::stringstream& conditionString(std::stringstream& ss) const;

private:
    void mergeOptimization(PrepareInfo& newPrepareInfo);

private:
    ConditionLogicRelation m_logicRelation{ConditionLogicRelation::AND};
    std::vector<ConditionInfo> m_conditionInfos;
};

}  // namespace sqlite
