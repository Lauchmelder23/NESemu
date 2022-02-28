#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#define FORCE_NO_DEBUG_LOG

class Log
{
public:
	static void Init();
	inline static std::shared_ptr<spdlog::logger> GetCoreLogger() { return coreLogger; }
	inline static std::shared_ptr<spdlog::logger> GetDebugLogger() { return debugLogger; }

private:
	static std::shared_ptr<spdlog::logger> coreLogger;
	static std::shared_ptr<spdlog::logger> debugLogger;
};

#define LOG_CORE_TRACE(...)      ::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)       ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)       ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)      ::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...)      ::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define LOG_DEBUG_TRACE(...)     ::Log::GetDebugLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG_INFO(...)      ::Log::GetDebugLogger()->info(__VA_ARGS__)
#define LOG_DEBUG_WARN(...)      ::Log::GetDebugLogger()->warn(__VA_ARGS__)
#define LOG_DEBUG_ERROR(...)     ::Log::GetDebugLogger()->error(__VA_ARGS__)
#define LOG_DEBUG_FATAL(...)     ::Log::GetDebugLogger()->critical(__VA_ARGS__)