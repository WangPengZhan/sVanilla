#pragma once
#include <string>
#include <vector>
#include <variant>
#include <sstream>

#include <sqlite3.h>

namespace sqlite
{
using BlobColumnValue = std::vector<uint8_t>;
using SqliteColumnValue = std::variant<std::monostate, int64_t, double, std::string, BlobColumnValue>;

class SQLiteStatement;
void bind(SQLiteStatement& stmt, int index, const SqliteColumnValue& value);
std::stringstream& conditionString(std::stringstream& ss, const SqliteColumnValue& value);

enum class FieldType
{
    Integer = SQLITE_INTEGER,
    Float = SQLITE_FLOAT,
    Text = SQLITE_TEXT,
    Blob = SQLITE_BLOB,
    Null = SQLITE_NULL,
};

class SqliteColumn
{
public:
    SqliteColumn(SqliteColumnValue value, int index, std::string colunmName, std::string colunmOriginName = {});
    SqliteColumn(SqliteColumnValue value = std::monostate{});
    ~SqliteColumn() = default;

    const std::string& getName() const;
    const std::string& getOriginName() const;

    int32_t getInt() const noexcept;
    uint32_t getUInt() const noexcept;
    int64_t getInt64() const noexcept;
    double getDouble() const noexcept;
    std::string getText(const std::string& defalutValue) const;
    std::vector<uint8_t> getBlob() const;
    std::string getString() const;

    FieldType getType() const noexcept;

    bool isInteger() const;
    bool isFloat() const;
    bool isText() const;
    bool isBlob() const;
    bool isNull() const;

    int getBytes() const;
    int size() const;

    operator char() const;
    operator int8_t() const;
    operator uint8_t() const;
    operator int16_t() const;
    operator uint16_t() const;
    operator int32_t() const;
    operator uint32_t() const;
    operator int64_t() const;
    operator double() const;
    operator const char*() const;
    operator std::string() const;
    operator std::vector<uint8_t>() const;

private:
    SqliteColumnValue m_value;
    int m_index;
    std::string m_colunmName;
    std::string m_originColunmName;
};

}  // namespace sqlite
