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

template <typename T, int Value>
struct FiledConcretenessType
{
    static constexpr int value = Value;
};

constexpr int integralStartIndex = 0x00000000;
constexpr int floatPointStartIndex = 0x000000FF;
constexpr int stringStartIndex = 0x0000FFFF;
constexpr int blobStartIndex = 0x00FFFFFF;

template struct FiledConcretenessType<bool, integralStartIndex + 0>;
template struct FiledConcretenessType<uint8_t, integralStartIndex + 1>;
template struct FiledConcretenessType<int8_t, integralStartIndex + 2>;
template struct FiledConcretenessType<uint16_t, integralStartIndex + 3>;
template struct FiledConcretenessType<int16_t, integralStartIndex + 4>;
template struct FiledConcretenessType<uint32_t, integralStartIndex + 5>;
template struct FiledConcretenessType<int32_t, integralStartIndex + 6>;
template struct FiledConcretenessType<uint64_t, integralStartIndex + 7>;
template struct FiledConcretenessType<int64_t, integralStartIndex + 8>;
template struct FiledConcretenessType<float, floatPointStartIndex + 0>;
template struct FiledConcretenessType<double, floatPointStartIndex + 1>;
template struct FiledConcretenessType<long double, floatPointStartIndex + 2>;
template struct FiledConcretenessType<std::string, stringStartIndex + 0>;
template struct FiledConcretenessType<const char*, stringStartIndex + 1>;
template struct FiledConcretenessType<std::vector<uint8_t>, blobStartIndex + 0>;
template struct FiledConcretenessType<const void*, blobStartIndex + 1>;

template <typename T>
struct TypeToIndex;

template <>
struct TypeToIndex<bool>
{
    using type = bool;
    static constexpr int value = integralStartIndex + 0;
};

template <>
struct TypeToIndex<uint8_t>
{
    using type = uint8_t;
    static constexpr int value = integralStartIndex + 1;
};

template <>
struct TypeToIndex<int8_t>
{
    using type = int8_t;
    static constexpr int value = integralStartIndex + 2;
};

template <>
struct TypeToIndex<uint16_t>
{
    using type = uint16_t;
    static constexpr int value = integralStartIndex + 3;
};

template <>
struct TypeToIndex<int16_t>
{
    using type = int16_t;
    static constexpr int value = integralStartIndex + 4;
};

template <>
struct TypeToIndex<uint32_t>
{
    using type = uint32_t;
    static constexpr int value = integralStartIndex + 5;
};

template <>
struct TypeToIndex<int32_t>
{
    using type = int32_t;
    static constexpr int value = integralStartIndex + 6;
};

template <>
struct TypeToIndex<uint64_t>
{
    using type = uint64_t;
    static constexpr int value = integralStartIndex + 7;
};

template <>
struct TypeToIndex<int64_t>
{
    using type = int64_t;
    static constexpr int value = integralStartIndex + 8;
};

template <>
struct TypeToIndex<float>
{
    using type = float;
    static constexpr int value = floatPointStartIndex;
};

template <>
struct TypeToIndex<double>
{
    using type = double;
    static constexpr int value = floatPointStartIndex + 1;
};

template <>
struct TypeToIndex<long double>
{
    using type = long double;
    static constexpr int value = floatPointStartIndex + 2;
};

template <>
struct TypeToIndex<std::string>
{
    using type = std::string;
    static constexpr int value = stringStartIndex;
};

template <>
struct TypeToIndex<const char*>
{
    using type = const char*;
    static constexpr int value = stringStartIndex + 1;
};

template <>
struct TypeToIndex<std::vector<uint8_t>>
{
    using type = std::vector<uint8_t>;
    static constexpr int value = blobStartIndex + 0;
};

template <>
struct TypeToIndex<const void*>
{
    using type = const void*;
    static constexpr int value = blobStartIndex + 1;
};

template <typename T>
constexpr int TypeToIndex_v = TypeToIndex<T>::value;

