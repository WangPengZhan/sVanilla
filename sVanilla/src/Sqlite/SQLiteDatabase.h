#pragma once

#include <QString>
#include <QVariant>

#include <any>

struct sqlite3;
struct sqlite3_stmt;

namespace SQLite 
{


class SQLiteDatabase 
{
private:
    SQLiteDatabase(const SQLiteDatabase& other) = delete;
    SQLiteDatabase& operator=(const SQLiteDatabase& other) = delete;

public:
    explicit SQLiteDatabase(const std::string& path);
    virtual ~SQLiteDatabase();

    bool isOpen() const;

    QStringList tables();
    QStringList views();

    bool prepare(const std::string& sql);
    bool next();
    bool execute(const std::string& sql);

    std::any value(int index) const;
    bool     bind(int index, int type, const std::any& value);

    std::string lastError() const;

    sqlite3* handle() const;

protected:
    void close();
    void finalize();
    void updateLastError();

    sqlite3*      m_db;
    sqlite3_stmt* m_stmt;
    std::string   m_lastError;
};

}  // namespace SQLite

