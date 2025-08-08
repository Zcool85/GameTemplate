# Ajout d'un Pre-Compiled Header
add_library(SharedPCH INTERFACE)
target_precompile_headers(SharedPCH INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)
target_link_libraries(SharedPCH
        INTERFACE
        SFML::System
        ImGui-SFML::ImGui-SFML
)


#### OpenGL

find_package(OpenGL REQUIRED)
if (APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif ()

#### libconfig

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(Config++ REQUIRED)

#### SFML

find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics Network Audio)

#### ImGui for SFML

# Check for imgui-sfml CMakeLists.txt file
if (NOT EXISTS "${CMAKE_SOURCE_DIR}/external/imgui-sfml/CMakeLists.txt")
    message("${CMAKE_SOURCE_DIR}/external/imgui-sfml/CMakeLists.txt")
    message(FATAL_ERROR "Please initialize submodules using:\n  git submodule update --init --recursive")
else ()
    message(STATUS "Found imgui-sfml in ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml")
endif ()

set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_ENABLE_WARNINGS ON)
#set(IMGUI_SFML_DISABLE_OBSOLETE_FUNCTIONS ON)
set(IMGUI_SFML_IMGUI_DEMO OFF)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")
#add_subdirectory(${CMAKE_SOURCE_DIR}/external/imgui-sfml ${CMAKE_CURRENT_BINARY_DIR}/imgui-sfml_build)

FetchContent_Declare(
        imgui-sfml
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/imgui-sfml
)
FetchContent_MakeAvailable(imgui-sfml)

add_library(GameLib STATIC
        GameEngine.cpp
        tools/ConfigurationManager.cpp
        scenes/Scene.cpp
        scenes/MainMenuScene.cpp
        scenes/GameScene.cpp
        Assets.cpp
        physics/Physics.cpp
)

target_link_libraries(GameLib
        PRIVATE
        config++
        SharedPCH
        SFML::System
        SFML::Graphics
        SFML::Audio
        SFML::Window
        SFML::Network
        ImGui-SFML::ImGui-SFML
        ecs::EcsLib
        ecs::LogsLib
)

target_include_directories(GameLib PUBLIC .)

add_library(GameLib::GameLib ALIAS GameLib)

