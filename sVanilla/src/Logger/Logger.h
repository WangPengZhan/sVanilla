#pragma once

#include <string>

class Logger
{
public:
    static Logger& GetInstance();
    void InitLog();

private:
    Logger();
    ~Logger();

    Logger(const Logger& other) = delete;
    Logger& operator=(const Logger& other) = delete;
    Logger(Logger&& other) = delete;
    Logger& operator=(Logger&& other) = delete;

    void RegisterLogger(const std::string& logName);
    void SetLog();

    static Logger m_logger;
};
