cmake_minimum_required(VERSION 3.14)

include(cmake/utils.cmake)

log_extract_version()

project(LogsLib
        LANGUAGES CXX
        VERSION 1.0
        DESCRIPTION "Logging system"
)

message(STATUS "Log v${LOG_VERSION}")

# Load spdlog dependency
include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.15.3
)
FetchContent_MakeAvailable(spdlog)

add_library(${PROJECT_NAME} STATIC src/Log.cpp)

target_include_directories(${PROJECT_NAME}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# TODO : PUBLIC pour que la compilation des projets utilisant la librairie ecs puissent compiler (car on référence des includes de spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

add_library(ecs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
