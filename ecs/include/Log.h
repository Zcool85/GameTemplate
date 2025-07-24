//
// Created by Zéro Cool on 23/07/2025.
//

#ifndef LOG_H
#define LOG_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace ecs {
    class Log {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger> &GetEcsLogger() { return core_logger_; }
        static std::shared_ptr<spdlog::logger> &GetAppLogger() { return app_logger_; }

    private:
        static std::shared_ptr<spdlog::logger> core_logger_;
        static std::shared_ptr<spdlog::logger> app_logger_;
    };
}

#define ECS_CORE_TRACE(...)    ::ecs::Log::GetEcsLogger()->trace(__VA_ARGS__)
#define ECS_CORE_DEBUG(...)    ::ecs::Log::GetEcsLogger()->debug(__VA_ARGS__)
#define ECS_CORE_INFO(...)     ::ecs::Log::GetEcsLogger()->info(__VA_ARGS__)
#define ECS_CORE_WARN(...)     ::ecs::Log::GetEcsLogger()->warn(__VA_ARGS__)
#define ECS_CORE_ERROR(...)    ::ecs::Log::GetEcsLogger()->error(__VA_ARGS__)
#define ECS_CORE_CRITICAL(...) ::ecs::Log::GetEcsLogger()->critical(__VA_ARGS__)

#define ECS_TRACE(...)         ::ecs::Log::GetAppLogger()->trace(__VA_ARGS__)
#define ECS_DEBUG(...)         ::ecs::Log::GetAppLogger()->debug(__VA_ARGS__)
#define ECS_INFO(...)          ::ecs::Log::GetAppLogger()->info(__VA_ARGS__)
#define ECS_WARN(...)          ::ecs::Log::GetAppLogger()->warn(__VA_ARGS__)
#define ECS_ERROR(...)         ::ecs::Log::GetAppLogger()->error(__VA_ARGS__)
#define ECS_CRITICAL(...)      ::ecs::Log::GetAppLogger()->critical(__VA_ARGS__)

#endif //LOG_H
