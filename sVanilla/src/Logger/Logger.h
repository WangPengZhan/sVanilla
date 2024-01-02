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

public:
    Logger(const Logger& other) = delete;
    Logger& operator=(const Logger& other) = delete;
    Logger(Logger&& other) = delete;
    Logger& operator=(Logger&& other) = delete;

private:
    void registerLogger(const std::string& logName);
    void setLog();

    static Logger m_logger;
};
