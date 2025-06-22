add_library(SrcLib STATIC
        Observable.cpp
        Observable.h
        Ecs.cpp
        Ecs.h
        Game.cpp
        Game.h
        tools/ConfigurationManager.cpp
        tools/ConfigurationManager.h
        tools/WindowSettings.h
        tools/GraphicsSettings.h
)

target_link_libraries(SrcLib PRIVATE
        SFML::Graphics
        SFML::Audio
        SFML::System
        SFML::Window
        SFML::Network
)
