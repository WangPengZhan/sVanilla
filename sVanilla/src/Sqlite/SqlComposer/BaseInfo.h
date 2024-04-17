#pragma once
#include <string>
#include <vector>

namespace SQLite
{

struct ColumnInfo
{
public:
    ColumnInfo(std::string columnName, bool autoIncremnet = false, bool unique = false)
        : m_columnName(std::move(columnName))
        , m_autoIncrement(autoIncremnet)
        , m_unique(unique)
    {
    }

public:
    std::string m_columnName;
    bool m_autoIncrement = false;
    bool m_unique = false;
};

template<typename Entity>
class TableStructInfo
{
public:
    static size_t colunmCount()
    {
        return m_columnInfos.size();
    }

    static ColumnInfo& columnInfo(int index)
    {
        return *m_columnInfos.at(index);
    }

    static void addColumn(ColumnInfo* col)
    {
        m_columnInfos.push_back(col);
    }

    template <typename OEntity>
    static void addSubTable(TableStructInfo<Entity>& subTable)
    {
        for (const auto& col : subTable.m_columnInfos)
        {
            m_columnInfos.push_back(col);
        }
    }

protected:
    static std::vector<ColumnInfo*> m_columnInfos;
};

template <typename Entity>
std::vector<ColumnInfo*> TableStructInfo<Entity>::m_columnInfos;

} // namespace SQLite
