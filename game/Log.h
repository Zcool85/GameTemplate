//
// Created by Zéro Cool on 23/07/2025.
//

#ifndef LOG_H
#define LOG_H

// TODO : A mettre dans ecs.
// TODO : Faire un CMakeLists.txt for ECS afin d'être utilisable par un add_subdirectory(ecs)

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class Log {
public:
    static void Init();

    static std::shared_ptr<spdlog::logger> &GetEcsLogger() { return core_logger_; }
    static std::shared_ptr<spdlog::logger> &GetAppLogger() { return app_logger_; }

private:
    static std::shared_ptr<spdlog::logger> core_logger_;
    static std::shared_ptr<spdlog::logger> app_logger_;
};

#define ECS_CORE_TRACE(...)    ::Log::GetEcsLogger()->trace(__VA_ARGS__)
#define ECS_CORE_DEBUG(...)    ::Log::GetEcsLogger()->debug(__VA_ARGS__)
#define ECS_CORE_INFO(...)     ::Log::GetEcsLogger()->info(__VA_ARGS__)
#define ECS_CORE_WARN(...)     ::Log::GetEcsLogger()->warn(__VA_ARGS__)
#define ECS_CORE_ERROR(...)    ::Log::GetEcsLogger()->error(__VA_ARGS__)
#define ECS_CORE_CRITICAL(...) ::Log::GetEcsLogger()->critical(__VA_ARGS__)

#define ECS_TRACE(...)         ::Log::GetAppLogger()->trace(__VA_ARGS__)
#define ECS_DEBUG(...)         ::Log::GetAppLogger()->debug(__VA_ARGS__)
#define ECS_INFO(...)          ::Log::GetAppLogger()->info(__VA_ARGS__)
#define ECS_WARN(...)          ::Log::GetAppLogger()->warn(__VA_ARGS__)
#define ECS_ERROR(...)         ::Log::GetAppLogger()->error(__VA_ARGS__)
#define ECS_CRITICAL(...)      ::Log::GetAppLogger()->critical(__VA_ARGS__)

#endif //LOG_H
