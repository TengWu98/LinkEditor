#pragma once

#include "pch.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

MESH_EDITOR_NAMESPACE_BEGIN

class Log
{
public:
    static std::shared_ptr<spdlog::logger>& GetLogger();
    
private:
    static std::shared_ptr<spdlog::logger> Logger;
};

#define LOG_TRACE(...)         Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)          Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)          Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)         Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)      Log::GetLogger()->critical(__VA_ARGS__)

MESH_EDITOR_NAMESPACE_END