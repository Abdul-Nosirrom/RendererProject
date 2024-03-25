/*#pragma once
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#include <spdlog/spdlog.h>

class Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
};

#define LOG_TRACE(...)   ::App::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)   ::App::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)   ::App::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)   ::App::Log::GetCoreLogger()->error(__VA_ARGS__)
*/