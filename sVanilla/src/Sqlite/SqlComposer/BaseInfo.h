#pragma once
#include <string>
#include <vector>
#include <type_traits>

#include "Sqlite/Database/SQLiteColunm.h"

#define ISTR(x) #x
#define STR(x)  ISTR(x)

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

    template <typename ValueType>
    ColumnInfo(std::string columnName, const ValueType& value, bool autoIncremnet = false, bool unique = false)
        : m_columnName(std::move(columnName))
        , m_autoIncrement(autoIncremnet)
        , m_unique(unique)
    {
        setFieldType(value);
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
 
      void addColumn(ColumnInfo * col) 
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

private:
    std::vector<ColumnInfo*> m_columnInfos;
};

template<typename Table>
class TableStructInfo;

struct FinishedItem
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
    ColumnInfo m_uniqueId = ColumnInfo(std::string(""), decltype(std::declval<FinishedItem>().uniqueId)());
    //ColumnInfo m_filePath(std::string("filePath"), decltype(std::declval<FinishedItem>().filePath)());
    //ColumnInfo m_bvid(std::string("bvid"), decltype(std::declval<FinishedItem>().bvid)());
    //ColumnInfo m_title(std::string("title"), decltype(std::declval<FinishedItem>().title)());
    //ColumnInfo m_duration(std::string("duration"), decltype(std::declval<FinishedItem>().duration)());

public:
    static const TableStructInfo<FinishedItem> m_self;
private:
    TableStructInfo()
    {
        init();
    }
    ~TableStructInfo() = default;

    void init()
    {
        addColumn(&m_uniqueId);
        //addColumn(&m_filePath);
        //addColumn(&m_bvid);
        //addColumn(&m_title);
        //addColumn(&m_duration);
    }
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
    static TableStructInfo<FinishedItem> m_self;\
private:\
    TableStructInfo()\
    {\
        init();\
    }\
    ~TableStructInfo() = default;\



 #define TABLESTRUCTINFO_COMLUNM(type, colunmName) \
    ColumnInfo type(STR(colunmName), decltype(std::declval<StructType>().type)());

 #define TABLESTRUCTINFO_END(type) \
 };\
 } // namespace SQLite
// clang-format on
