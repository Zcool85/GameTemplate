add_library(SrcLib STATIC
        tools/Observable.h
        Ecs.h
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
        tools/strong_typedef.h
        tools/for_each_type.h
        tools/type_sequence.h
        ecs/Settings.h
        ecs/SignatureBitsets.h
        ecs/SignatureBitsetsStorage.h
)

target_link_libraries(SrcLib PRIVATE
        SFML::Graphics
        SFML::Audio
        SFML::System
        SFML::Window
        SFML::Network
        ImGui-SFML::ImGui-SFML
)
