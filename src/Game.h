//
// Created by Zéro Cool on 19/06/2025.
//

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>

#include "tools/ConfigurationManager.h"

class Game {
    tools::ConfigurationManager configuration_manager_;
    sf::RenderWindow window_;
    void update();
    void render();
public:
    explicit Game(const std::string& configuration_file_path);
    void run();
};



#endif //GAME_H
