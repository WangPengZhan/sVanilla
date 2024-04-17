#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/common.h>

#define BILIBILI_LOG_TRACE(...)    SPDLOG_LOGGER_CALL(spdlog::get("BiliBili"), spdlog::level::trace, __VA_ARGS__)
#define BILIBILI_LOG_DEBUG(...)    SPDLOG_LOGGER_CALL(spdlog::get("BiliBili"), spdlog::level::debug, __VA_ARGS__)
#define BILIBILI_LOG_INFO(...)     SPDLOG_LOGGER_CALL(spdlog::get("BiliBili"), spdlog::level::info, __VA_ARGS__)
#define BILIBILI_LOG_WARN(...)     SPDLOG_LOGGER_CALL(spdlog::get("BiliBili"), spdlog::level::warn, __VA_ARGS__)
#define BILIBILI_LOG_ERROR(...)    SPDLOG_LOGGER_CALL(spdlog::get("BiliBili"), spdlog::level::err, __VA_ARGS__)
#define BILIBILI_LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(spdlog::get("BiliBili"), spdlog::level::critical, __VA_ARGS__)
#define BILIBILI_LOG_OFF(...)      SPDLOG_LOGGER_CALL(spdlog::get("BiliBili"), spdlog::level::off, __VA_ARGS__)
