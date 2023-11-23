#include <sqlite3.h>

#include "SQLiteUtility.h"


namespace SQLite
{

void StmtDeleter::operator()(sqlite3_stmt * stmt)
{
    if (stmt != nullptr)
    {
        sqlite3_finalize(stmt);
    }
}
    
} // namespace SQLite


