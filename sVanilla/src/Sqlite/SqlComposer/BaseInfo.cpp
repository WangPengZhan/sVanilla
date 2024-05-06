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

void test()
{
    FinishedItem testItem;
    ColumnInfo columnInfo =
        ColumnInfo(std::string(""), &FinishedItem::uniqueId, (std::vector<ColumnInfo*>*)nullptr, (std::vector<ColumnInfo*>*)nullptr, false, false, false);
    auto memberPtr = columnInfo.memberPtr<FinishedItem, std::string>();
    testItem.*(*memberPtr) = std::string("test");
    auto value = testItem.*(*memberPtr);
}

ColumnInfo::ColumnInfo(std::string columnName, bool autoIncremnet, bool unique)
    : m_columnName(std::move(columnName))
    , m_autoIncrement(autoIncremnet)
    , m_unique(unique)
{
}

ColumnInfo::ColumnInfo(std::string columnName, std::vector<ColumnInfo*>* pColumnInfos, std::vector<ColumnInfo*>* pPrimaryKeyColumnInfos, bool autoIncremnet,
                       bool unique)
    : m_columnName(std::move(columnName))
    , m_autoIncrement(autoIncremnet)
    , m_unique(unique)
    , m_pColumnInfos(pColumnInfos)
{
    if (pColumnInfos)
    {
        pColumnInfos->push_back(this);
    }

    if (pPrimaryKeyColumnInfos && autoIncremnet)
    {
        pPrimaryKeyColumnInfos->push_back(this);
    }
}

int ColumnInfo::valueTypeIndex() const
{
    return m_valueTypeIndex;
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

const std::vector<ColumnInfo*>& BaseTableStructInfo::columnInfos() const
{
    return m_columnInfos;
}

const std::vector<ColumnInfo*>& BaseTableStructInfo::primaryColumnInfos() const
{
    return m_primaryColunmInfos;
}

std::string BaseTableStructInfo::createSql() const
{
    if (m_createSql.empty())
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

        if (!m_primaryColunmInfos.empty())
        {
            for (auto it = m_primaryColunmInfos.begin(); it != m_primaryColunmInfos.end(); ++it)
            {
                ss << "PRIMARY KEY"
                   << "(";
                if (it != m_primaryColunmInfos.begin())
                {
                    ss << ", ";
                    ss << (*it)->colunmName();
                }
                else
                {
                    ss << (*it)->colunmName();
                }
                ss << ")";
            }
        }

        m_createSql = ss.str();
    }

    return m_createSql;
}

std::string BaseTableStructInfo::prepareSql() const
{
    if (m_prepareSql.empty())
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
        m_prepareSql = ss.str();
    }

    return m_prepareSql;
}

std::string BaseTableStructInfo::insertPrepareSql() const
{
    if (m_insertPrepareSql.empty())
    {
        std::stringstream ss;
        ss << "(" << colunmsSql() << ")";
        ss << " VALUES ";
        ss << "(" << prepareSql() << ")";

        m_insertPrepareSql = ss.str();
    }

    return m_insertPrepareSql;
}

std::string BaseTableStructInfo::colunmsSql() const
{
    if (m_colunmsSql.empty())
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

        m_colunmsSql = ss.str();
    }

    return m_colunmsSql;
}

std::string BaseTableStructInfo::updatePrepareSql() const
{
    if (m_updatePrepareSql.empty())
    {
        std::stringstream ss;
        for (auto it = m_columnInfos.begin(); it != m_columnInfos.end(); ++it)
        {
            if (it != m_columnInfos.begin())
            {
                ss << ", ";
                ss << (*it)->colunmName();
                ss << " = ?";
            }
            else
            {
                ss << (*it)->colunmName();
                ss << " = ?";
            }
        }

        m_updatePrepareSql = ss.str();
    }

    return m_updatePrepareSql;
}

std::string BaseTableStructInfo::primaryKeyPrepareSql() const
{
    if (m_primaryKeyPrepareSql.empty())
    {
        std::stringstream ss;
        for (auto it = m_primaryColunmInfos.begin(); it != m_primaryColunmInfos.end(); ++it)
        {
            if (it != m_primaryColunmInfos.begin())
            {
                ss << ", ";
                ss << (*it)->colunmName();
                ss << " = ?";
            }
            else
            {
                ss << (*it)->colunmName();
                ss << " = ?";
            }
        }

        m_primaryKeyPrepareSql = ss.str();
    }

    return m_primaryKeyPrepareSql;
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
