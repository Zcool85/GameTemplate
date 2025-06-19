add_library(SrcLib
        Observable.cpp
        Observable.h
        Ecs.cpp
        Ecs.h
        Game.cpp
        Game.h
)

target_link_libraries(SrcLib
        PRIVATE
        SFML::Graphics SFML::Audio SFML::System SFML::Window SFML::Network
)
