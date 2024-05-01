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

std::string fieldTypeSql(FieldType fieldType);

class ColumnInfo
{
public:
    ColumnInfo(std::string columnName, bool autoIncremnet = false, bool unique = false);
    ColumnInfo(std::string columnName, std::vector<ColumnInfo*>* pColumnInfos, std::vector<ColumnInfo*>* pPrimaryKeyColumnInfos, bool autoIncremnet = false,
               bool unique = false);

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
    ColumnInfo(std::string columnName, const ValueType& value, std::vector<ColumnInfo*>* pColumnInfos, std::vector<ColumnInfo*>* pPrimaryKeyColumnInfos,
               bool autoIncremnet, bool unique, bool primaryKey)
        : m_columnName(std::move(columnName))
        , m_autoIncrement(autoIncremnet)
        , m_unique(unique)
        , m_pColumnInfos(pColumnInfos)
        , m_pPrimaryKeyColumnInfos(pPrimaryKeyColumnInfos)
    {
        setFieldType(value);
        if (pColumnInfos)
        {
            pColumnInfos->push_back(this);
        }

        if (pPrimaryKeyColumnInfos && (autoIncremnet || primaryKey))
        {
            pPrimaryKeyColumnInfos->push_back(this);
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
        std::visit(value, [this](auto&& elem) {
            setFieldType(elem);
        });
    }

    std::string colunmSql() const;
    const std::string& colunmName() const;

private:
    std::string m_columnName;
    FieldType m_fieldType = FieldType::Integer;
    bool m_autoIncrement = false;
    bool m_unique = false;
    std::vector<ColumnInfo*>* m_pColumnInfos = nullptr;
    std::vector<ColumnInfo*>* m_pPrimaryKeyColumnInfos = nullptr;
};

class BaseTableStructInfo
{
public:
    size_t colunmCount() const;
    ColumnInfo& columnInfo(int index) const;

    void addColumn(ColumnInfo* col);
    void addSubTable(BaseTableStructInfo& subTable);

    std::string createSql() const;
    std::string prepareSql() const;
    std::string insertPrepareSql() const;
    std::string colunmsSql() const;
    std::string updatePrepareSql() const;
    std::string primaryKeyPrepareSql() const;

protected:
    std::vector<ColumnInfo*> m_columnInfos;
    std::vector<ColumnInfo*> m_primaryColunmInfos;

    mutable std::string m_createSql;
    mutable std::string m_prepareSql;
    mutable std::string m_insertPrepareSql;
    mutable std::string m_colunmsSql;
    mutable std::string m_updatePrepareSql;
    mutable std::string m_primaryKeyPrepareSql;
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

enum class IndexAttribute
{
    EMPTY,
    ASC,
    DESC,
    NO_CASE  // case-insensitive
};

class IndexColInfo
{
public:
    IndexColInfo(std::string colName, std::vector<IndexAttribute> attr = {});

    std::string indexSql() const;
    const std::string& colunmName() const;

private:
    std::string m_colName;
    std::vector<IndexAttribute> m_attributes;
};

}  // namespace sqlite

// clang-format off
#define TABLESTRUCTINFO_BEGIN(type) \
namespace sqlite \
{ \
template<> \
class TableStructInfo<::type> : public BaseTableStructInfo \
{ \
public:\
    using StructType = ::type;\
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

#define TABLESTRUCTINFO_COMLUNM_5(type, colunmName, autoIncremnet, unique, primaryKey) \
    ColumnInfo type = ColumnInfo(STR(colunmName), decltype(std::declval<StructType>().type)(), &m_columnInfos, &m_primaryColunmInfos, autoIncremnet, unique, primaryKey);
#define TABLESTRUCTINFO_COMLUNM_4(type, colunmName, autoIncremnet, unique) TABLESTRUCTINFO_COMLUNM_5(type, colunmName, autoIncremnet, unique, false)
#define TABLESTRUCTINFO_COMLUNM_3(type, colunmName, autoIncremnet) TABLESTRUCTINFO_COMLUNM_4(type, colunmName, autoIncremnet, false)
#define TABLESTRUCTINFO_COMLUNM_2(type, colunmName) TABLESTRUCTINFO_COMLUNM_3(type, colunmName, false)
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

// clang-format on

// namespace test
// {
// const auto& self2 = sqlite::TableStructInfo<sqlite::FinishedItem>::self();
// const auto& self = sqlite::TableStructInfo<sqlite::FinishedItems>::self();
// const auto& self1 = sqlite::TableStructInfo<sqlite::FinishedItemNosames>::self();
// }  // namespace test
