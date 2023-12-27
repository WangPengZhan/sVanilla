#pragma once

#include <string>

class Logger
{
public:
    static Logger& getInstance();
    void initLog();

private:
    Logger();
    ~Logger();

    Logger(const Logger& other) = delete;
    Logger& operator=(const Logger& other) = delete;
    Logger(Logger&& other) = delete;
    Logger& operator=(Logger&& other) = delete;

    void registerLogger(const std::string& logName);
    void setLog();

    static Logger m_logger;
};
