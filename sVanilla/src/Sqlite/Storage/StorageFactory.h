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
    template <typename Storage, typename... StorageArgs>
    static std::shared_ptr<Storage> createStorage(SqliteWithMutexPtr read, std::string tableName, StorageArgs&&... args)
    {
        if (!read)
        {
            return {};
        }

        auto tableStruct = TableStructInfo<typename Storage::Entity>::self();
        SqliteUtil::createTableIfNotExists(read, tableName, tableStruct);
        return std::make_shared<Storage>(read, tableName, std::forward<StorageArgs>(args)...);
    }
};

}  // namespace sqlite
