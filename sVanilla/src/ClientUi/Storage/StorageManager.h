#pragma once
#include <string>

class FinishItemStorage;

namespace sqlite
{
class StorageManager
{
public:
    static StorageManager& intance();
    std::shared_ptr<FinishItemStorage> finishedItemStorage() const;

    static std::shared_ptr<FinishItemStorage> createFinishedItemStorage(const std::string& tableName);

private:
    StorageManager();
    ~StorageManager() = default;

private:
    static const std::string m_dbName;
    std::shared_ptr<FinishItemStorage> m_finishedItemStorage;
};

}  // namespace sqlite
