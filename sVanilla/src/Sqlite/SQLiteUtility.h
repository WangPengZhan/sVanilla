#pragma once
#include <memory>

struct sqlite3_stmt;
namespace SQLite
{
struct StmtDeleter
{
    void operator()(sqlite3_stmt* value);
};

using SQLiteStmtPtr = std::unique_ptr<sqlite3_stmt, StmtDeleter>;

}  // namespace SQLite
