#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/common.h>

#define MAINWINDOW_LOG_TRACE(...)    SPDLOG_LOGGER_CALL(spdlog::get("MainWindow"), spdlog::level::trace, __VA_ARGS__)
#define MAINWINDOW_LOG_DEBUG(...)    SPDLOG_LOGGER_CALL(spdlog::get("MainWindow"), spdlog::level::debug, __VA_ARGS__)
#define MAINWINDOW_LOG_INFO(...)     SPDLOG_LOGGER_CALL(spdlog::get("MainWindow"), spdlog::level::info, __VA_ARGS__)
#define MAINWINDOW_LOG_WARN(...)     SPDLOG_LOGGER_CALL(spdlog::get("MainWindow"), spdlog::level::warn, __VA_ARGS__)
#define MAINWINDOW_LOG_ERROR(...)    SPDLOG_LOGGER_CALL(spdlog::get("MainWindow"), spdlog::level::err, __VA_ARGS__)
#define MAINWINDOW_LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(spdlog::get("MainWindow"), spdlog::level::critical, __VA_ARGS__)
#define MAINWINDOW_LOG_OFF(...)      SPDLOG_LOGGER_CALL(spdlog::get("MainWindow"), spdlog::level::off, __VA_ARGS__)