template <typename Lambda>
decltype(auto) indexToType(int index, Lambda lambda)
{
    switch (index)
    {
    case integralStartIndex + 0:
    {
        static_assert(TypeToIndex_v<bool> == integralStartIndex + 0, "error index");
        lambda(bool{});
    }
    case integralStartIndex + 1:
    {
        static_assert(TypeToIndex_v<uint8_t> == integralStartIndex + 1, "error index");
        lambda(uint8_t{});
    }
    case integralStartIndex + 2:
    {
        static_assert(TypeToIndex_v<int8_t> == integralStartIndex + 2, "error index");
        lambda(int8_t{});
    }
    case integralStartIndex + 3:
    {
        static_assert(TypeToIndex_v<uint16_t> == integralStartIndex + 3, "error index");
        lambda(uint16_t{});
    }
    case integralStartIndex + 4:
    {
        static_assert(TypeToIndex_v<int16_t> == integralStartIndex + 4, "error index");
        lambda(int16_t{});
    }
    case integralStartIndex + 5:
    {
        static_assert(TypeToIndex_v<uint32_t> == integralStartIndex + 5, "error index");
        lambda(uint32_t{});
    }
    case integralStartIndex + 6:
    {
        static_assert(TypeToIndex_v<int32_t> == integralStartIndex + 6, "error index");
        lambda(int32_t{});
    }
    case integralStartIndex + 7:
    {
        static_assert(TypeToIndex_v<uint64_t> == integralStartIndex + 7, "error index");
        lambda(uint64_t{});
    }
    case integralStartIndex + 8:
    {
        static_assert(TypeToIndex_v<int64_t> == integralStartIndex + 8, "error index");
        lambda(int64_t{});
    }
    case floatPointStartIndex + 0:
    {
        static_assert(TypeToIndex_v<float> == floatPointStartIndex + 0, "error index");
        lambda(float{});
    }
    case floatPointStartIndex + 1:
    {
        static_assert(TypeToIndex_v<double> == floatPointStartIndex + 1, "error index");
        lambda(double{});
    }
    case floatPointStartIndex + 2:
    {
        using ValueIndex = TypeToIndex<long double>;
        static_assert(ValueIndex::value == floatPointStartIndex + 2, "error index");
        lambda(typename ValueIndex::type{});
    }
    case stringStartIndex + 0:
    {
        static_assert(TypeToIndex_v<std::string> == stringStartIndex + 0, "error index");
        lambda(std::string{});
    }
    case stringStartIndex + 1:
    {
        using ValueIndex = TypeToIndex<const char*>;
        static_assert(ValueIndex::value == stringStartIndex + 1, "error index");
        lambda(typename ValueIndex::type{});
    }
    case blobStartIndex + 0:
    {
        static_assert(TypeToIndex_v<std::vector<uint8_t>> == blobStartIndex + 0, "error index");
        lambda(std::vector<uint8_t>{});
    }
    // case blobStartIndex + 1:
    //{
    //     using ValueIndex = TypeToIndex<const void*>;
    //     static_assert(ValueIndex::value == blobStartIndex + 1, "error index");
    //     lambda(typename ValueIndex::type{});
    // }
    default:
        lambda(std::string{});
        break;
    }
}

std::string fieldTypeSql(FieldType fieldType);

template <typename ValueType>
FieldType getFieldType()
{
    if constexpr (std::is_integral_v<ValueType>)
    {
        return FieldType::Integer;
    }
    else if constexpr (std::is_floating_point_v<ValueType>)
    {
        return FieldType::Float;
    }
    else if constexpr (std::is_same_v<std::vector<uint8_t>, ValueType> || std::is_same_v<const void*, ValueType>)
    {
        return FieldType::Blob;
    }
    else if constexpr (std::is_same_v<std::string, ValueType> || std::is_same_v<const char*, ValueType>)
    {
        return FieldType::Text;
    }
    else
    {
        // static_assert(false, "FieldType error");
    }

    return FieldType::Text;
}

class ColumnInfo
{
public:
    ColumnInfo(std::string columnName, bool autoIncremnet = false, bool unique = false);
    ColumnInfo(std::string columnName, std::vector<ColumnInfo*>* pColumnInfos, std::vector<ColumnInfo*>* pPrimaryKeyColumnInfos, bool autoIncremnet = false,
               bool unique = false);

    template <typename Entity, typename ValueType>
    ColumnInfo(std::string columnName, ValueType Entity::*memberPtr, std::vector<ColumnInfo*>* pColumnInfos, std::vector<ColumnInfo*>* pPrimaryKeyColumnInfos,
               bool autoIncremnet, bool unique, bool primaryKey)
        : m_columnName(std::move(columnName))
        , m_fieldType(getFieldType<ValueType>())
        , m_autoIncrement(autoIncremnet)
        , m_unique(unique)
        , m_pColumnInfos(pColumnInfos)
        , m_pPrimaryKeyColumnInfos(pPrimaryKeyColumnInfos)
        , m_valueTypeIndex(TypeToIndex_v<ValueType>)
    {
        using MemberPtrType = ValueType Entity::*;
        auto pMemberPtr = new (MemberPtrType);
        *pMemberPtr = memberPtr;
        m_memberPtr = pMemberPtr;

        if (pColumnInfos)
        {
            pColumnInfos->push_back(this);
        }

        if (pPrimaryKeyColumnInfos && (autoIncremnet || primaryKey))
        {
            pPrimaryKeyColumnInfos->push_back(this);
        }
    }

    template <typename Entity, typename ValueType>
    auto memberPtr() const
    {
        using MemberPtrType = ValueType Entity::*;
        return static_cast<MemberPtrType*>(m_memberPtr);
    }

    int valueTypeIndex() const;

    std::string colunmSql() const;
    const std::string& colunmName() const;

private:
    std::string m_columnName;
    FieldType m_fieldType = FieldType::Text;
    bool m_autoIncrement = false;
    bool m_unique = false;
    std::vector<ColumnInfo*>* m_pColumnInfos = nullptr;
    std::vector<ColumnInfo*>* m_pPrimaryKeyColumnInfos = nullptr;
    void* m_memberPtr = nullptr;
    int m_valueTypeIndex = -1;
};

class BaseTableStructInfo
{
public:
    size_t colunmCount() const;
    ColumnInfo& columnInfo(int index) const;

    void addColumn(ColumnInfo* col);
    void addSubTable(BaseTableStructInfo& subTable);

    const std::vector<ColumnInfo*>& columnInfos() const;
    const std::vector<ColumnInfo*>& primaryColumnInfos() const;

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

void test();

template <>
class TableStructInfo<FinishedItem> : public BaseTableStructInfo
{
public:
    ColumnInfo m_uniqueId = ColumnInfo(std::string(""), &FinishedItem::uniqueId, &m_columnInfos, &m_primaryColunmInfos, false, false, false);

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
    ColumnInfo type = ColumnInfo(STR(colunmName), &StructType::type, &m_columnInfos, &m_primaryColunmInfos, autoIncremnet, unique, primaryKey);
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
