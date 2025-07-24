cmake_minimum_required(VERSION 3.22)

include(cmake/utils.cmake)

ecs_extract_version()

project(ecs VERSION ${ECS_VERSION} LANGUAGES CXX)
message(STATUS "Build ecs: ${ECS_VERSION}")

# Récupération du projet spdlog
include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.15.3
)
FetchContent_MakeAvailable(spdlog)

# Ajout des fichiers à compiler
set(ECS_SRCS src/Log.cpp)

set(ECS_HEADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

file(GLOB ECS_TOP_HEADERS "${ECS_HEADERS_DIR}/*.h")
file(GLOB ECS_IMPL_HEADERS "${ECS_HEADERS_DIR}/impl/*.h")
file(GLOB ECS_TOOLS_HEADERS "${ECS_HEADERS_DIR}/tools/*.h")
set(ECS_ALL_HEADERS ${ECS_TOP_HEADERS} ${ECS_IMPL_HEADERS} ${ECS_TOOLS_HEADERS})

add_library(ecs STATIC ${ECS_SRCS} ${ECS_ALL_HEADERS})

# PUBLIC pour que la compilation des projets utilisant la librairie ecs puissent compiler (car on référence des includes de spdlog)
target_link_libraries(ecs PUBLIC spdlog::spdlog)

target_include_directories(ecs PUBLIC ${ECS_HEADERS_DIR})

add_library(ecs::ecs ALIAS ecs)

add_executable(TestEcs src/main.cpp)
target_link_libraries(TestEcs PRIVATE ecs::ecs)
