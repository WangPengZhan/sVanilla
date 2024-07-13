#include "Sqlite/Storage/SqliteDBManager.h"
#include "Storage/FinishedItemStorage.h"
#include "Storage/DownloadingItemStorage.h"
#include "Storage/SearchHistoryStorage.h"
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

std::shared_ptr<SearchHistoryStorage> StorageManager::searchHistoryStorage() const
{
    return m_searchHistoryStorage;
}

bool StorageManager::isDownloaded(const std::string& guid) const
{
    return m_downloadingItemStorage->isDownload(guid) || m_finishedItemStorage->isDownload(guid);
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

std::shared_ptr<SearchHistoryStorage> StorageManager::createSearchHistoryStorage(const std::string& tableName)
{
    auto readPtr = sqlite::SqliteDBManager::createDBWithMutexPtr(dbPath + "/" + m_dbName);
    auto writePtr = sqlite::SqliteDBManager::createDBWithMutexPtr(dbPath + "/" + m_dbName);
    auto& tableStruct = sqlite::TableStructInfo<typename SearchHistoryStorage::Entity>::self();
    sqlite::SqliteUtil::createTableIfNotExists(writePtr, tableName, tableStruct);
    std::string limitTrigger = "CREATE TRIGGER IF NOT EXISTS limit_table_size AFTER INSERT ON " + tableName + " ";
    limitTrigger += "BEGIN DELETE FROM " + tableName + " WHERE ";
    limitTrigger += tableStruct.url.colunmName() + " IN ( ";
    limitTrigger += "SELECT " + tableStruct.url.colunmName() + " FROM " + tableName + " ";
    limitTrigger += "ORDER BY " + tableStruct.timestamp.colunmName() + " DESC ";
    limitTrigger += "LIMIT 1 OFFSET " + std::to_string(SearchHistoryStorage::maxNum) + " );";
    limitTrigger += "END;";
    writePtr->execute(limitTrigger);

    std::string updateTimesTrigger = "CREATE TRIGGER IF NOT EXISTS update_time AFTER UPDATE ON " + tableName + " FOR EACH ROW ";
    updateTimesTrigger += "BEGIN UPDATE " + tableName + " ";
    updateTimesTrigger += "SET " + tableStruct.searchTimes.colunmName() + " = " + tableStruct.searchTimes.colunmName() + " + 1 ";
    updateTimesTrigger += "WHERE " + tableStruct.url.colunmName() + " = OLD." + tableStruct.url.colunmName() + "; ";
    updateTimesTrigger += "END;";
    writePtr->execute(updateTimesTrigger);

    return std::make_shared<SearchHistoryStorage>(readPtr, tableName, writePtr);
}

StorageManager::StorageManager()
{
    m_downloadingItemStorage = createDownloadingItemStorage("DownloadingItem");
    m_finishedItemStorage = createFinishedItemStorage("FinishedItem");
    m_searchHistoryStorage = createSearchHistoryStorage("SearchHistory");
}

}  // namespace sqlite
