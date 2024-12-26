#pragma once
#include <string>
#include <memory>

#include "LoggerConfig.h"

namespace spdlog
{
class logger;
}

class Logger
{
public:
    static Logger& getInstance();
    static void setLogDir(const std::string& dir);
    static constexpr char defaultName[] = "log";
    static void registerLogger(const std::string& logName);
    static std::shared_ptr<spdlog::logger> get(const std::string& logName);

private:
    Logger();
    ~Logger();

    Logger(const Logger& other) = delete;
    Logger& operator=(const Logger& other) = delete;
    Logger(Logger&& other) = delete;
    Logger& operator=(Logger&& other) = delete;

    void initLog();
    void setLog();

private:
    static std::string m_dir;
};
