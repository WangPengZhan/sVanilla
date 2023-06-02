#pragma once
#pragma execution_character_set("utf-8")

#include <string>

class Logger
{
public:
	static Logger& GetInstance();
	void InitLog();

private:
	Logger();
	~Logger();

	// µ¥Àý½ûÖ¹
	Logger(const Logger& other) = delete;
	Logger& operator=(const Logger& other) = delete;
    Logger(Logger&& other) = delete;
    Logger& operator=(Logger&& other) = delete;

	// ×¢²áloggerºÍÉèÖÃ
	void RegisterLogger(const std::string& logName);
	void SetLog();

	static Logger m_logger;
};
