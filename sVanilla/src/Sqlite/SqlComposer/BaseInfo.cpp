#include "BaseInfo.h"

namespace sqlite
{
std::string fieldTypeSql(FieldType fieldType)
{
    std::string res;
    switch (fieldType)
    {
    case sqlite::FieldType::Integer:
        res = "INTEGER";
        break;
    case sqlite::FieldType::Float:
        res = "REAL";
        break;
    case sqlite::FieldType::Text:
        res = "TEXT";
        break;
    case sqlite::FieldType::Blob:
        res = "BLOB";
        break;
    case sqlite::FieldType::Null:
    default:
        res = "TEXT";
        break;
    }

    return res;
}

ColumnInfo::ColumnInfo(std::string columnName, bool autoIncremnet, bool unique)
    : m_columnName(std::move(columnName))
    , m_autoIncrement(autoIncremnet)
    , m_unique(unique)
{
}

ColumnInfo::ColumnInfo(std::string columnName, std::vector<ColumnInfo*>* pColumnInfos, bool autoIncremnet, bool unique)
    : m_columnName(std::move(columnName))
    , m_autoIncrement(autoIncremnet)
    , m_unique(unique)
    , m_pColumnInfos(pColumnInfos)
{
    if (pColumnInfos)
    {
        pColumnInfos->push_back(this);
    }
}

std::string ColumnInfo::colunmSql() const
{
    std::stringstream ss;
    ss << m_columnName << " " << fieldTypeSql(m_fieldType);
    if (m_autoIncrement)
    {
        ss << " PRIMARY KEY AUTOINCREMENT";
    }
    else if (m_unique)
    {
        ss << " UNIQUE";
    }

    return ss.str();
}

const std::string& ColumnInfo::colunmName() const
{
    return m_columnName;
}

size_t BaseTableStructInfo::colunmCount() const
{
    return m_columnInfos.size();
}

ColumnInfo& BaseTableStructInfo::columnInfo(int index) const
{
    return *m_columnInfos.at(index);
}

void BaseTableStructInfo::addColumn(ColumnInfo* col)
{
    m_columnInfos.push_back(col);
}

void BaseTableStructInfo::addSubTable(BaseTableStructInfo& subTable)
{
    for (const auto& col : subTable.m_columnInfos)
    {
        m_columnInfos.push_back(col);
    }
}

std::string BaseTableStructInfo::createSql() const
{
    std::stringstream ss;
    for (auto it = m_columnInfos.begin(); it != m_columnInfos.end(); ++it)
    {
        if (it != m_columnInfos.begin())
        {
            ss << ", ";
            ss << (*it)->colunmSql();
        }
        else
        {
            ss << (*it)->colunmSql();
        }
    }

    return ss.str();
}

std::string BaseTableStructInfo::prepareSql() const
{
    std::stringstream ss;
    for (auto it = m_columnInfos.begin(); it != m_columnInfos.end(); ++it)
    {
        if (it != m_columnInfos.begin())
        {
            ss << ", ?";
        }
        else
        {
            ss << "?";
        }
    }

    return ss.str();
}

std::string BaseTableStructInfo::insertPrepareSql() const
{
    std::stringstream ss;
    ss << "(" << colunmsSql() << ")";
    ss << " VALUES ";
    ss << "(" << prepareSql() << ")";

    return ss.str();
}

std::string BaseTableStructInfo::colunmsSql() const
{
    std::stringstream ss;
    for (auto it = m_columnInfos.begin(); it != m_columnInfos.end(); ++it)
    {
        if (it != m_columnInfos.begin())
        {
            ss << ", ";
            ss << (*it)->colunmName();
        }
        else
        {
            ss << (*it)->colunmName();
        }
    }

    return ss.str();
}

IndexColInfo::IndexColInfo(std::string colName, std::vector<IndexAttribute> attr)
    : m_colName(colName)
    , m_attributes(attr)
{
}

std::string IndexColInfo::indexSql() const
{
    std::stringstream ss;
    ss << m_colName;
    for (auto attribute : m_attributes)
    {
        ss << " ";
        switch (attribute)
        {
        case IndexAttribute::ASC:
            ss << "ASC";
            break;
        case IndexAttribute::DESC:
            ss << "DESC";
            break;
        case IndexAttribute::NO_CASE:
            ss << "COLLATE NOCASE";
            break;
        case IndexAttribute::EMPTY:
        default:
            break;
        }
    }

    return ss.str();
}

const std::string& IndexColInfo::colunmName() const
{
    return m_colName;
}

}  // namespace sqlite
