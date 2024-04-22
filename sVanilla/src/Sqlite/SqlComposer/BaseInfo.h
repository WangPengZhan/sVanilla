#pragma once
#include <string>
#include <vector>
#include <type_traits>

#include "Sqlite/Database/SQLiteColunm.h"

// clang-format off
#define ISTR(x)                                                           #x
#define STR(x)                                                            ISTR(x)

#define ICAT(a, b)                                                        a##b
#define CAT(a, b)                                                         ICAT(a, b)

#define EXPAND(...)                                                       __VA_ARGS__

#define HELPER_OF_MACRO_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#define MACRO_COUNT(...)                                                  EXPAND(HELPER_OF_MACRO_COUNT(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1))
// clang-format on

namespace sqlite
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

    ColumnInfo(std::string columnName, std::vector<ColumnInfo*>* pColumnInfos, bool autoIncremnet = false, bool unique = false)
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

    template <typename ValueType>
    ColumnInfo(std::string columnName, const ValueType& value, std::vector<ColumnInfo*>* pColumnInfos, bool autoIncremnet = false, bool unique = false)
        : m_columnName(std::move(columnName))
        , m_autoIncrement(autoIncremnet)
        , m_unique(unique)
        , m_pColumnInfos(pColumnInfos)
    {
        setFieldType(value);
        if (pColumnInfos)
        {
            pColumnInfos->push_back(this);
        }
    }

    template <typename ValueType>
    void setFieldType(const ValueType& value)
    {
        if constexpr (std::is_integral_v<ValueType>)
        {
            m_fieldType = FieldType::Integer;
        }
        else if constexpr (std::is_floating_point_v<ValueType>)
        {
            m_fieldType = FieldType::Float;
        }
        else if constexpr (std::is_same_v<std::vector<uint8_t>, ValueType> || std::is_same_v<const void*, ValueType>)
        {
            m_fieldType = FieldType::Blob;
        }
        else
        {
            m_fieldType = FieldType::Text;
        }
    }

    template <typename... ValueType>
    void setFieldType(const std::variant<ValueType...>& value)
    {
        std::visit(value, [](auto&& elem) {
            setFieldType(elem);
        });
    }

public:
    std::string m_columnName;
    FieldType m_fieldType = FieldType::Integer;
    bool m_autoIncrement = false;
    bool m_unique = false;
    std::vector<ColumnInfo*>* m_pColumnInfos = nullptr;
};

class BaseTableStructInfo
{
public:
    size_t colunmCount()
    {
        return m_columnInfos.size();
    }

    ColumnInfo& columnInfo(int index)
    {
        return *m_columnInfos.at(index);
    }

    void addColumn(ColumnInfo* col)
    {
        m_columnInfos.push_back(col);
    }

    void addSubTable(BaseTableStructInfo& subTable)
    {
        for (const auto& col : subTable.m_columnInfos)
        {
            m_columnInfos.push_back(col);
        }
    }

protected:
    std::vector<ColumnInfo*> m_columnInfos;
};

template <typename Table>
class TableStructInfo;

struct FinishedItem
{
    std::string uniqueId;  // UniqueId Text
};

struct FinishedItems
{
    std::string uniqueId;  // UniqueId Text
    std::string filePath;  //
    std::string bvid;      //
    std::string title;
    int duration;
};

struct FinishedItemNosames
{
    std::string uniqueId;  // UniqueId Text
    std::string filePath;  //
    std::string bvid;      //
    std::string title;
    int duration;
};

template <>
class TableStructInfo<FinishedItem> : public BaseTableStructInfo
{
public:
    ColumnInfo m_uniqueId = ColumnInfo(std::string(""), decltype(std::declval<FinishedItem>().uniqueId)(), &m_columnInfos);

public:
    static const TableStructInfo<FinishedItem>& self()
    {
        static const TableStructInfo<FinishedItem> intance;
        return intance;
    }

    TableStructInfo(const TableStructInfo&) = delete;
    TableStructInfo& operator=(const TableStructInfo&) = delete;

protected:
    TableStructInfo() = default;
    ~TableStructInfo() = default;
};

}  // namespace sqlite

// clang-format off
#define TABLESTRUCTINFO_BEGIN(type) \
namespace sqlite \
{ \
template<> \
class TableStructInfo<type> : public BaseTableStructInfo \
{ \
public:\
    using StructType = type;\
    static const TableStructInfo<StructType>& self()\
    {\
        static const TableStructInfo<StructType> intance;\
        return intance;\
    }\
    TableStructInfo(const TableStructInfo&) = delete;\
    TableStructInfo& operator=(const TableStructInfo&) = delete;\
protected:\
    TableStructInfo() = default;\
    ~TableStructInfo() = default;\
public:

#define TABLESTRUCTINFO_COMLUNM_2(type, colunmName) \
    ColumnInfo type = ColumnInfo(STR(colunmName), decltype(std::declval<StructType>().type)(), &m_columnInfos);

#define TABLESTRUCTINFO_COMLUNM_1(type) TABLESTRUCTINFO_COMLUNM_2(type, type)

#define TABLESTRUCTINFO_COMLUNM(...) EXPAND(CAT(TABLESTRUCTINFO_COMLUNM_, MACRO_COUNT(__VA_ARGS__))(__VA_ARGS__))

#define TABLESTRUCTINFO_END(type) \
};\
} // namespace SQLite
// clang-format on

// clang-format off
TABLESTRUCTINFO_BEGIN(sqlite::FinishedItems)
    TABLESTRUCTINFO_COMLUNM(uniqueId)
    TABLESTRUCTINFO_COMLUNM(filePath)
    TABLESTRUCTINFO_COMLUNM(bvid)
    TABLESTRUCTINFO_COMLUNM(title)
    TABLESTRUCTINFO_COMLUNM(duration)
TABLESTRUCTINFO_END(sqlite::FinishedItems)

TABLESTRUCTINFO_BEGIN(sqlite::FinishedItemNosames)
    TABLESTRUCTINFO_COMLUNM(uniqueId, uniqueId1)
    TABLESTRUCTINFO_COMLUNM(filePath, filePath1)
    TABLESTRUCTINFO_COMLUNM(bvid, bvid1)
    TABLESTRUCTINFO_COMLUNM(title, title1)
    TABLESTRUCTINFO_COMLUNM(duration, duration1)
TABLESTRUCTINFO_END(sqlite::FinishedItemNosames)

auto& self2 =  sqlite::TableStructInfo<sqlite::FinishedItem>::self();
auto& self = sqlite::TableStructInfo<sqlite::FinishedItems>::self();
auto& self1 = sqlite::TableStructInfo<sqlite::FinishedItemNosames>::self();
// clang-format on
