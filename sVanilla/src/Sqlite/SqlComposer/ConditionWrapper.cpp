#include <sstream>
#include <algorithm>

#include "ConditionWrapper.h"
#include "Sqlite/SQLiteLog.h"

namespace SQLite
{

std::string getQueryConditionName(QueryCondition condition)
{
    switch (condition)
    {
    case QueryCondition::EQUALS:
        return "==";
    case QueryCondition::NOT_EQUALS:
        return "<>";
    case QueryCondition::LE:
        return "<=";
    case QueryCondition::GE:
        return ">=";
    case QueryCondition::LT:
        return "<";
    case QueryCondition::GT:
        return ">";
    case QueryCondition::VALUES_IN:
        return "IN";
    case QueryCondition::VALUES_NOT_IN:
        return "NOT IN";
    case QueryCondition::LIKE:
        return "LIKE";
    case QueryCondition::NOT_LIKE:
        return "NOT LIKE";
    case QueryCondition::IS_NOT_NULL:
        return "IS NOT";
    case QueryCondition::IS_NULL:
        return "IS";
    default:
        break;
    }
    return std::string();
}

std::string getConditionRelationName(ConditionLogicRelation relation)
{
    switch (relation)
    {
    case ConditionLogicRelation::AND:
        return "AND";
    case ConditionLogicRelation::OR:
        return "OR";
    default:
        break;
    }
    return std::string();
}

bool PrepareInfo::valueSetEqual(const std::vector<SqliteColumnValue>& lv, const std::vector<SqliteColumnValue>& rv)
{
    if (lv.size() != rv.size())
    {
        return false;
    }
    for (size_t i = 0; i < lv.size(); i++)
    {
        if (lv[i] != rv[i])
        {
            return false;
        }
    }
    return true;
}

bool PrepareInfo::operator==(const PrepareInfo& other) const
{
    return m_columnName == other.m_columnName
        && m_condition == other.m_condition
        && m_value == m_value;
}

bool PrepareInfo::like(const PrepareInfo& other) const
{
    if (m_columnName != other.m_columnName)
    {
        return false;
    }

    switch (m_condition)
    {
    case QueryCondition::LE:
    case QueryCondition::LT:
        return other.m_condition == QueryCondition::LE || other.m_condition == QueryCondition::LT;
    case QueryCondition::GE:
    case QueryCondition::GT:
        return other.m_condition == QueryCondition::GE || other.m_condition == QueryCondition::GT;
    default:
        return m_condition == other.m_condition;
    }
}

std::stringstream& PrepareInfo::prepareConditionString(std::stringstream& ss) const
{
    if (m_condition == QueryCondition::VALUES_IN || m_condition == QueryCondition::VALUES_NOT_IN)
    {
        size_t setSize = std::get<2>(m_value).size();
        if (setSize == 0)
        {
            SQLITE_LOG_WARN("parsePrepareInfo error: valueset's size is zero");
            return ss;
        }

        ss << " " << m_columnName << " ";
        ss << " " << getQueryConditionName(m_condition) << " ";
        ss << "(";
        for (size_t i = 0; i < setSize - 1; i++)
        {
            ss << "?,";
        }
        ss << "?)";
    }
    else
    {
        ss << " " << m_columnName << " ";
        ss << " " << getQueryConditionName(m_condition) << " ";
        ss << "?";
    }

    return ss;
}

int PrepareInfo::bind(SQLiteStatement& stmt, int startIndex) const
{
    if (m_condition == QueryCondition::VALUES_IN || m_condition == QueryCondition::VALUES_NOT_IN)
    {
        auto& valueSets = std::get<std::vector<SqliteColumnValue>>(m_value);
        std::for_each(valueSets.begin(), valueSets.end(), [&](const SqliteColumnValue& value) {
            SQLite::bind(stmt, startIndex++, value);
        });
    }
    else
    {
        if (m_value.index() == 1)
        {
            SQLite::bind(stmt, startIndex++, std::get<SqliteColumnValue>(m_value));
        }
    }

    return startIndex;
}

std::stringstream& PrepareInfo::conditionString(std::stringstream& ss) const
{
    if (m_condition == QueryCondition::VALUES_IN || m_condition == QueryCondition::VALUES_NOT_IN)
    {
        auto& valueSet = std::get<std::vector<SqliteColumnValue>>(m_value);
        size_t setSize = valueSet.size();
        if (setSize == 0)
        {
            SQLITE_LOG_WARN("parsePrepareInfo error: valueset's size is zero");
            return ss;
        }

        ss << " " << m_columnName << " ";
        ss << " " << getQueryConditionName(m_condition) << " ";
        ss << "(";
        for (size_t i = 0; i < setSize - 1; i++)
        {
            SQLite::conditionString(ss, valueSet.at(i));
            ss << ",";
        }
        SQLite::conditionString(ss, valueSet.at(setSize - 1));
        ss << ")";
    }
    else
    {
        ss << " " << m_columnName << " ";
        ss << " " << getQueryConditionName(m_condition) << " ";
        SQLite::conditionString(ss, std::get<SqliteColumnValue>(m_value));
    }

    return ss;
}

QueryWrapper& QueryWrapper::and_()
{
    if (m_logicRelation == ConditionLogicRelation::AND)
    {
        return *this;
    }

    auto oldCondition = *this;
    m_logicRelation = ConditionLogicRelation::AND;
    addCondition(oldCondition);
    return *this;
}

QueryWrapper& QueryWrapper::or_()
{
    if (m_logicRelation == ConditionLogicRelation::OR)
    {
        return *this;
    }

    auto oldCondition = *this;
    m_logicRelation = ConditionLogicRelation::OR;
    addCondition(oldCondition);
    return *this;
}
QueryWrapper& QueryWrapper::operator|(const QueryWrapper& other)
{
    return or_().addCondition(other);
}

QueryWrapper& QueryWrapper::operator&(const QueryWrapper& other)
{
    return and_().addCondition(other);
}

QueryWrapper& QueryWrapper::addCondition(const QueryWrapper& other)
{
    if (this == &other)
    {
        return *this;
    }

    if (other.m_logicRelation == m_logicRelation)
    {
        for (const auto& condition : other.m_conditionInfos)
        {
            if (condition.index() == 0)
            {
                auto prepareInfo = std::get<PrepareInfo>(condition);
                mergeOptimization(prepareInfo);
                m_conditionInfos.emplace_back(prepareInfo);
            }
            else
            {
                m_conditionInfos.emplace_back(condition);
            }
        }
    }
    else if (other.size() == 1)
    {
        auto& condition = other.m_conditionInfos.front();
        if (condition.index() == 0)
        {
            auto prepareInfo = std::get<PrepareInfo>(condition);
            mergeOptimization(prepareInfo);
            m_conditionInfos.emplace_back(prepareInfo);
        }
        else
        {
            m_conditionInfos.emplace_back(condition);
        }
    }
    else
    {
        m_conditionInfos.emplace_back(other);
    }
}

QueryWrapper& QueryWrapper::addCondition(const ColumnInfo& column, QueryCondition condition, ValueType value)
{
    if (condition == QueryCondition::LIKE || condition == QueryCondition::NOT_LIKE)
    {
        auto& strValue = std::get<std::string>(std::get<SqliteColumnValue>(value));
        strValue += "%";
    }

    PrepareInfo prepareInfo = { column.m_columnName, condition, value };
    mergeOptimization(prepareInfo);
    m_conditionInfos.emplace_back(prepareInfo);

    return *this;
}

QueryWrapper& QueryWrapper::popCondition(const std::string& colName)
{
    bool ret = false;
    for (auto it = m_conditionInfos.begin(); it != m_conditionInfos.end(); ++it)
    {
        if (it->index() == 0)
        {
            auto& prepareInfo = std::get<PrepareInfo>(*it);
            if (prepareInfo.m_columnName == colName)
            {
                it = m_conditionInfos.erase(it);
                it--;
            }
        }
        else
        {
            auto& queryWrapper = std::get<QueryWrapper>(*it);
            queryWrapper.popCondition(colName);
        }
    }
    return *this;
}

QueryWrapper& QueryWrapper::popCondition(const ColumnInfo& colInfo)
{
    return popCondition(colInfo.m_columnName);
}

bool QueryWrapper::contain(const std::string& colName) const
{
    bool ret = false;
    for (const auto& conditionInfo : m_conditionInfos)
    {
        if (conditionInfo.index() == 0)
        {
            auto& prepareInfo = std::get<PrepareInfo>(conditionInfo);
            if (prepareInfo.m_columnName == colName)
            {
                return true;
            }
        }
        else
        {
            auto& queryWrapper = std::get<QueryWrapper>(conditionInfo);
            if (queryWrapper.contain(colName))
            {
                return true;
            }
        }
    }
    return false;
}

bool QueryWrapper::contain(const ColumnInfo& col) const
{
    return contain(col.m_columnName);
}

bool QueryWrapper::empty() const
{
    return m_conditionInfos.empty();
}

size_t QueryWrapper::size() const
{
    return m_conditionInfos.size();
}

void QueryWrapper::reset()
{
    m_logicRelation = ConditionLogicRelation::AND;
    m_conditionInfos.clear();
}

std::string QueryWrapper::prepareConditionString() const
{
    std::stringstream ss;
    prepareConditionString(ss);
    std::string str = ss.str();
    if (!str.empty())
    {
        str = " WHERE " + str;
    }
    return str;
}

std::string QueryWrapper::conditionString() const
{
    std::stringstream ss;
    conditionString(ss);
    std::string str = ss.str();
    if (!str.empty())
    {
        str = " WHERE " + str;
    }
    return str;
}

int QueryWrapper::bind(SQLiteStatement& stmt, int startIndex) const
{
    for (const auto& condition : m_conditionInfos)
    {
        std::visit([&](auto&& value) {
            value.bind(stmt, startIndex);
        }, condition);
    }
    return startIndex;
}

std::stringstream& QueryWrapper::prepareConditionString(std::stringstream& ss) const
{
    if (m_conditionInfos.empty())
    {
        return ss;
    }

    if (m_conditionInfos.size() == 1)
    {
        std::visit([&ss](auto&& value) {
            value.prepareConditionString(ss);
        }, m_conditionInfos.at(0));
    }
    else
    {
        ss << "(";
        std::visit([&ss](auto&& value) {
            value.prepareConditionString(ss);
        }, m_conditionInfos.at(0));
        size_t size = m_conditionInfos.size();
        for (size_t i = 1; i < size; ++i)
        {
            ss << " " << getConditionRelationName(m_logicRelation) << " ";
            std::visit([&ss](auto&& value) {
                value.prepareConditionString(ss);
            }, m_conditionInfos.at(i));
        }
        ss << ")";
    }

    return ss;
}

std::stringstream& QueryWrapper::conditionString(std::stringstream& ss) const
{
    if (m_conditionInfos.empty())
    {
        return ss;
    }

    if (m_conditionInfos.size() == 1)
    {
        std::visit([&ss](auto&& value) {
                value.conditionString(ss);
        }, m_conditionInfos.at(0));
    }
    else
    {
        ss << "(";
        std::visit([&ss](auto&& value) {
            value.conditionString(ss);
        }, m_conditionInfos.at(0));
        size_t size = m_conditionInfos.size();
        for (size_t i = 1; i < size; ++i)
        {
            ss << " " << getConditionRelationName(m_logicRelation) << " ";
            std::visit([&ss](auto&& value) {
                value.prepareConditionString(ss);
            }, m_conditionInfos.at(i));
        }
        ss << ")";
    }

    return ss;
}

void SQLite::QueryWrapper::mergeOptimization(PrepareInfo& newPrepareInfo)
{
    for (auto it = m_conditionInfos.begin(); it != m_conditionInfos.end(); ++it)
    {
        if (it->index() == 1)
        {
            continue;
        }

        bool canMerge = false;
        auto& prepareInfo = std::get<PrepareInfo>(*it);
        if (newPrepareInfo.like(prepareInfo))
        {
            switch (newPrepareInfo.m_condition)
            {
            case QueryCondition::EQUALS:
            case QueryCondition::NOT_EQUALS:
            {
                if (newPrepareInfo.m_value == prepareInfo.m_value)
                {
                    canMerge = true;
                }
                break;
            }
            case QueryCondition::LE:
            case QueryCondition::LT:
            {
                if (newPrepareInfo.m_value > prepareInfo.m_value && m_logicRelation == ConditionLogicRelation::AND)
                {
                    newPrepareInfo = std::move(prepareInfo);
                    canMerge = true;
                }
                else if (newPrepareInfo.m_value < prepareInfo.m_value && m_logicRelation == ConditionLogicRelation::OR)
                {
                    newPrepareInfo = std::move(prepareInfo);
                    canMerge = true;
                }
                else if (newPrepareInfo.m_value == prepareInfo.m_value && m_logicRelation == ConditionLogicRelation::AND)
                {
                    newPrepareInfo.m_condition = std::max(newPrepareInfo.m_condition, prepareInfo.m_condition);
                    canMerge = true;
                }
                else if (newPrepareInfo.m_value == prepareInfo.m_value && m_logicRelation == ConditionLogicRelation::OR)
                {
                    newPrepareInfo.m_condition = std::min(newPrepareInfo.m_condition, prepareInfo.m_condition);
                    canMerge = true;
                }
                break;
            }
            case QueryCondition::GE:
            case QueryCondition::GT:
            {
                if (newPrepareInfo.m_value < prepareInfo.m_value && m_logicRelation == ConditionLogicRelation::AND)
                {
                    newPrepareInfo = std::move(prepareInfo);
                    canMerge = true;
                }
                else if (newPrepareInfo.m_value  > prepareInfo.m_value && m_logicRelation == ConditionLogicRelation::OR)
                {
                    newPrepareInfo = std::move(prepareInfo);
                    canMerge = true;
                }
                else if (newPrepareInfo.m_value > prepareInfo.m_value && m_logicRelation == ConditionLogicRelation::AND)
                {
                    newPrepareInfo.m_condition = std::max(newPrepareInfo.m_condition, prepareInfo.m_condition);
                    canMerge = true;
                }
                else if (newPrepareInfo.m_value > prepareInfo.m_value && m_logicRelation == ConditionLogicRelation::OR)
                {
                    newPrepareInfo.m_condition = std::min(newPrepareInfo.m_condition, prepareInfo.m_condition);
                    canMerge = true;
                }
                break;
            }
            case QueryCondition::VALUES_IN:
            case QueryCondition::VALUES_NOT_IN:
            case QueryCondition::LIKE:
            case QueryCondition::NOT_LIKE:
            case QueryCondition::IS_NOT_NULL:
            default:
                break;
            }
        }

        if (canMerge)
        {
            it = m_conditionInfos.erase(it);
            it--;
        }
    }
}

}  // namespace SQLite
