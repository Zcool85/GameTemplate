cmake_minimum_required(VERSION 3.31)
project(GameTemplate
        LANGUAGES CXX
        VERSION 1.0
)

# C++23
set(CMAKE_CXX_STANDARD 23)

# SFML
find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics Network Audio)

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
        SrcLib
        SFML::Graphics SFML::Audio SFML::System SFML::Window SFML::Network
        ImGui-SFML::ImGui-SFML
)
