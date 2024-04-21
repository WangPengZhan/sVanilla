#pragma once
#include <variant>
#include <vector>
#include <string>

#include "BaseInfo.h"
#include "Sqlite/Database/SQLiteColunm.h"
#include "Sqlite/Database/SQLiteStatement.h"

namespace sqlite
{
class SqliteUtil
{
    static createIndexIfNotExists(SqliteDBPtr db, const std::string& tableName, );
};
   
}  // namespace sqlite
