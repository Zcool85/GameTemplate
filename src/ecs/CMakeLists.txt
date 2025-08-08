cmake_minimum_required(VERSION 3.12...3.28)

include(cmake/utils.cmake)

ecs_extract_version()

project(EcsLib
        VERSION ${ECS_VERSION}
        DESCRIPTION "C++ header-only library for ECS (Entity-Component-System)"
        LANGUAGES CXX)

message(STATUS "Ecs v${ECS_VERSION}")

# Standard C++ requis
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# === CRÉATION DE LA CIBLE HEADER-ONLY ===
add_library(${PROJECT_NAME} INTERFACE)

# Alias pour faciliter l'usage avec des namespaces
add_library(ecs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Répertoires d'inclusion
target_include_directories(${PROJECT_NAME}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Fonctionnalités C++ requises (optionnel)
target_compile_features(${PROJECT_NAME}
        INTERFACE
        cxx_std_20
)

add_executable(TestEcs "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(TestEcs PRIVATE ecs::${PROJECT_NAME})
