#pragma once

#include <string>

class Logger
{
public:
    static Logger& getInstance();
    static void setLogDir(const std::string& dir);
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

    static std::string m_dir;
};
