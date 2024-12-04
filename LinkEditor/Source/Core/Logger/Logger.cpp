#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

LINK_EDITOR_NAMESPACE_BEGIN

std::shared_ptr<spdlog::logger> Log::Logger = nullptr;

std::shared_ptr<spdlog::logger>& Log::GetLogger()
{
    if (!Logger)
    {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("MeshEditor.log", true));

        logSinks[0]->set_pattern("%^[%T] %n: %v%$");
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

        Logger = std::make_shared<spdlog::logger>("MeshEditor", begin(logSinks), end(logSinks));
        spdlog::register_logger(Logger);
        Logger->set_level(spdlog::level::trace);
        Logger->flush_on(spdlog::level::trace);
    }
    
    return Logger;
}

LINK_EDITOR_NAMESPACE_END
