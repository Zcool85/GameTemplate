add_library(GameLib STATIC
        GameEngine.cpp
        tools/ConfigurationManager.cpp
        scenes/Scene.cpp
        scenes/MainMenuScene.cpp
        scenes/GameScene.cpp
        Assets.cpp
        physics/Physics.cpp
)

# Local includes
target_include_directories(GameLib PRIVATE .)

# ECS Framework
target_include_directories(GameLib PRIVATE ../ecs/include)

target_link_libraries(GameLib PRIVATE
        SFML::Graphics
        SFML::Audio
        SFML::System
        SFML::Window
        SFML::Network
        ImGui-SFML::ImGui-SFML
)
