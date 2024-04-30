#include "Sqlite/Storage/SqliteDBManager.h"
#include "FinishedItemStorage.h"
#include "ClientUi/Storage/DownloadingItemStorage.h"
#include "StorageManager.h"

namespace sqlite
{

const std::string StorageManager::m_dbName{"data.db"};

StorageManager& StorageManager::intance()
{
    static StorageManager storageManager;
    return storageManager;
}

std::shared_ptr<FinishItemStorage> StorageManager::finishedItemStorage() const
{
    return m_finishedItemStorage;
}

std::shared_ptr<DownloadingItemStorage> StorageManager::downloadingStorage() const
{
    return m_downloadingItemStorage;
}

std::shared_ptr<FinishItemStorage> StorageManager::createFinishedItemStorage(const std::string& tableName)
{
    auto readPtr = sqlite::SqliteDBManager::createDBWithMutexPtr(dbPath + "/" + m_dbName);
    auto writePtr = sqlite::SqliteDBManager::createDBWithMutexPtr(dbPath + "/" + m_dbName);
    auto& tableStruct = sqlite::TableStructInfo<typename FinishItemStorage::Entity>::self();
    sqlite::SqliteUtil::createTableIfNotExists(writePtr, tableName, tableStruct);

    std::vector<std::string> indexColNames = {tableStruct.filePath.colunmName()};
    std::string indexName = sqlite::SqliteUtil::indexName(tableName, indexColNames);
    sqlite::SqliteUtil::createIndexIfNotExists(writePtr, tableName, indexName, indexColNames);
    indexColNames = {tableStruct.bvid.colunmName()};
    indexName = sqlite::SqliteUtil::indexName(tableName, indexColNames);
    sqlite::SqliteUtil::createIndexIfNotExists(writePtr, tableName, indexName, indexColNames);

    return std::make_shared<FinishItemStorage>(readPtr, tableName, writePtr);
}

std::shared_ptr<DownloadingItemStorage> StorageManager::createDownloadingItemStorage(const std::string& tableName)
{
    auto readPtr = sqlite::SqliteDBManager::createDBWithMutexPtr(dbPath + "/" + m_dbName);
    auto writePtr = sqlite::SqliteDBManager::createDBWithMutexPtr(dbPath + "/" + m_dbName);
    auto& tableStruct = sqlite::TableStructInfo<typename DownloadingItemStorage::Entity>::self();
    sqlite::SqliteUtil::createTableIfNotExists(writePtr, tableName, tableStruct);

    std::vector<std::string> indexColNames = {tableStruct.filePath.colunmName()};
    std::string indexName = sqlite::SqliteUtil::indexName(tableName, indexColNames);
    sqlite::SqliteUtil::createIndexIfNotExists(writePtr, tableName, indexName, indexColNames);
    indexColNames = {tableStruct.bvid.colunmName()};
    indexName = sqlite::SqliteUtil::indexName(tableName, indexColNames);
    sqlite::SqliteUtil::createIndexIfNotExists(writePtr, tableName, indexName, indexColNames);

    return std::make_shared<DownloadingItemStorage>(readPtr, tableName, writePtr);
}

StorageManager::StorageManager()
{
    m_downloadingItemStorage = createDownloadingItemStorage("DownloadingItem");
    m_finishedItemStorage = createFinishedItemStorage("FinishedItem");
}

}  // namespace sqlite
