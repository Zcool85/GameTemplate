add_library(GameLib STATIC
        Game.cpp
        Game.h
        tools/ConfigurationManager.cpp
        tools/ConfigurationManager.h
        tools/WindowSettings.h
        tools/GraphicsSettings.h
        scenes/Scene.cpp
        scenes/Scene.h
        scenes/MainMenuScene.cpp
        scenes/MainMenuScene.h
        Types.h
        Action.h
        tools/FontSettings.h
        tools/PlayerSettings.h
        tools/EnemySettings.h
        tools/BulletSettings.h
)

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
