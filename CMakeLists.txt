cmake_minimum_required(VERSION 3.31)
project(GameTemplate
        LANGUAGES CXX
        VERSION 1.0
)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")

# Détection du compilateur et de l'architecture
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(NATIVE_OPTIMIZATION_FLAGS "-march=native")

    # Optimisations supplémentaires pour les compilateurs GCC/Clang
    set(PERFORMANCE_FLAGS "-O3 -funroll-loops -ffast-math")

elseif (MSVC)
    # Équivalent MSVC
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64")
        set(NATIVE_OPTIMIZATION_FLAGS "/arch:AVX2")
    endif ()
    set(PERFORMANCE_FLAGS "/O2 /Ob2")
endif ()

# argparse
# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

if (APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif ()

# SFML
find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics Network Audio)

# libconfig
find_package(Config++ REQUIRED)

# Check for imgui-sfml CMakeLists.txt file
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml/CMakeLists.txt")
    message("${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml/CMakeLists.txt")
    message(FATAL_ERROR "Please initialize submodules using:\n  git submodule update --init --recursive")
else()
    message(STATUS "Found imgui-sfml in ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml")
endif()

# ImGui for SFML
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_ENABLE_WARNINGS ON)
#set(IMGUI_SFML_DISABLE_OBSOLETE_FUNCTIONS ON)
set(IMGUI_SFML_IMGUI_DEMO OFF)
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
add_subdirectory(external/imgui-sfml)

# Create our executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Add another CMakeLists.txt
add_subdirectory(src)
# Add src directory where we can find header files for the project
target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        config++
        argparse
        SrcLib
        SFML::Graphics SFML::Audio SFML::System SFML::Window SFML::Network
        ImGui-SFML::ImGui-SFML
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

message(STATUS "Compilateur: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Type de build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Standard C++: ${CMAKE_CXX_STANDARD}")
