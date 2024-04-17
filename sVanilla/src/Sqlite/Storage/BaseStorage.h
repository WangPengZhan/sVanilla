//#pragma once 
//
//
//class BaseStorage
//{
//public:
//    BaseStorage(SqliteDBPtr readDBPtr, SqliteDBPtr writeDBPtr);
//    BaseStorage(SqliteDBPtr readDBPtr, SqliteDBPtr writeDBPtr, std::string tableName);
//    virtual ~BaseStorage() = default;
//
//protected:
//
//protected:
//    SqliteDBPtr m_writeDBPtr;
//    SqliteDBPtr m_readDBPtr;
//    std::mutex m_readMutex;
//    std::mutex m_writeMutex;
//};