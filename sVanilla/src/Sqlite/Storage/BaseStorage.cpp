//#include "Storage/Interface/BaseStorage.h"
//
//void BaseStorage::setDBCStorage(std::shared_ptr<DBCStorage> dbcStorage)
//{
//    m_dbcStorage = dbcStorage;
//}
//
//BaseStorage::BaseStorage(SqliteDBPtr readDBPtr, SqliteDBPtr writeDBPtr)
//    : m_readDBPtr(readDBPtr)
//    , m_writeDBPtr(writeDBPtr)
//    , m_dbOpt(readDBPtr, writeDBPtr, "")
//{
//}
//
//BaseStorage::BaseStorage(SqliteDBPtr readDBPtr, SqliteDBPtr writeDBPtr, std::string tableName)
//    : m_readDBPtr(readDBPtr)
//    , m_writeDBPtr(writeDBPtr)
//    , m_dbOpt(readDBPtr, writeDBPtr, tableName)
//{
//}
//
//template<typename EntityType>
//std::vector<EntityType> BaseStorage::queryTraceNormalOnline(int index, int nums, const QueryWrapper& queryWrapper)
//{
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (index < 0)
//    {
//        return {};
//    }
//    SQL_EXEC_TIMER_GUARD(timer);
//    if (!m_dbcStorage)
//    {
//        return m_dbOpt.queryEntities<EntityType>(index, nums, queryWrapper);
//    }
//    else
//    {
//        std::string dbcTableName = m_dbcStorage->getAllChannleDBCName();
//        if (dbcTableName.empty())
//        {
//            return m_dbOpt.queryEntities<EntityType>(index, nums, queryWrapper);
//        }
//        std::stringstream ss;
//        ss << "SELECT * FROM " << m_dbOpt.getTableName();
//        ss << " LEFT JOIN ";
//        ss << "(";
//        ss << "SELECT " << g_dbcTableCols << " FROM " << dbcTableName;
//        ss << ") as b";
//        ss << " ON " << getDbcConnectCondition("", "b", {}, g_dbcTableMaps);
//
//        ss << queryWrapper.getConditionStr();
//        if (queryWrapper.existCondition())
//        {
//            ss << " ORDER BY trace_id ";
//        }
//        ss << " LIMIT " << index << "," << nums;
//
//        std::string querySql = ss.str();
//
//        std::vector<EntityType> entities;
//        try
//        {
//            SQLite::Statement statement(*m_readDBPtr, querySql);
//            try
//            {
//                entities.reserve(nums);
//                queryWrapper.bind(statement);
//                timer.setSql(statement.getExpandedSQL());
//                while (statement.executeStep())
//                {
//                    EntityType entity;
//                    entity.setValue(statement);
//                    if (!dbcTableName.empty())
//                    {
//                        entity.m_messageName = statement.getColumn("message_name").getString();
//                        entity.m_comment = statement.getColumn("comment").getString();
//                    }
//                    entities.push_back(std::move(entity));
//                }
//                timer.setResultNum(entities.size());
//            }
//            catch (const std::exception&)
//            {
//                STORAGE_LOG_ERROR("errorSql: {}, errorMsg: {}", statement.getExpandedSQL(), statement.getErrorMsg());
//            }
//        }
//        catch (const std::exception& e)
//        {
//            STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", querySql, e.what());
//        }
//        return entities;
//    }
//}
//
//template<typename EntityType>
//std::vector<EntityType> BaseStorage::queryTraceNormalOnlineWithSingleTable(int index, int nums, const QueryWrapper& queryWrapper)
//{
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (index < 0)
//    {
//        return {};
//    }
//    return m_dbOpt.queryEntities<EntityType>(index, nums, queryWrapper);
//}
//
//template<typename EntityType>
//std::vector<EntityType> BaseStorage::queryTraceNormalOffline(
//    int index, int nums, const QueryWrapper& queryWrapper
//    , const std::vector<std::string>& tableNames
//)
//{
//    SQL_EXEC_TIMER_GUARD(timer);
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (tableNames.empty())
//    {
//        return {};
//    }
//
//    //offline table will not conflict
//    std::stringstream ss;
//    {
//        ss << "SELECT * FROM (";
//        ss << "SELECT * FROM " << tableNames[0];
//        for (size_t i = 1; i < tableNames.size(); i++)
//        {
//            ss << " UNION ALL";
//            ss << " SELECT * FROM " << tableNames[i];
//        }
//        ss << ") as base";
//    }
//
//    
//    std::string dbcTableName;
//
//    if (m_dbcStorage)
//    {
//        dbcTableName = m_dbcStorage->getAllChannleDBCName();
//    }
//
//    if (!dbcTableName.empty())
//    {
//        ss << " LEFT JOIN ";
//        ss << "(";
//        ss << "SELECT " << g_dbcTableCols << " FROM " << dbcTableName;
//        ss << ") as b";
//        if constexpr (std::is_same_v<EntityType, TraceMsgCANEntity>)
//        {
//            ss << " ON (base.message_id == b.dbc_message_id OR base.trace_message_name == b.message_name) AND base.channel_id == b.dbc_channel_id \
//                    AND base.IDE == b.is_extended AND base.carrier == b.is_canfd ";
//        }
//        else
//        {
//            ss << " ON (base.message_id == b.dbc_message_id OR base.trace_message_name == b.message_name) AND base.channel_id == b.dbc_channel_id";
//        }
//    }
//
//    ss << " " << queryWrapper.getConditionStr();
//    ss << " ORDER BY timestamp LIMIT " << index << "," << nums;
//
//    std::string sql = ss.str();
//
//    std::vector<EntityType> entities;
//    try
//    {
//        SQLite::Statement statement(*m_readDBPtr, sql);
//        queryWrapper.bind(statement);
//        timer.setSql(statement.getExpandedSQL());
//        try
//        {
//            while (statement.executeStep())
//            {
//                EntityType entity;
//                entity.setValue(statement);
//                if (!dbcTableName.empty())
//                {
//                    entity.m_messageName = statement.getColumn("message_name").getString();
//                    entity.m_comment = statement.getColumn("comment").getString();
//                }
//                entities.push_back(std::move(entity));
//            }
//            timer.setResultNum(entities.size());
//        }
//        catch (const std::exception&)
//        {
//            STORAGE_LOG_ERROR("errorSql: {}, errorMsg: {}", statement.getExpandedSQL(), statement.getErrorMsg());
//        }
//    }
//    catch (const std::exception& e)
//    {
//        STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", ss.str(), e.what());
//    }
//
//    return entities;
//}
//
//template<typename EntityType>
//[[ deprecated ]] std::vector<EntityType> BaseStorage::queryTraceFixOnline(int index, int nums, const QueryWrapper& queryWrapper)
//{
//#if 0
//    SQL_EXEC_TIMER_GUARD(timer);
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    std::string tableName = m_dbOpt.getTableName();
//    if (tableName.empty())
//    {
//        return std::vector<EntityType>();
//    }
//    std::stringstream ss;
//    ss << "SELECT *, count(*) as count FROM " << tableName;
//    std::vector<std::string> dbcTables;
//    if (m_dbcStorage && !(dbcTables = m_dbcStorage->getAllChannleDBCName()).empty())
//    {
//        ss << " LEFT JOIN ";
//        ss << "(";
//        for (size_t i = 0; i < dbcTables.size() - 1; i++)
//        {
//            ss << "SELECT message_id as dbc_message_id, channel_id as dbc_channel_id, message_name, comment FROM " << dbcTables[i] << " UNION ALL";
//        }
//        ss << "SELECT message_id as dbc_message_id, channel_id as dbc_channel_id, message_name, comment FROM " << dbcTables[dbcTables.size() - 1];
//        ss << ") as b";
//        ss << " ON message_id = b.dbc_message_id AND channel_id == b.dbc_channel_id";
//    }
//
//    ss << " " << queryWrapper.getConditionStr();
//
//    ss << " GROUP BY unique_id LIMIT " << index << ", " << nums;
//
//    std::string sql = ss.str();
//    std::vector<EntityType> entities;
//    try
//    {
//        SQLite::Statement statement(*m_readDBPtr, sql);
//        queryWrapper.bind(statement);
//        timer.setSql(statement.getExpandedSQL());
//        try
//        {
//            while (statement.executeStep())
//            {
//                EntityType entity;
//                entity.setValue(statement);
//                entity.m_count = statement.getColumn("count").getInt();
//                if (!dbcTables.empty())
//                {
//                    entity.m_messageName = statement.getColumn("message_name").getString();
//                    entity.m_comment = statement.getColumn("comment").getString();
//                }
//                entities.push_back(std::move(entity));
//            }
//            statement.reset();
//            timer.setResultNum(entities.size());
//        }
//        catch (const std::exception&)
//        {
//            STORAGE_LOG_ERROR("errorSql: {}, errorMsg: {}", statement.getExpandedSQL(), statement.getErrorMsg());
//        }
//    }
//    catch (const std::exception& e)
//    {
//        STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", sql, e.what());
//    }
//    return entities;
//#endif 
//    return {};
//}
//
//template<typename EntityType>
//[[ deprecated ]] std::vector<EntityType> BaseStorage::queryTraceFixOffline(int index, int nums, const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames)
//{
//#if 0
//    SQL_EXEC_TIMER_GUARD(timer);
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (tableNames.empty())
//    {
//        return std::vector<EntityType>();
//    }
//    std::stringstream ss;
//    ss << "SELECT *, count(*) as count FROM (";
//    ss << "SELECT * FROM (";
//    if (tableNames.size() >= 2)
//    {
//        ss << "SELECT * FROM " << tableNames[0];
//        for (size_t i = 1; i < tableNames.size(); i++)
//        {
//            ss << " UNION ALL";
//            ss << " SELECT * FROM " << tableNames[i];
//        }
//    }
//    else
//    {
//        ss << "SELECT * FROM " << tableNames[0];
//    }
//    ss << ") ORDER BY timestamp DESC )";
//    std::vector<std::string> dbcTables;
//    if (m_dbcStorage && !(dbcTables = m_dbcStorage->getAllChannleDBCName()).empty())
//    {
//        ss << " LEFT JOIN ";
//        ss << "(";
//        for (size_t i = 0; i < dbcTables.size() - 1; i++)
//        {
//            ss << "SELECT message_id as dbc_message_id, channel_id as dbc_channel_id, message_name, comment FROM " << dbcTables[i] << " UNION ALL";
//        }
//        ss << "SELECT message_id as dbc_message_id, channel_id as dbc_channel_id, message_name, comment FROM " << dbcTables[dbcTables.size() - 1];
//        ss << ") as b";
//        ss << " ON message_id = b.dbc_message_id AND channel_id == b.dbc_channel_id";
//    }
//
//    ss << " " << queryWrapper.getConditionStr();
//
//    ss << " GROUP BY unique_id LIMIT " << index << ", " << nums;
//
//    std::string sql = ss.str();
//    std::vector<EntityType> entities;
//    try
//    {
//        SQLite::Statement statement(*m_readDBPtr, sql);
//        queryWrapper.bind(statement);
//        timer.setSql(statement.getExpandedSQL());
//        try
//        {
//            while (statement.executeStep())
//            {
//                EntityType entity;
//                entity.setValue(statement);
//                entity.m_count = statement.getColumn("count").getInt();
//                if (!dbcTables.empty())
//                {
//                    entity.m_messageName = statement.getColumn("message_name").getString();
//                    entity.m_comment = statement.getColumn("comment").getString();
//                }
//                entities.push_back(std::move(entity));
//            }
//            std::reverse(entities.begin(), entities.end());
//            timer.setResultNum(entities.size());
//        }
//        catch (const std::exception&)
//        {
//            STORAGE_LOG_ERROR("errorSql: {}, errorMsg: {}", statement.getExpandedSQL(), statement.getErrorMsg());
//        }
//    }
//    catch (const std::exception& e)
//    {
//        STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", sql, e.what());
//    }
//    return entities;
//#endif 
//    return {};
//}
//
//template<typename EntityType>
//int32_t BaseStorage::insertEntities(const std::vector<EntityType>& entities, const std::string& insertSql)
//{
//    std::lock_guard<std::mutex> lk(m_writeMutex);
//    return m_dbOpt.insertEntities(entities, insertSql);
//}
//
//// Explicitly instantiating template functions helps to avoid redundant definitions when used.
//template std::vector<TraceMsgCANEntity> BaseStorage::queryTraceNormalOnline(int index, int nums, const QueryWrapper& queryWrapper);
//template std::vector<TraceMsgLINEntity> BaseStorage::queryTraceNormalOnline(int index, int nums, const QueryWrapper& queryWrapper);
//template std::vector<TraceMsgETHEntity> BaseStorage::queryTraceNormalOnline(int index, int nums, const QueryWrapper& queryWrapper);
//
//template std::vector<TraceMsgCANEntity> BaseStorage::queryTraceFixOnline(int index, int nums, const QueryWrapper& queryWrapper);
//template std::vector<TraceMsgLINEntity> BaseStorage::queryTraceFixOnline(int index, int nums, const QueryWrapper& queryWrapper);
//template std::vector<TraceMsgETHEntity> BaseStorage::queryTraceFixOnline(int index, int nums, const QueryWrapper& queryWrapper);
//
//template std::vector<TraceMsgCANEntity> BaseStorage::queryTraceNormalOnlineWithSingleTable(int index, int nums, const QueryWrapper& queryWrapper);
//template std::vector<TraceMsgLINEntity> BaseStorage::queryTraceNormalOnlineWithSingleTable(int index, int nums, const QueryWrapper& queryWrapper);
//template std::vector<TraceMsgETHEntity> BaseStorage::queryTraceNormalOnlineWithSingleTable(int index, int nums, const QueryWrapper& queryWrapper);
//
//template std::vector<TraceMsgCANEntity> BaseStorage::queryTraceNormalOffline(int index, int nums, const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames);
//template std::vector<TraceMsgLINEntity> BaseStorage::queryTraceNormalOffline(int index, int nums, const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames);
//template std::vector<TraceMsgETHEntity> BaseStorage::queryTraceNormalOffline(int index, int nums, const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames);
//
//template std::vector<TraceMsgCANEntity> BaseStorage::queryTraceFixOffline(int index, int nums, const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames);
//template std::vector<TraceMsgLINEntity> BaseStorage::queryTraceFixOffline(int index, int nums, const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames);
//template std::vector<TraceMsgETHEntity> BaseStorage::queryTraceFixOffline(int index, int nums, const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames);
//
//template int32_t BaseStorage::insertEntities<TraceMsgCANEntity>(const std::vector<TraceMsgCANEntity>& entities, const std::string& insertSql);
//template int32_t BaseStorage::insertEntities<TraceMsgLINEntity>(const std::vector<TraceMsgLINEntity>& entities, const std::string& insertSql);
//template int32_t BaseStorage::insertEntities<TraceMsgETHEntity>(const std::vector<TraceMsgETHEntity>& entities, const std::string& insertSql);
//template int32_t BaseStorage::insertEntities<MsgNameMapperEntity>(const std::vector<MsgNameMapperEntity>& entities, const std::string& insertSql);
//template int32_t BaseStorage::insertEntities<SignalEntity>(const std::vector<SignalEntity>& entities, const std::string& insertSql);
//
//int64_t BaseStorage::countTracesNormalOffline(const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames)
//{
//    if (queryWrapper.contain(m_msgWrapper.m_messageName.m_columnName))
//    {
//        return countMsgNamesOffline(queryWrapper, tableNames);
//    }
//    else
//    {
//        return countEntitiesOfflineWithSingleTable(queryWrapper, tableNames);
//    }
//}
//
//int64_t BaseStorage::countTracesNormalOnline(const QueryWrapper& queryWrapper)
//{
//    int64_t ret = BaseStorage::countEntitiesOnline(queryWrapper);
//    return ret < 0 ? 0 : ret;
//}
//
//[[ deprecated ]] int64_t BaseStorage::countTracesFixOnline(const QueryWrapper& queryWrapper)
//{
//#if 0
//    SQL_EXEC_TIMER_GUARD(timer);
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    std::stringstream ss;
//    ss << "SELECT count(1) FROM (";
//    ss << "SELECT DISTINCT(unique_id) FROM " << m_dbOpt.getTableName() << " " << queryWrapper.getConditionStr();
//    ss << ")";
//    std::string sql = ss.str();
//    try
//    {
//        SQLite::Statement statement(*m_readDBPtr, sql);
//        queryWrapper.bind(statement);
//        timer.setSql(statement.getExpandedSQL());
//        while (statement.executeStep())
//        {
//            return statement.getColumn(0).getUInt();
//        }
//    }
//    catch (const std::exception& e)
//    {
//        STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", sql, e.what());
//    }
//#endif
//    return 0;
//}
//
//[[ deprecated ]] int64_t BaseStorage::countTracesFixOffline(const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames)
//{
//#if 0
//    SQL_EXEC_TIMER_GUARD(timer);
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (tableNames.empty())
//    {
//        return 0;
//    }
//    std::stringstream ss;
//    ss << "SELECT count(1) FROM (";
//    if (tableNames.size() >= 2)
//    {
//        ss << "SELECT DISTINCT(unique_id) FROM (";
//        ss << "SELECT DISTINCT(unique_id) FROM " << tableNames[0] << " " << queryWrapper.getConditionStr();
//        for (size_t i = 1; i < tableNames.size(); i++)
//        {
//            ss << " UNION ALL";
//            ss << " SELECT DISTINCT(unique_id) FROM " << tableNames[i] << " " << queryWrapper.getConditionStr();
//        }
//        ss << ")";
//    }
//    else
//    {
//        ss << "SELECT DISTINCT(unique_id) FROM " << tableNames[0] << " " << queryWrapper.getConditionStr();
//    }
//    ss << ")";
//    std::string sql = ss.str();
//    try
//    {
//        SQLite::Statement statement(*m_readDBPtr, sql);
//        int startIndex = 0;
//        for (size_t i = 0; i < tableNames.size(); i++)
//        {
//            startIndex += queryWrapper.bind(statement, startIndex);
//        }
//        timer.setSql(statement.getExpandedSQL());
//        while (statement.executeStep())
//        {
//            return statement.getColumn(0).getUInt();
//        }
//    }
//    catch (const std::exception& e)
//    {
//        STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", sql, e.what());
//    }
//#endif
//    return 0;
//}
//
//int64_t BaseStorage::countMsgNamesOnline(const QueryWrapper& queryWrapper)
//{
//    int64_t ret = -1;
//    SQL_EXEC_TIMER_GUARD(timer);
//    if (!m_dbcStorage)
//    {
//        return ret;
//    }
//    else
//    {
//        std::string dbcTableName = m_dbcStorage->getAllChannleDBCName();
//        if (dbcTableName.empty())
//        {
//            return ret;
//        }
//        std::stringstream ss;
//        ss << "SELECT COUNT(1) FROM (";
//        ss << "SELECT * FROM " << m_dbOpt.getTableName();
//        ss << " LEFT JOIN ";
//        ss << "(";
//        ss << "SELECT " << g_dbcTableCols << " FROM " << dbcTableName;
//        ss << ") as b";
//        ss << " ON " << getDbcConnectCondition("", "b", {}, g_dbcTableMaps);;
//        ss << ") ";
//        ss << queryWrapper.getConditionStr();
//
//        std::string querySql = ss.str();
//
//        try
//        {
//            SQLite::Statement statement(*m_readDBPtr, querySql);
//            queryWrapper.bind(statement);
//            timer.setSql(statement.getExpandedSQL());
//            try
//            {
//                while (statement.executeStep())
//                {
//                    return statement.getColumn(0);
//                }
//            }
//            catch (const std::exception&)
//            {
//                STORAGE_LOG_ERROR("errorSql: {}, errorMsg: {}", statement.getExpandedSQL(), statement.getErrorMsg());
//            }
//        }
//        catch (const std::exception& e)
//        {
//            STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", querySql, e.what());
//        }
//        return ret;
//    }
//}
//
//std::vector<std::string> BaseStorage::queryMsgNamesOnline(const QueryWrapper& queryWrapper)
//{
//    SQL_EXEC_TIMER_GUARD(timer);
//    if (!m_dbcStorage)
//    {
//        return {};
//    }
//    else
//    {
//        std::string dbcTableName = m_dbcStorage->getAllChannleDBCName();
//        if (dbcTableName.empty())
//        {
//            return {};
//        }
//        std::stringstream ss;
//        ss << "SELECT DISTINCT message_name FROM " << m_dbOpt.getTableName() << " a ";
//        ss << "LEFT JOIN " << dbcTableName << " b ";
//        ss << "ON " << getDbcConnectCondition("a", "b", {}, {});
//        QueryWrapper wrapper = queryWrapper;
//        wrapper.addCondition({"trace_id", (int64_t)0}, QueryCondition::GE, 1);
//        ss << wrapper.getConditionStr();
//
//        std::string querySql = ss.str();
//
//        std::vector<std::string> messageNames;
//        try
//        {
//            SQLite::Statement statement(*m_readDBPtr, querySql);
//            wrapper.bind(statement);
//            timer.setSql(statement.getExpandedSQL());
//            try
//            {
//                while (statement.executeStep())
//                {
//                    SQLite::Column col = statement.getColumn(0);
//                    if (col.isNull())
//                    {
//                        messageNames.push_back("");
//                    }
//                    else
//                    {
//                        messageNames.push_back(col);
//                    }
//                }
//            }
//            catch (const std::exception&)
//            {
//                STORAGE_LOG_ERROR("errorSql: {}, errorMsg: {}", statement.getExpandedSQL(), statement.getErrorMsg());
//            }
//        }
//        catch (const std::exception& e)
//        {
//            STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", querySql, e.what());
//        }
//        return messageNames;
//    }
//}
//
//std::vector<std::string> BaseStorage::queryMsgNamesOffline(const std::vector<std::string>& tableNames, const QueryWrapper& queryWrapper)
//{
//    SQL_EXEC_TIMER_GUARD(timer);
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (!m_dbcStorage)
//    {
//        return {};
//    }
//    else
//    {
//        std::string dbcTableName = m_dbcStorage->getAllChannleDBCName();
//        if (dbcTableName.empty())
//        {
//            return {};
//        }
//        std::stringstream ss;
//        ss << "SELECT DISTINCT message_name FROM (";
//        if (tableNames.size() >= 2)
//        {
//            ss << "SELECT message_id, channel_id, message_name FROM (";
//            ss << "SELECT message_id, channel_id FROM " << tableNames[0];
//            for (size_t i = 1; i < tableNames.size(); i++)
//            {
//                ss << " UNION ALL";
//                ss << " SELECT message_id, channel_id FROM " << tableNames[i];
//            }
//            ss << ") as base";
//        }
//        else
//        {
//            ss << "SELECT message_id, channel_id, message_name FROM " << tableNames[0] << " as base";
//        }
//        ss << " LEFT JOIN ";
//        ss << "(";
//        ss << "SELECT " << g_dbcTableCols << " FROM " << dbcTableName;
//        ss << ") as b";
//        ss << " ON " << getDbcConnectCondition("base", "b", {}, g_dbcTableMaps);
//        ss << ")";
//        ss << " " << queryWrapper.getConditionStr();
//
//        std::string querySql = ss.str();
//
//        std::vector<std::string> messageNames;
//        try
//        {
//            SQLite::Statement statement(*m_readDBPtr, querySql);
//            queryWrapper.bind(statement);
//            timer.setSql(statement.getExpandedSQL());
//            try
//            {
//                while (statement.executeStep())
//                {
//                    messageNames.push_back(statement.getColumn(0));
//                }
//            }
//            catch (const std::exception&)
//            {
//                STORAGE_LOG_ERROR("errorSql: {}, errorMsg: {}", statement.getExpandedSQL(), statement.getErrorMsg());
//            }
//        }
//        catch (const std::exception& e)
//        {
//            STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", querySql, e.what());
//        }
//        return messageNames;
//    }
//}
//
//int64_t BaseStorage::countMsgNamesOffline(const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames)
//{
//    int ret = -1;
//    SQL_EXEC_TIMER_GUARD(timer);
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (!m_dbcStorage)
//    {
//        return ret;
//    }
//    else
//    {
//        std::string dbcTableName = m_dbcStorage->getAllChannleDBCName();
//        if (dbcTableName.empty())
//        {
//            return ret;
//        }
//        std::stringstream ss;
//        ss << "SELECT COUNT(1) FROM (";
//        if (tableNames.size() >= 2)
//        {
//            ss << "SELECT * FROM (";
//            ss << "SELECT * FROM " << tableNames[0];
//            for (size_t i = 1; i < tableNames.size(); i++)
//            {
//                ss << " UNION ALL";
//                ss << " SELECT * FROM " << tableNames[i];
//            }
//            ss << ") as base";
//        }
//        else
//        {
//            ss << "SELECT * FROM " << tableNames[0] << " as base";
//        }
//        ss << " LEFT JOIN ";
//        ss << "(";
//        ss << "SELECT " << g_dbcTableCols << " FROM " << dbcTableName;
//        ss << ") as b";
//        ss << " ON " << getDbcConnectCondition("base", "b", {}, g_dbcTableMaps);
//        ss << ") ";
//        ss << queryWrapper.getConditionStr();
//
//        std::string querySql = ss.str();
//
//        try
//        {
//            SQLite::Statement statement(*m_readDBPtr, querySql);
//            queryWrapper.bind(statement);
//            timer.setSql(statement.getExpandedSQL());
//            try
//            {
//                while (statement.executeStep())
//                {
//                    return statement.getColumn(0);
//                }
//            }
//            catch (const std::exception&)
//            {
//                STORAGE_LOG_ERROR("errorSql: {}, errorMsg: {}", statement.getExpandedSQL(), statement.getErrorMsg());
//            }
//        }
//        catch (const std::exception& e)
//        {
//            STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", querySql, e.what());
//        }
//        return ret;
//    }
//}
//
//std::vector<std::string> BaseStorage::queryDistinctColOffline(const ColumnMetaInfo& metaInfo, const std::vector<std::string>& tableNames, const QueryWrapper& queryWrapper)
//{
//    if (metaInfo.m_columnName == m_msgWrapper.m_messageName.m_columnName)
//    {
//        return queryMsgNamesOffline(tableNames, queryWrapper);
//    }
//
//    SQL_EXEC_TIMER_GUARD(timer);
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    std::vector<std::string> res;
//    if (tableNames.empty())
//    {
//        return res;
//    }
//    std::stringstream ss;
//
//    if (tableNames.size() >= 2)
//    {
//        ss << "SELECT DISTINCT " << metaInfo.m_columnName << " FROM (";
//        ss << "SELECT " << metaInfo.m_columnName << " FROM " << tableNames[0];
//        for (size_t i = 1; i < tableNames.size(); i++)
//        {
//            ss << " UNION ALL";
//            ss << " SELECT " << metaInfo.m_columnName << " FROM " << tableNames[i];
//        }
//        ss << ")";
//    }
//    else
//    {
//        ss << "SELECT DISTINCT " << metaInfo.m_columnName << " FROM " << tableNames[0];
//    }
//    ss << " " << queryWrapper.getConditionStr();
//
//    std::string sql = ss.str();
//    try
//    {
//        SQLite::Statement statement(*m_readDBPtr, sql);
//        queryWrapper.bind(statement);
//        timer.setSql(statement.getExpandedSQL());
//        while (statement.executeStep())
//        {
//            res.emplace_back(statement.getColumn(0).getString());
//        }
//    }
//    catch (const std::exception& e)
//    {
//        STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", sql, e.what());
//    }
//
//    return res;
//}
//
//std::vector<std::string> BaseStorage::queryDistinctCol(const ColumnMetaInfo& metaInfo, const QueryWrapper& queryWrapper)
//{
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (metaInfo.m_columnName == m_msgWrapper.m_messageName.m_columnName)
//    {
//        return queryMsgNamesOnline(queryWrapper);
//    }
//    else
//    {
//        return m_dbOpt.queryDistinctCol(metaInfo, queryWrapper);
//    }
//}
//
//int64_t BaseStorage::countEntitiesOfflineWithSingleTable(const QueryWrapper& queryWrapper, const std::vector<std::string>& tableNames)
//{
//    int64_t ret = -1;
//    SQL_EXEC_TIMER_GUARD(timer);
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (tableNames.empty())
//    {
//        return 0;
//    }
//    std::stringstream ss;
//    if (tableNames.size() >= 2)
//    {
//        ss << "SELECT count(1) as count FROM ( ";
//        ss << "SELECT trace_id FROM " << tableNames[0] << " " << queryWrapper.getConditionStr();
//        for (size_t i = 1; i < tableNames.size(); i++)
//        {
//            ss << " UNION ALL ";
//            ss << " SELECT trace_id FROM " << tableNames[i] << " " << queryWrapper.getConditionStr();
//        }
//        ss << ")";
//    }
//    else
//    {
//        ss << "SELECT count(1) as count FROM " << tableNames[0] << " " << queryWrapper.getConditionStr();
//    }
//    std::string sql = ss.str();
//    try
//    {
//        SQLite::Statement statement(*m_readDBPtr, sql);
//        int startIndex = 0;
//        for (size_t i = 0; i < tableNames.size(); i++)
//        {
//            startIndex += queryWrapper.bind(statement, startIndex);
//        }
//        timer.setSql(statement.getExpandedSQL());
//        while (statement.executeStep())
//        {
//            return statement.getColumn(0).getUInt();
//        }
//    }
//    catch (const std::exception& e)
//    {
//        STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", sql, e.what());
//    }
//    return ret;
//}
//
//int64_t BaseStorage::countEntitiesOnline(const QueryWrapper& queryWrapper)
//{
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (queryWrapper.contain(m_msgWrapper.m_messageName.m_columnName))
//    {
//        return countMsgNamesOnline(queryWrapper);
//    }
//    else
//    {
//        return m_dbOpt.countEntities(queryWrapper);
//    }
//}
//
//std::tuple<int64_t, int64_t, int64_t> BaseStorage::getRxAndTxCountAndMaxTraceId(const QueryWrapper& queryWrapper)
//{
//    std::lock_guard<std::mutex> lk(m_readMutex);
//    if (queryWrapper.contain(m_msgWrapper.m_messageName.m_columnName))
//    {
//        return getRxAndTxCountWithMsgName(queryWrapper);
//    }
//    else
//    {
//        return m_dbOpt.getRxAndTxCountAndMaxTraceId(queryWrapper);
//    }
//}
//
//std::tuple<int64_t, int64_t, int64_t> BaseStorage::getRxAndTxCountWithMsgName(const QueryWrapper& queryWrapper)
//{
//    int ret = 0;
//    SQL_EXEC_TIMER_GUARD(timer);
//    if (!m_dbcStorage)
//    {
//        return std::make_tuple<int64_t, int64_t, int64_t>(ret, ret, ret);
//    }
//    else
//    {
//        std::string dbcTableName = m_dbcStorage->getAllChannleDBCName();
//        if (dbcTableName.empty())
//        {
//            return std::make_tuple<int64_t, int64_t, int64_t>(ret, ret, ret);
//        }
//        std::stringstream ss;
//        ss << "SELECT count(CASE WHEN DIR=1 THEN 1 ELSE NULL END) as TX, count(1) as COUNT_NUM, ";
//        ss << " (SELECT max(trace_id) FROM " << m_dbOpt.getTableName() << ") as trace_id";
//        ss << " FROM (";
//        ss << "SELECT * FROM " << m_dbOpt.getTableName();
//        ss << " LEFT JOIN ";
//        ss << "(";
//        ss << "SELECT " << g_dbcTableCols << " FROM " << dbcTableName;
//        ss << ") as b";
//        ss << " ON " << getDbcConnectCondition("","b", {}, g_dbcTableMaps);
//        ss << ") ";
//        ss << queryWrapper.getConditionStr();
//
//        std::string querySql = ss.str();
//
//        try
//        {
//            SQLite::Statement statement(*m_readDBPtr, querySql);
//            queryWrapper.bind(statement);
//            timer.setSql(statement.getExpandedSQL());
//            try
//            {
//                while (statement.executeStep())
//                {
//                    int64_t txCount = statement.getColumn(0).getInt64();
//                    int64_t rxCount = statement.getColumn(1).getInt64() - txCount;
//                    int64_t maxTraceId = statement.getColumn(2).getInt64();
//                    return std::tuple<int64_t, int64_t, int64_t>(rxCount, txCount, maxTraceId);
//                }
//            }
//            catch (const std::exception&)
//            {
//                STORAGE_LOG_ERROR("errorSql: {}, errorMsg: {}", statement.getExpandedSQL(), statement.getErrorMsg());
//            }
//        }
//        catch (const std::exception& e)
//        {
//            STORAGE_LOG_ERROR("curSql: {}, errorMsg: {}", querySql, e.what());
//        }
//        return std::make_tuple<int64_t, int64_t, int64_t>(ret, ret, ret);
//    }
//}
