#include "Log.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Log::coreLogger;
std::shared_ptr<spdlog::logger> Log::debugLogger;

void Log::Init()
{
	debugLogger = spdlog::stdout_color_mt("DEBUG");
	debugLogger->set_pattern("%^[%T] %n: %v%$");
	debugLogger->set_level(spdlog::level::trace);

	coreLogger = spdlog::stdout_color_mt("CORE");
	coreLogger->set_pattern("%^[%T] %n: %v%$");
	coreLogger->set_level(spdlog::level::info);

	LOG_CORE_INFO("Logger initialized");
}
