#include <iostream>
#include <type_traits>
#include <utility>
#include <sqlite3.h>

#include "SQLiteStatement.h"
#include "Sqlite/SQLiteLog.h"
#include "SQLiteColumn.h"

namespace sqlite
{

void bind(SQLiteStatement& stmt, int index, const SqliteColumnValue& value)
{
    std::visit(
        [&stmt, index](auto&& obj) {
            if constexpr (std::is_same_v<std::remove_cvref_t<decltype(obj)>, std::monostate>)
            {
                stmt.bind(index);
            }
            else
            {
                stmt.bind(index, obj);
            }
        },
        value);
}

std::stringstream& conditionString(std::stringstream& ss, const SqliteColumnValue& value)
{
    std::visit(
        [&ss](auto&& obj) {
            if constexpr (std::is_same_v<std::remove_cvref_t<decltype(obj)>, std::monostate> ||
                          std::is_same_v<std::remove_cvref_t<decltype(obj)>, BlobColumnValue>)
            {
            }
            else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(obj)>, std::string>)
            {
                ss << "'" << obj << "'";
            }
            else
            {
                ss << obj;
            }
        },
        value);

    return ss;
}

SqliteColumn::SqliteColumn(SqliteColumnValue value, int index, std::string columnName, std::string originOriginName)
    : m_value(std::move(value))
    , m_index(index)
    , m_columnName(std::move(columnName))
    , m_originColumnName(std::move(originOriginName))

{
}

SqliteColumn::SqliteColumn(SqliteColumnValue value)
    : SqliteColumn(value, -1, "")
{
}

const std::string& SqliteColumn::getName() const
{
    return m_columnName;
}

const std::string& SqliteColumn::getOriginName() const
{
    return m_originColumnName;
}

const SqliteColumnValue& SqliteColumn::value() const
{
    return m_value;
}

int32_t SqliteColumn::getInt() const noexcept
{
    int32_t res = 0;

    std::visit(
        [&res](auto&& elem) {
            using ElemType = std::remove_cvref_t<decltype(elem)>;
            if constexpr (std::is_arithmetic_v<ElemType>)
            {
                res = static_cast<int32_t>(elem);
            }
        },
        m_value);

    return res;
}

uint32_t SqliteColumn::getUInt() const noexcept
{
    uint32_t res = 0;

    std::visit(
        [&res](auto&& elem) {
            using ElemType = std::remove_cvref_t<decltype(elem)>;
            if constexpr (std::is_arithmetic_v<ElemType>)
            {
                res = static_cast<uint32_t>(elem);
            }
        },
        m_value);

    return res;
}

int64_t SqliteColumn::getInt64() const noexcept
{
    int64_t res = 0;

    std::visit(
        [&res](auto&& elem) {
            using ElemType = std::remove_cvref_t<decltype(elem)>;
            if constexpr (std::is_arithmetic_v<ElemType>)
            {
                res = static_cast<int64_t>(elem);
            }
        },
        m_value);

    return res;
}

double SqliteColumn::getDouble() const noexcept
{
    double res = 0;

    std::visit(
        [&res](auto&& elem) {
            using ElemType = std::remove_cvref_t<decltype(elem)>;
            if constexpr (std::is_arithmetic_v<ElemType>)
            {
                res = static_cast<double>(elem);
            }
        },
        m_value);

    return res;
}

std::string SqliteColumn::getText(const std::string& defaultValue) const
{
    std::string res = defaultValue;
    std::visit(
        [&res](auto&& elem) {
            using ElemType = std::remove_cvref_t<decltype(elem)>;
            if constexpr (std::is_same_v<ElemType, std::string>)
            {
                res = elem;
            }
            else if constexpr (std::is_same_v<ElemType, std::vector<uint8_t>>)
            {
                res.clear();
                if (!elem.empty())
                {
                    res.assign(reinterpret_cast<const char*>(elem.data()), elem.size());
                }
            }
            else if constexpr (std::is_same_v<ElemType, int64_t> || std::is_same_v<ElemType, double>)
            {
                res = std::to_string(elem);
            }
            else if constexpr (std::is_same_v<ElemType, std::monostate>)
            {
                res.clear();
            }
        },
        m_value);
    return res;
}

