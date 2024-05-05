#include <sstream>
#include <algorithm>

#include "ConditionWrapper.h"
#include "Sqlite/SQLiteLog.h"

namespace sqlite
{

std::string getQueryConditionName(Condition condition)
{
    switch (condition)
    {
    case Condition::EQUALS:
        return "==";
    case Condition::NOT_EQUALS:
        return "<>";
    case Condition::LE:
        return "<=";
    case Condition::GE:
        return ">=";
    case Condition::LT:
        return "<";
    case Condition::GT:
        return ">";
    case Condition::VALUES_IN:
        return "IN";
    case Condition::VALUES_NOT_IN:
        return "NOT IN";
    case Condition::LIKE:
        return "LIKE";
    case Condition::NOT_LIKE:
        return "NOT LIKE";
    case Condition::IS_NOT_NULL:
        return "IS NOT";
    case Condition::IS_NULL:
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
    return m_columnName == other.m_columnName && m_condition == other.m_condition && m_value == m_value;
}

bool PrepareInfo::like(const PrepareInfo& other) const
{
    if (m_columnName != other.m_columnName)
    {
        return false;
    }

    switch (m_condition)
    {
    case Condition::LE:
    case Condition::LT:
        return other.m_condition == Condition::LE || other.m_condition == Condition::LT;
    case Condition::GE:
    case Condition::GT:
        return other.m_condition == Condition::GE || other.m_condition == Condition::GT;
    default:
        return m_condition == other.m_condition;
    }
}

std::stringstream& PrepareInfo::prepareConditionString(std::stringstream& ss) const
{
    if (m_condition == Condition::VALUES_IN || m_condition == Condition::VALUES_NOT_IN)
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
    if (m_condition == Condition::VALUES_IN || m_condition == Condition::VALUES_NOT_IN)
    {
        auto& valueSets = std::get<std::vector<SqliteColumnValue>>(m_value);
        std::for_each(valueSets.begin(), valueSets.end(), [&](const SqliteColumnValue& value) {
            sqlite::bind(stmt, startIndex++, value);
        });
    }
    else
    {
        if (m_value.index() == 1)
        {
            sqlite::bind(stmt, startIndex++, std::get<SqliteColumnValue>(m_value));
        }
    }

    return startIndex;
}

std::stringstream& PrepareInfo::conditionString(std::stringstream& ss) const
{
    if (m_condition == Condition::VALUES_IN || m_condition == Condition::VALUES_NOT_IN)
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
            sqlite::conditionString(ss, valueSet.at(i));
            ss << ",";
        }
        sqlite::conditionString(ss, valueSet.at(setSize - 1));
        ss << ")";
    }
    else
    {
        ss << " " << m_columnName << " ";
        ss << " " << getQueryConditionName(m_condition) << " ";
        sqlite::conditionString(ss, std::get<SqliteColumnValue>(m_value));
    }

    return ss;
}

ConditionWrapper& ConditionWrapper::and_()
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

ConditionWrapper& ConditionWrapper::or_()
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
ConditionWrapper& ConditionWrapper::operator|(const ConditionWrapper& other)
{
    return or_().addCondition(other);
}

ConditionWrapper& ConditionWrapper::operator&(const ConditionWrapper& other)
{
    return and_().addCondition(other);
}

ConditionWrapper& ConditionWrapper::addCondition(const ConditionWrapper& other)
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

    return *this;
}

ConditionWrapper& ConditionWrapper::addCondition(const ColumnInfo& column, Condition condition, ValueType value)
{
    if (condition == Condition::LIKE || condition == Condition::NOT_LIKE)
    {
        auto& strValue = std::get<std::string>(std::get<SqliteColumnValue>(value));
        strValue += "%";
    }

    PrepareInfo prepareInfo = {column.colunmName(), condition, value};
    mergeOptimization(prepareInfo);
    m_conditionInfos.emplace_back(prepareInfo);

    return *this;
}

ConditionWrapper& ConditionWrapper::popCondition(const std::string& colName)
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
            auto& queryWrapper = std::get<ConditionWrapper>(*it);
            queryWrapper.popCondition(colName);
        }
    }
    return *this;
}

