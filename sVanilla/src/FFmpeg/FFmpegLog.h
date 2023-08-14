#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/common.h>

#define FFMPEG_LOG_TRACE(...) SPDLOG_LOGGER_CALL(spdlog::get("FFmpeg"), spdlog::level::trace, __VA_ARGS__)
#define FFMPEG_LOG_DEBUG(...) SPDLOG_LOGGER_CALL(spdlog::get("FFmpeg"), spdlog::level::debug, __VA_ARGS__)
#define FFMPEG_LOG_INFO(...) SPDLOG_LOGGER_CALL(spdlog::get("FFmpeg"), spdlog::level::info, __VA_ARGS__)
#define FFMPEG_LOG_WARN(...) SPDLOG_LOGGER_CALL(spdlog::get("FFmpeg"), spdlog::level::warn, __VA_ARGS__)
#define FFMPEG_LOG_ERROR(...) SPDLOG_LOGGER_CALL(spdlog::get("FFmpeg"), spdlog::level::err, __VA_ARGS__)
#define FFMPEG_LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(spdlog::get("FFmpeg"), spdlog::level::critical, __VA_ARGS__)
#define FFMPEG_LOG_OFF(...) SPDLOG_LOGGER_CALL(spdlog::get("FFmpeg"), spdlog::level::off, __VA_ARGS__)
