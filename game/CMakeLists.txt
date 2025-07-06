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
)

target_link_libraries(GameLib PRIVATE
        SFML::Graphics
        SFML::Audio
        SFML::System
        SFML::Window
        SFML::Network
        ImGui-SFML::ImGui-SFML
)
