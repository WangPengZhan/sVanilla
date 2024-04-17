#include <iostream>
#include <sqlite3.h>

#include "SQLiteStatement.h"
#include "Sqlite/SQLiteLog.h"
#include "SQLiteColunm.h"

namespace SQLite
{

void bind(SQLiteStatement& stmt, int index, const SqliteColumnValue& value)
{
    std::visit([&stmt, index](auto&& obj) {
        if constexpr (std::is_same_v<std::remove_reference_t<decltype(obj)>, const std::monostate>)
        {
            stmt.bind(index);
        }
        else
        {
            stmt.bind(index, obj);
        }
    }, value);
}

std::stringstream& conditionString(std::stringstream& ss, const SqliteColumnValue& value)
{
    std::visit([&ss](auto&& obj) {
        if constexpr (std::is_same_v<std::remove_reference_t<decltype(obj)>, const std::monostate> ||
                      std::is_same_v<std::remove_reference_t<decltype(obj)>, const BlobColumnValue>)
        {
        }
        else
        {
            ss << obj;
        }
    }, value);

    return ss;
}

SqliteColumn::SqliteColumn(SqliteColumnValue value, int index, std::string colunmName)
    : m_value(std::move(value))
    , m_index(index)
    , m_colunmName(std::move(colunmName))
	
{
}

SqliteColumn::SqliteColumn(SqliteColumnValue value)
    : SqliteColumn(value, -1, "")
{
}

const std::string& SqliteColumn::getName() const
{
    return m_colunmName;
}

int32_t SqliteColumn::getInt() const noexcept
{
    int32_t res = 0;

    std::visit([&res](auto&& elem) {
        if constexpr (std::is_constructible_v<decltype(elem), int32_t>)
        {
            res = elem;
        }
    }, m_value);
    
    return res;
}

uint32_t SqliteColumn::getUInt() const noexcept
{
    uint32_t res = 0;

    std::visit([&res](auto&& elem) {
        if constexpr (std::is_constructible_v<decltype(elem), uint32_t>)
        {
            res = elem;
        }
    }, m_value);

    return res;
}

int64_t SqliteColumn::getInt64() const noexcept
{
    int64_t res = 0;

    std::visit([&res](auto&& elem) {
        if constexpr (std::is_constructible_v<decltype(elem), uint32_t>)
        {
            res = elem;
        }
    }, m_value);

    return res;
}
double SqliteColumn::getDouble() const noexcept
{
    double res = 0;

    std::visit([&res](auto&& elem) {
        if constexpr (std::is_constructible_v<decltype(elem), uint32_t>)
        {
            res = elem;
        }
    }, m_value);

    return res;
}

std::string SqliteColumn::getText(const std::string& defalutValue) const
{
    std::string res = defalutValue;
    std::visit([&res](auto&& elem) {
        if constexpr (std::is_same_v<decltype(elem), std::string>)
        {
            res = elem;
        }
        else if constexpr (std::is_same_v<decltype(elem), std::vector<uint8_t>>)
        {
            res = std::string(elem.data(), elem.size());
        }
        else if constexpr (std::is_same_v<decltype(elem), std::monostate>)
        {
            res.clear();
        }
        else if constexpr (std::is_same_v<decltype(elem), std::monostate>)
	    {
            res = std::to_string(elem);
	    }
    }, m_value);
    return res;
}

std::vector<uint8_t> SqliteColumn::getBlob() const
{
    std::vector<uint8_t> res;
    std::visit([&res](auto&& elem) {
        if constexpr (std::is_same_v<decltype(elem), std::vector<uint8_t> >)
        {
            res = elem;
        }
    }, m_value);
    return res;
}

std::string SqliteColumn::getString() const
{
    std::string res;
    std::visit([&res](auto&& elem) {
        if constexpr (std::is_same_v<decltype(elem), std::string>)
        {
            res = elem;
        }
        else if constexpr (std::is_same_v<decltype(elem), std::vector<uint8_t>>)
        {
            res = std::string(elem.data(), elem.size());
        }
        else if constexpr (std::is_same_v<decltype(elem), std::monostate>)
        {
            res.clear();
        }
        else if constexpr (std::is_same_v<decltype(elem), std::monostate>)
        {
            res = std::to_string(elem);
        }
    }, m_value);
    return res;
}

FieldType SqliteColumn::getType() const noexcept
{
    if (m_value.index() == 0)
    {
        return FieldType::Null;
    }
    else if (m_value.index() >= 1 && m_value.index() <= 3)
    {
        return FieldType::Integer;
    }
    else if (m_value.index() == 4)
    {
        return FieldType::Float;
    }
    else if (m_value.index() == 5)
    {
        return FieldType::Text;
    }
    else if (m_value.index() == 6)
    {
        return FieldType::Blob;
    }
    else
    {
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
    std::visit([&res](auto&& elem) {
        if constexpr (std::is_same_v<decltype(elem), std::string>)
        {
            res = elem;
        }
        else if constexpr (std::is_same_v<decltype(elem), std::vector<uint8_t>>)
        {
            res = std::string(elem.data(), elem.size());
        }
        else if constexpr (std::is_same_v<decltype(elem), std::monostate>)
        {
            res.clear();
        }
        else if constexpr (std::is_same_v<decltype(elem), std::monostate>)
        {
            res = std::to_string(elem);
        }
    }, m_value);
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

SQLite::SqliteColumn::operator std::vector<uint8_t>() const
{
    return getBlob();
}

}  // namespace SQLite