ConditionWrapper& ConditionWrapper::popCondition(const ColumnInfo& colInfo)
{
    return popCondition(colInfo.colunmName());
}

bool ConditionWrapper::contain(const std::string& colName) const
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
            auto& queryWrapper = std::get<ConditionWrapper>(conditionInfo);
            if (queryWrapper.contain(colName))
            {
                return true;
            }
        }
    }
    return false;
}

bool ConditionWrapper::contain(const ColumnInfo& col) const
{
    return contain(col.colunmName());
}

std::unordered_set<std::string> ConditionWrapper::colunmNames() const
{
    return std::unordered_set<std::string>();
}

bool ConditionWrapper::empty() const
{
    return m_conditionInfos.empty();
}

size_t ConditionWrapper::size() const
{
    return m_conditionInfos.size();
}

void ConditionWrapper::reset()
{
    m_logicRelation = ConditionLogicRelation::AND;
    m_conditionInfos.clear();
}

std::string ConditionWrapper::prepareConditionString() const
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

std::string ConditionWrapper::conditionString() const
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

int ConditionWrapper::bind(SQLiteStatement& stmt, int startIndex) const
{
    for (const auto& condition : m_conditionInfos)
    {
        std::visit(
            [&](auto&& value) {
                startIndex = value.bind(stmt, startIndex);
            },
            condition);
    }
    return startIndex;
}

std::stringstream& ConditionWrapper::prepareConditionString(std::stringstream& ss) const
{
    if (m_conditionInfos.empty())
    {
        return ss;
    }

    if (m_conditionInfos.size() == 1)
    {
        std::visit(
            [&ss](auto&& value) {
                value.prepareConditionString(ss);
            },
            m_conditionInfos.at(0));
    }
    else
    {
        ss << "(";
        std::visit(
            [&ss](auto&& value) {
                value.prepareConditionString(ss);
            },
            m_conditionInfos.at(0));
        size_t size = m_conditionInfos.size();
        for (size_t i = 1; i < size; ++i)
        {
            ss << " " << getConditionRelationName(m_logicRelation) << " ";
            std::visit(
                [&ss](auto&& value) {
                    value.prepareConditionString(ss);
                },
                m_conditionInfos.at(i));
        }
        ss << ")";
    }

    return ss;
}

std::stringstream& ConditionWrapper::conditionString(std::stringstream& ss) const
{
    if (m_conditionInfos.empty())
    {
        return ss;
    }

    if (m_conditionInfos.size() == 1)
    {
        std::visit(
            [&ss](auto&& value) {
                value.conditionString(ss);
            },
            m_conditionInfos.at(0));
    }
    else
    {
        ss << "(";
        std::visit(
            [&ss](auto&& value) {
                value.conditionString(ss);
            },
            m_conditionInfos.at(0));
        size_t size = m_conditionInfos.size();
        for (size_t i = 1; i < size; ++i)
        {
            ss << " " << getConditionRelationName(m_logicRelation) << " ";
            std::visit(
                [&ss](auto&& value) {
                    value.conditionString(ss);
                },
                m_conditionInfos.at(i));
        }
        ss << ")";
    }

    return ss;
}

void sqlite::ConditionWrapper::mergeOptimization(PrepareInfo& newPrepareInfo)
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
            case Condition::EQUALS:
            case Condition::NOT_EQUALS:
            {
                if (newPrepareInfo.m_value == prepareInfo.m_value)
                {
                    canMerge = true;
                }
                break;
            }
            case Condition::LE:
            case Condition::LT:
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
            case Condition::GE:
            case Condition::GT:
            {
                if (newPrepareInfo.m_value < prepareInfo.m_value && m_logicRelation == ConditionLogicRelation::AND)
                {
                    newPrepareInfo = std::move(prepareInfo);
                    canMerge = true;
                }
                else if (newPrepareInfo.m_value > prepareInfo.m_value && m_logicRelation == ConditionLogicRelation::OR)
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
            case Condition::VALUES_IN:
            case Condition::VALUES_NOT_IN:
            case Condition::LIKE:
            case Condition::NOT_LIKE:
            case Condition::IS_NOT_NULL:
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

}  // namespace sqlite
