#pragma once
#include "Logger/LogUtil.h"

#define CLINET_LOG_TRACE(...)    SPDLOG_LOGGER_CALL(spdlog::get("Client"), spdlog::level::trace, __VA_ARGS__)
#define CLINET_LOG_DEBUG(...)    SPDLOG_LOGGER_CALL(spdlog::get("Client"), spdlog::level::debug, __VA_ARGS__)
#define CLINET_LOG_INFO(...)     SPDLOG_LOGGER_CALL(spdlog::get("Client"), spdlog::level::info, __VA_ARGS__)
#define CLINET_LOG_WARN(...)     SPDLOG_LOGGER_CALL(spdlog::get("Client"), spdlog::level::warn, __VA_ARGS__)
#define CLINET_LOG_ERROR(...)    SPDLOG_LOGGER_CALL(spdlog::get("Client"), spdlog::level::err, __VA_ARGS__)
#define CLINET_LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(spdlog::get("Client"), spdlog::level::critical, __VA_ARGS__)
#define CLINET_LOG_OFF(...)      SPDLOG_LOGGER_CALL(spdlog::get("Client"), spdlog::level::off, __VA_ARGS__)

namespace
{
std::string concatenate_strings(const std::string& first, const std::string& second)
{
    return "[" + first + "] " + second;
}
}  // namespace

#define FIRST_ARG(arg1, ...)    arg1
#define REST_ARGS(arg1, ...)    __VA_ARGS__

#define MLogT(ModuleName, ...)  CLINET_LOG_TRACE(concatenate_strings(ModuleName, FIRST_ARG(__VA_ARGS__)), REST_ARGS(__VA_ARGS__));
#define MLogD(ModuleName, ...)  CLINET_LOG_DEBUG(concatenate_strings(ModuleName, FIRST_ARG(__VA_ARGS__)), REST_ARGS(__VA_ARGS__));
#define MLogI(ModuleName, ...)  CLINET_LOG_INFO(concatenate_strings(ModuleName, FIRST_ARG(__VA_ARGS__)), REST_ARGS(__VA_ARGS__));
#define MLogW(ModuleName, ...)  CLINET_LOG_WARN(concatenate_strings(ModuleName, FIRST_ARG(__VA_ARGS__)), REST_ARGS(__VA_ARGS__));
#define MLogE(ModuleName, ...)  CLINET_LOG_ERROR(concatenate_strings(ModuleName, FIRST_ARG(__VA_ARGS__)), REST_ARGS(__VA_ARGS__));
#define MLogC(ModuleName, ...)  CLINET_LOG_OFF(concatenate_strings(ModuleName, FIRST_ARG(__VA_ARGS__)), REST_ARGS(__VA_ARGS__));

#define CLogLimitTime()         LogLimitTime(CLINET_LOG_INFO)
#define CLogTimer()             LogTimer(CLINET_LOG_INFO)
#define CLog_Unique_Timer()     Log_Unique_Timer(KEY_Start, CLINET_LOG_INFO)
#define CLog_Unique_Timer_END() Log_Unique_Timer_END(KEY_Start)
#define CLogFuncT()             LogFuncT(CLINET_LOG_INFO)
