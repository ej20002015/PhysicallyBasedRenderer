#pragma once
#include "PCH.h"

#include "spdlog/spdlog.h"

class Log
{
public:

	enum class LogLevel
	{
		TRACE = 0,
		INFO,
		WARN,
		ERR,
		CRITICAL
	};

public:

	static void init();

	static void setLogLevel(LogLevel logLevel);

	template<typename... Args>
	inline static void trace(Args&&... args) { SPDLOG_LOGGER_TRACE(s_logger, std::forward<Args>(args)...); }
	
	template<typename... Args>
	inline static void info(Args&&... args) { SPDLOG_LOGGER_INFO(s_logger, std::forward<Args>(args)...); }

	template<typename... Args>
	inline static void warn(Args&&... args) { SPDLOG_LOGGER_WARN(s_logger, std::forward<Args>(args)...); }

	template<typename... Args>
	inline static void error(Args&&... args) { SPDLOG_LOGGER_ERROR(s_logger, std::forward<Args>(args)...); }

	template<typename... Args>
	inline static void critical(Args&&... args) { SPDLOG_LOGGER_CRITICAL(s_logger, std::forward<Args>(args)...); }

private:

	static void setUpSinks(std::vector<spdlog::sink_ptr>& logSinks);

private:

	static Reference<spdlog::logger> s_logger;
};