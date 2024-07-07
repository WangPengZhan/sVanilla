#include "Logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

namespace
{
constexpr int logFileMaxSize = 100 * 1024 * 1024;  // 20M
}

std::string Logger::m_dir;

Logger& Logger::getInstance()
{
    static Logger m_logger;
    return m_logger;
}

void Logger::setLogDir(const std::string& dir)
{
    m_dir = dir;
}

void Logger::initLog()
{
    registerLogger("MainWindow");
    registerLogger("Network");
    registerLogger("Aria2Net");
    registerLogger("FFmpeg");
    registerLogger("SQLite");
    registerLogger("ThreadPool");
}

Logger::Logger()
{
    setLog();
    initLog();
}

Logger::~Logger()
{
    spdlog::shutdown();
}

void Logger::registerLogger(const std::string& logName)
{
    spdlog::rotating_logger_mt<spdlog::async_factory>(logName, m_dir + "log/" + logName + ".log", logFileMaxSize, 100);
    // spdlog::create_async<spdlog::sinks::basic_file_sink_mt>(logName, "log/" + logName + ".log", logFileMaxSize, 100);
}

void Logger::setLog()
{
    spdlog::init_thread_pool(32768, 1);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%g:%#,%!][%l] : %v");
#ifdef _DEBUG
    spdlog::set_level(spdlog::level::trace);
#else
    spdlog::set_level(spdlog::level::info);
#endif
    spdlog::flush_on(spdlog::level::err);
    spdlog::flush_every(std::chrono::seconds(5));
}