std::vector<uint8_t> SqliteColumn::getBlob() const
{
    std::vector<uint8_t> res;
    std::visit(
        [&res](auto&& elem) {
            if constexpr (std::is_same_v<std::remove_cvref_t<decltype(elem)>, std::vector<uint8_t>>)
            {
                res = elem;
            }
        },
        m_value);
    return res;
}

std::string SqliteColumn::getString() const
{
    std::string res;
    std::visit(
        [&res](auto&& elem) {
            using ElemType = std::remove_cvref_t<decltype(elem)>;
            if constexpr (std::is_same_v<ElemType, std::string>)
            {
                res = elem;
            }
            else if constexpr (std::is_same_v<ElemType, std::vector<uint8_t>>)
            {
                if (!elem.empty())
                {
                    res.assign(reinterpret_cast<const char*>(elem.data()), elem.size());
                }
            }
            else if constexpr (std::is_same_v<ElemType, int64_t> || std::is_same_v<ElemType, double>)
            {
                res = std::to_string(elem);
            }
            else if constexpr (std::is_same_v<ElemType, std::monostate>)
            {
                res.clear();
            }
        },
        m_value);
    return res;
}

FieldType SqliteColumn::getType() const noexcept
{
    static_assert(std::variant_size_v<SqliteColumnValue> == 5, "The number of SqliteColumnValue alternatives has changed; update the type mapping below");
    static_assert(std::is_same_v<std::variant_alternative_t<0, SqliteColumnValue>, std::monostate>, "Index 0 must be std::monostate");
    static_assert(std::is_same_v<std::variant_alternative_t<1, SqliteColumnValue>, int64_t>, "Index 1 must be int64_t");
    static_assert(std::is_same_v<std::variant_alternative_t<2, SqliteColumnValue>, double>, "Index 2 must be double");
    static_assert(std::is_same_v<std::variant_alternative_t<3, SqliteColumnValue>, std::string>, "Index 3 must be std::string");
    static_assert(std::is_same_v<std::variant_alternative_t<4, SqliteColumnValue>, BlobColumnValue>, "Index 4 must be BlobColumnValue");

    switch (m_value.index())
    {
    case 1:
        return FieldType::Integer;
    case 2:
        return FieldType::Float;
    case 3:
        return FieldType::Text;
    case 4:
        return FieldType::Blob;
    case 0:
    default:
        return FieldType::Null;
    }
}

bool SqliteColumn::isInteger() const
{
    return getType() == FieldType::Integer;
}

bool SqliteColumn::isFloat() const
{
    return getType() == FieldType::Float;
}

bool SqliteColumn::isText() const
{
    return getType() == FieldType::Text;
}

bool SqliteColumn::isBlob() const
{
    return getType() == FieldType::Blob;
}

bool SqliteColumn::isNull() const
{
    return getType() == FieldType::Null;
}

int SqliteColumn::getBytes() const
{
    std::size_t res = 0;
    std::visit(
        [&res](auto&& elem) {
            if constexpr (std::is_same_v<std::remove_cvref_t<decltype(elem)>, std::string> ||
                          std::is_same_v<std::remove_cvref_t<decltype(elem)>, std::vector<uint8_t>>)
            {
                res = elem.size();
            }
            else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(elem)>, int64_t> || std::is_same_v<std::remove_cvref_t<decltype(elem)>, double>)
            {
                res = sizeof(elem);
            }
            else
            {
                res = 0;
            }
        },
        m_value);
    return res;
}

int SqliteColumn::size() const
{
    return getBytes();
}

SqliteColumn::operator char() const
{
    return static_cast<char>(getInt());
}

SqliteColumn::operator int8_t() const
{
    return static_cast<int8_t>(getInt());
}

SqliteColumn::operator uint8_t() const
{
    return static_cast<uint8_t>(getInt());
}

SqliteColumn::operator int16_t() const
{
    return static_cast<int16_t>(getInt());
}

SqliteColumn::operator uint16_t() const
{
    return static_cast<uint16_t>(getInt());
}

SqliteColumn::operator int32_t() const
{
    return getInt();
}

SqliteColumn::operator uint32_t() const
{
    return getUInt();
}

SqliteColumn::operator int64_t() const
{
    return getInt64();
}

SqliteColumn::operator double() const
{
    return getDouble();
}

SqliteColumn::operator const char*() const
{
    static std::string value;
    value = getText("");
    return value.c_str();
}

sqlite::SqliteColumn::operator std::vector<uint8_t>() const
{
    return getBlob();
}

}  // namespace sqlite
