#pragma once
#include <string>
#include <memory>

class DownloadedItemStorage;
class DownloadingItemStorage;
class SearchHistoryStorage;
class CookiesInfoStorage;

namespace sqlite
{
class StorageManager
{
public:
    static StorageManager& instance();
    std::shared_ptr<DownloadedItemStorage> downloadedtemStorage() const;
    std::shared_ptr<DownloadingItemStorage> downloadingStorage() const;
    std::shared_ptr<SearchHistoryStorage> searchHistoryStorage() const;
    std::shared_ptr<CookiesInfoStorage> cookiesInfoStorage() const;

    bool isDownloaded(const std::string& guid) const;

    static std::shared_ptr<DownloadedItemStorage> createFinishedItemStorage(const std::string& tableName);
    static std::shared_ptr<DownloadingItemStorage> createDownloadingItemStorage(const std::string& tableName);
    static std::shared_ptr<SearchHistoryStorage> createSearchHistoryStorage(const std::string& tableName);
    static std::shared_ptr<CookiesInfoStorage> createCookiesInfoStorage(const std::string& tableName);

private:
    StorageManager();
    ~StorageManager() = default;

private:
    static const std::string m_dbName;
    std::shared_ptr<DownloadedItemStorage> m_downloadedItemStorage;
    std::shared_ptr<DownloadingItemStorage> m_downloadingItemStorage;
    std::shared_ptr<SearchHistoryStorage> m_searchHistoryStorage;
    std::shared_ptr<CookiesInfoStorage> m_cookiesInfoStorage;
};

}  // namespace sqlite
