//
// Created by Zéro Cool on 23/07/2025.
//

#include "Log.h"

namespace ecs {
    std::shared_ptr<spdlog::logger> Log::core_logger_;
    std::shared_ptr<spdlog::logger> Log::app_logger_;

    void Log::Init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        core_logger_ = spdlog::stdout_color_mt("ECS");
        core_logger_->set_level(spdlog::level::trace);
        app_logger_ = spdlog::stdout_color_mt("APP");
        app_logger_->set_level(spdlog::level::trace);
    }
}
