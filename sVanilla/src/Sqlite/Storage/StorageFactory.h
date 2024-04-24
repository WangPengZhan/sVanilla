#pragma once
#include <memory>
#include <string>

#include "Sqlite/Database/SQLiteDatabase.h"
#include "Sqlite/SqlComposer/SqlUtil.h"
#include "Sqlite/SqlComposer/BaseInfo.h"

namespace sqlite
{
class StorageFactory
{
public:
    template <typename T>
    static std::shared_ptr<T> createStorage(SqliteDBPtr db, const std::string& tableName)
    {
        if (!db)
        {
            return std::shared_ptr<T>();
        }

        auto tableStruct = TableStructInfo<typename T::Entity>::self();
        SqliteUtil::createTableIfNotExists(db, tableName, tableStruct);
        return std::make_shared<T>(db, tableName);
    }
};

}  // namespace sqlite
