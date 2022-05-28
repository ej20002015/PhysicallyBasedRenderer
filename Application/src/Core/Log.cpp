#include "PCH.h"
#include "Log.h"

#include <iostream>
#include <vector>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

Reference<spdlog::logger> Log::s_logger;

void Log::init()
{
	try
	{
		// Set up sinks for logger (where the log should output to)

		std::vector<spdlog::sink_ptr> logSinks;
		setUpSinks(logSinks);

		s_logger = createReference<spdlog::logger>("PBR", logSinks.begin(), logSinks.end());
		spdlog::register_logger(s_logger);

		// Set the default logging level to the maximum (trace)

		s_logger->set_level(SPDLOG_ACTIVE_LEVEL_SOURCE);
		s_logger->flush_on(SPDLOG_ACTIVE_LEVEL_SOURCE);
	}
	catch (const spdlog::spdlog_ex& e)
	{
		std::cout << "Error when initialising logging: " << e.what() << std::endl;
	}
}

void Log::setLogLevel(LogLevel logLevel)
{
	switch (logLevel)
	{
	case Log::LogLevel::TRACE:
		s_logger->set_level(spdlog::level::trace);
		break;
	case Log::LogLevel::INFO:
		s_logger->set_level(spdlog::level::info);
		break;
	case Log::LogLevel::WARN:
		s_logger->set_level(spdlog::level::warn);
		break;
	case Log::LogLevel::ERR:
		s_logger->set_level(spdlog::level::err);
		break;
	case Log::LogLevel::CRITICAL:
		s_logger->set_level(spdlog::level::critical);
		break;
	default:
		break;
	}
}

void Log::setUpSinks(std::vector<spdlog::sink_ptr>& logSinks)
{
	logSinks.emplace_back(createReference<spdlog::sinks::stdout_color_sink_mt>());
	logSinks.emplace_back(createReference<spdlog::sinks::basic_file_sink_mt>("PBR.log", true));

	// Set logger format for each sink (file sink format contains no color information)

	logSinks[0]->set_pattern("%^[%T:%e] [%l] %n: %v%$");  // For console
	logSinks[1]->set_pattern("[%T:%e] [%l] %n: %v");      // For file
}
