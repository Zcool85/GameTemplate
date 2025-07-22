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

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


###########################################################################
#
#    Définition des options de compilation
#
###########################################################################

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

#### SFML

find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics Network Audio)

# Check for imgui-sfml CMakeLists.txt file
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml/CMakeLists.txt")
    message("${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml/CMakeLists.txt")
    message(FATAL_ERROR "Please initialize submodules using:\n  git submodule update --init --recursive")
else ()
    message(STATUS "Found imgui-sfml in ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml")
endif ()

#### ImGui for SFML
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_ENABLE_WARNINGS ON)
#set(IMGUI_SFML_DISABLE_OBSOLETE_FUNCTIONS ON)
set(IMGUI_SFML_IMGUI_DEMO OFF)
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
add_subdirectory(external/imgui-sfml)

#### libconfig

find_package(Config++ REQUIRED)

###########################################################################
#
#    Binaire de test pour le Framework ECS
#
###########################################################################

add_executable(TestEcs ecs/src/main.cpp)

# ECS Framework
target_include_directories(TestEcs PRIVATE ecs/include)

###########################################################################
#
#    Binaire du jeu
#
###########################################################################

add_executable(${PROJECT_NAME} main.cpp)

# ECS Framework
target_include_directories(${PROJECT_NAME} PRIVATE ecs/include)

# Add CMakeLists.txt for the game sources
add_subdirectory(game)
# Add src directory where we can find header files for the project
target_include_directories(${PROJECT_NAME} PRIVATE game)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        config++
        argparse
        GameLib
        SFML::Graphics SFML::Audio SFML::System SFML::Window SFML::Network
        ImGui-SFML::ImGui-SFML
        # TODO : Voir si à garder...
        ${OPENGL_LIBRARIES}
        "-framework OpenGL"
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
