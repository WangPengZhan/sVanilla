#include <filesystem>

#include <sqlite3.h>

#ifdef _WIN32
#    include <windows.h>
#endif

#include "SqliteDBManager.h"

namespace
{

std::string localeToUtf8(const std::string& localeStr)
{
#ifdef _WIN32
    int len = MultiByteToWideChar(GetACP(), 0, localeStr.data(), localeStr.size(), nullptr, 0);
    std::wstring wsz_utf8(len, L'\0');
    MultiByteToWideChar(GetACP(), 0, localeStr.data(), localeStr.size(), &wsz_utf8[0], len);

    len = WideCharToMultiByte(CP_UTF8, 0, wsz_utf8.data(), wsz_utf8.size(), nullptr, 0, nullptr, nullptr);
    std::string strTemp(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wsz_utf8.data(), wsz_utf8.size(), &strTemp[0], len, nullptr, nullptr);

#else
    std::string strTemp = localeStr;
#endif

    return strTemp;
}

std::string utf8ToLocale(const std::string& utf8Str)
{
#ifdef _WIN32
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.data(), utf8Str.size(), nullptr, 0);
    std::wstring wsz_ansi(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.data(), utf8Str.size(), &wsz_ansi[0], len);

    len = WideCharToMultiByte(GetACP(), 0, wsz_ansi.data(), wsz_ansi.size(), nullptr, 0, nullptr, nullptr);
    std::string strTemp(len, '\0');
    WideCharToMultiByte(GetACP(), 0, wsz_ansi.data(), wsz_ansi.size(), &strTemp[0], len, nullptr, nullptr);

#else
    std::string strTemp = utf8Str;
#endif

    return strTemp;
}

}  // namespace

namespace sqlite
{
std::string dbPath = ".db";
std::once_flag SqliteDBManager::m_createFlag;
std::mutex SqliteDBManager::m_mutex;
std::unordered_map<std::string, SqliteWithMutexPtr> SqliteDBManager::m_defaultWriteDbWtihMutexs;
std::unordered_map<std::string, SqliteDBPtr> SqliteDBManager::m_defaultWriteDbs;

void SqliteDBManager::setDbPath(const std::string& path)
{
    dbPath = path;
}

SqliteDBPtr SqliteDBManager::createDBPtr(const std::string& path, bool createNew)
{
    init();

    std::string fullPath = std::filesystem::absolute(utf8ToLocale(path)).string();
    SqliteDBPtr db;
    if (createNew)
    {
        db = std::make_shared<SQLiteDatabase>(path);
        std::lock_guard lk(m_mutex);
        if (m_defaultWriteDbs.find(fullPath) == m_defaultWriteDbs.end())
        {
            m_defaultWriteDbs.insert({fullPath, db});
            dbInit(db);
        }
    }
    else
    {
        db = writeDBPtr(path);
    }

    return db;
}

SqliteDBPtr SqliteDBManager::writeDBPtr(const std::string& path)
{
    std::string fullPath = std::filesystem::absolute(utf8ToLocale(path)).string();
    {
        std::lock_guard lk(m_mutex);
        if (m_defaultWriteDbs.find(fullPath) == m_defaultWriteDbs.end())
        {
            auto db = std::make_shared<SQLiteDatabase>(path);
            m_defaultWriteDbs.insert({fullPath, db});
            dbInit(db);
        }

        return m_defaultWriteDbs.at(fullPath);
    }
}

SqliteWithMutexPtr SqliteDBManager::createDBWithMutexPtr(const std::string& path, bool createNew)
{
    init();
    std::string fullPath = std::filesystem::absolute(utf8ToLocale(path)).string();
    SqliteWithMutexPtr db;
    if (createNew)
    {
        db = std::make_shared<SqliteDbWithMutex>(path);
        std::lock_guard lk(m_mutex);
        if (m_defaultWriteDbWtihMutexs.find(fullPath) == m_defaultWriteDbWtihMutexs.end())
        {
            m_defaultWriteDbWtihMutexs.insert({fullPath, std::make_shared<SqliteDbWithMutex>(path)});
            dbInit(db);
        }
    }
    else
    {
        db = writeDBWithMutexPtr(path);
    }

    return db;
}

SqliteWithMutexPtr SqliteDBManager::writeDBWithMutexPtr(const std::string& path)
{
    std::string fullPath = std::filesystem::absolute(utf8ToLocale(path)).string();
    {
        std::lock_guard lk(m_mutex);
        if (m_defaultWriteDbWtihMutexs.find(fullPath) == m_defaultWriteDbWtihMutexs.end())
        {
            auto db = std::make_shared<SqliteDbWithMutex>(path);
            m_defaultWriteDbWtihMutexs.insert({fullPath, db});
            dbInit(db);
        }

        return m_defaultWriteDbWtihMutexs.at(fullPath);
    }
}

void SqliteDBManager::init()
{
    std::call_once(m_createFlag, []() {
        sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        createDir();
    });
}

void SqliteDBManager::dbInit(SQLiteDatabase& db)
{
    db.execute("PRAGMA journal_mode=WAL;");
    db.execute("PRAGMA synchronous=OFF;");
}

void SqliteDBManager::dbInit(SqliteDBPtr& db)
{
    if (db)
    {
        dbInit(*(db.get()));
    }
}

void SqliteDBManager::dbInit(SqliteWithMutexPtr& db)
{
    if (db)
    {
        std::lock_guard lk(db->mutex);
        dbInit(*(db.get()));
    }
}

void SqliteDBManager::createDir()
{
    auto dbDir = std::filesystem::path(utf8ToLocale(dbPath));
    if (!std::filesystem::is_directory(dbDir))
    {
        std::filesystem::create_directory(dbDir);
    }
}

}  // namespace sqlite
