add_library(GameLib STATIC
        Game.cpp
        tools/ConfigurationManager.cpp
        scenes/Scene.cpp
        scenes/MainMenuScene.cpp
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
