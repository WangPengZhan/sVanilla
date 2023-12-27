#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/common.h>

#define THEADPOOL_LOG_TRACE(...) SPDLOG_LOGGER_CALL(spdlog::get("TheadPool"), spdlog::level::trace, __VA_ARGS__)
#define THEADPOOL_LOG_DEBUG(...) SPDLOG_LOGGER_CALL(spdlog::get("TheadPool"), spdlog::level::debug, __VA_ARGS__)
#define THEADPOOL_LOG_INFO(...) SPDLOG_LOGGER_CALL(spdlog::get("TheadPool"), spdlog::level::info, __VA_ARGS__)
#define THEADPOOL_LOG_WARN(...) SPDLOG_LOGGER_CALL(spdlog::get("TheadPool"), spdlog::level::warn, __VA_ARGS__)
#define THEADPOOL_LOG_ERROR(...) SPDLOG_LOGGER_CALL(spdlog::get("TheadPool"), spdlog::level::err, __VA_ARGS__)
#define THEADPOOL_LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(spdlog::get("TheadPool"), spdlog::level::critical, __VA_ARGS__)
#define THEADPOOL_LOG_OFF(...) SPDLOG_LOGGER_CALL(spdlog::get("TheadPool"), spdlog::level::off, __VA_ARGS__)
