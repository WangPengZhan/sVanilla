#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/common.h>

#define SQLITE_LOG_TRACE(...) SPDLOG_LOGGER_CALL(spdlog::get("SQLite"), spdlog::level::trace, __VA_ARGS__)
#define SQLITE_LOG_DEBUG(...) SPDLOG_LOGGER_CALL(spdlog::get("SQLite"), spdlog::level::debug, __VA_ARGS__)
#define SQLITE_LOG_INFO(...) SPDLOG_LOGGER_CALL(spdlog::get("SQLite"), spdlog::level::info, __VA_ARGS__)
#define SQLITE_LOG_WARN(...) SPDLOG_LOGGER_CALL(spdlog::get("SQLite"), spdlog::level::warn, __VA_ARGS__)
#define SQLITE_LOG_ERROR(...) SPDLOG_LOGGER_CALL(spdlog::get("SQLite"), spdlog::level::err, __VA_ARGS__)
#define SQLITE_LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(spdlog::get("SQLite"), spdlog::level::critical, __VA_ARGS__)
#define SQLITE_LOG_OFF(...) SPDLOG_LOGGER_CALL(spdlog::get("SQLite"), spdlog::level::off, __VA_ARGS__)

