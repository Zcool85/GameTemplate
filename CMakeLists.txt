cmake_minimum_required(VERSION 3.31)

###########################################################################
#
#    Définition du projet
#
###########################################################################

project(GameTemplate
        LANGUAGES CXX
        VERSION 1.0
        DESCRIPTION "Game project template"
)

###########################################################################
#
#    Définition des options de compilation
#
###########################################################################

SET(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE STRING "C++ compiler" FORCE)

# C++23 (CLion limit to C++2b)
set(CMAKE_CXX_STANDARD 23)

# Détection du compilateur et de l'architecture
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(NATIVE_OPTIMIZATION_FLAGS "-march=native")

    # Optimisations supplémentaires pour les compilateurs GCC/Clang
    set(PERFORMANCE_FLAGS "-O3 -funroll-loops -ffast-math")

    add_compile_options(-Werror -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wattributes)
elseif (MSVC)
    # Équivalent MSVC
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64")
        set(NATIVE_OPTIMIZATION_FLAGS "/arch:AVX2")
    endif ()
    set(PERFORMANCE_FLAGS "/O2 /Ob2")

    add_compile_options("/Wall")
endif ()

# TODO : Voir quoi garder
if (APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif ()
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
    find_package(OpenGL REQUIRED)
endif ()

# Generate file "${CMAKE_BINARY_DIR}/compile_commands.json"
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


###########################################################################
#
#    Récupération des éléments tiers
#
###########################################################################

#### argparse

# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

#### game

add_subdirectory(game)

###########################################################################
#
#    Binaire du jeu
#
###########################################################################

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        argparse
        GameLib::GameLib
        SharedPCH
        ecs::ecs
)

# Copy config directory to output directory
add_custom_target(copy-config-files ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/config
        ${CMAKE_CURRENT_BINARY_DIR}/config
)
# Add copy-file target to this project dependencies
add_dependencies(${PROJECT_NAME} copy-config-files)

# Copy assets directory to output directory
add_custom_target(copy-assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
)
# Add copy-file target to this project dependencies
add_dependencies(${PROJECT_NAME} copy-assets)

# Copy data directory to output directory
add_custom_target(copy-data ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        ${CMAKE_CURRENT_BINARY_DIR}/data
)
# Add copy-file target to this project dependencies
add_dependencies(${PROJECT_NAME} copy-data)

message(STATUS "Compilateur: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Type de build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Standard C++: ${CMAKE_CXX_STANDARD}")
