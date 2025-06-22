cmake_minimum_required(VERSION 3.31)
project(GameTemplate
        LANGUAGES CXX
        VERSION 1.0
)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# C++23
set(CMAKE_CXX_STANDARD 23)

# argparse
# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

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
