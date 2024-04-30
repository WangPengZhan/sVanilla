#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_map>

#include "SQLiteColunm.h"

struct sqlite3;
struct sqlite3_stmt;

namespace sqlite
{

class SQLiteDatabase;
class SQLiteStatement
{
private:
    SQLiteStatement(const SQLiteStatement& other) = delete;
    SQLiteStatement& operator=(const SQLiteStatement& other) = delete;

public:
    struct Deleter
    {
        void operator()(sqlite3_stmt* stmt);
    };
    using statementPtr = std::unique_ptr<sqlite3_stmt, Deleter>;
    using statementSPtr = std::shared_ptr<sqlite3_stmt>;
    using dbPtr = std::shared_ptr<SQLiteStatement>;

    explicit SQLiteStatement(SQLiteDatabase& db, const std::string& query);
    ~SQLiteStatement() = default;

    void clearBindings();
    int getBindParameterCount() const;
    int getIndex(const std::string& colunmName);

    bool executeStep();
    bool reset();

    bool hasRow() const;
    bool isDone() const;

    // for bind
    void bind(int index, int32_t value);
    void bind(int index, uint32_t value);
    void bind(int index, int64_t value);
    void bind(int index, double value);
    void bind(int index, const std::string& value);
    void bind(int index, const char* value);
    void bind(int index, const void* value, const int size);
    void bind(int index, const std::vector<uint8_t>& blob);
    void bindNoCopy(int index, const std::string& value);
    void bindNoCopy(int index, const char* value);
    void bindNoCopy(int index, const void* value, int size);
    void bindNoCopy(int index, const std::vector<uint8_t>& blob);
    void bind(int index);  // bind NULL value

    void bind(const std::string& columnName, int32_t value);
    void bind(const std::string& columnName, uint32_t value);
    void bind(const std::string& columnName, int64_t value);
    void bind(const std::string& columnName, double value);
    void bind(const std::string& columnName, const std::string& value);
    void bind(const std::string& columnName, const char* value);
    void bind(const std::string& columnName, const void* value, int size);
    void bind(const std::string& columnName, const std::vector<uint8_t>& blob);
    void bindNoCopy(const std::string& columnName, const std::string& value);
    void bindNoCopy(const std::string& columnName, const char* value);
    void bindNoCopy(const std::string& columnName, const void* value, int size);
    void bindNoCopy(const std::string& columnName, const std::vector<uint8_t>& blob);
    void bind(const std::string& columnName);  // bind NULL value

    std::string expandedSQL() const;
    int columnCount() const;

    SqliteColumn column(int col) const;
    SqliteColumn column(const std::string& colName) const;
    bool isColunmNull(int col) const;
    bool isColunmNull(const std::string& colName) const;
    std::string colunmName(int col) const;
    std::string originColunmName(int col) const;
    int columnIndex(const std::string& colName) const;

    int getChanges() const;

    int errorCode() const;
    int extendedErrorCode() const;
    std::string errorMsg() const;

    // for handle
    const SQLiteDatabase& db() const;
    sqlite3* dbHandle() const;
    sqlite3_stmt* handle() const;

private:
    void prepareStatement();
    int tryExecuteStep() noexcept;
    SqliteColumn getColunmFromStmt(int col) const;

private:
    statementPtr m_stmt;
    SQLiteDatabase& m_db;
    std::string m_sql;
    bool m_hasRow;
    bool m_done;
    int m_colunmCount;
    mutable std::unordered_map<std::string, int> m_colunmIndexMap;
    mutable std::unordered_map<int, SqliteColumn> m_colunmsMap;
};

}  // namespace sqlite
