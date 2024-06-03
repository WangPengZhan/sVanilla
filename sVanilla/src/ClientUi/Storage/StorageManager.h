#pragma once
#include <string>

class FinishItemStorage;
class DownloadingItemStorage;
class SearchHistoryStorage;

namespace sqlite
{
class StorageManager
{
public:
    static StorageManager& intance();
    std::shared_ptr<FinishItemStorage> finishedItemStorage() const;
    std::shared_ptr<DownloadingItemStorage> downloadingStorage() const;
    std::shared_ptr<SearchHistoryStorage> searchHistoryStorage() const;

    bool isDownloaded(const std::string& guid) const;

    static std::shared_ptr<FinishItemStorage> createFinishedItemStorage(const std::string& tableName);
    static std::shared_ptr<DownloadingItemStorage> createDownloadingItemStorage(const std::string& tableName);
    static std::shared_ptr<SearchHistoryStorage> createSearchHistoryStorage(const std::string& tableName);

private:
    StorageManager();
    ~StorageManager() = default;

private:
    static const std::string m_dbName;
    std::shared_ptr<FinishItemStorage> m_finishedItemStorage;
    std::shared_ptr<DownloadingItemStorage> m_downloadingItemStorage;
    std::shared_ptr<SearchHistoryStorage> m_searchHistoryStorage;
};

}  // namespace sqlite
