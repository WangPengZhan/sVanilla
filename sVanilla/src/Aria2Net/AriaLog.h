#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/common.h>

#define ARIA_LOG_TRACE(...) SPDLOG_LOGGER_CALL(spdlog::get("Aria"), spdlog::level::trace, __VA_ARGS__)
#define ARIA_LOG_DEBUG(...) SPDLOG_LOGGER_CALL(spdlog::get("Aria"), spdlog::level::debug, __VA_ARGS__)
#define ARIA_LOG_INFO(...) SPDLOG_LOGGER_CALL(spdlog::get("Aria"), spdlog::level::info, __VA_ARGS__)
#define ARIA_LOG_WARN(...) SPDLOG_LOGGER_CALL(spdlog::get("Aria"), spdlog::level::warn, __VA_ARGS__)
#define ARIA_LOG_ERROR(...) SPDLOG_LOGGER_CALL(spdlog::get("Aria"), spdlog::level::err, __VA_ARGS__)
#define ARIA_LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(spdlog::get("Aria"), spdlog::level::critical, __VA_ARGS__)
#define ARIA_LOG_OFF(...) SPDLOG_LOGGER_CALL(spdlog::get("Aria"), spdlog::level::off, __VA_ARGS__)
