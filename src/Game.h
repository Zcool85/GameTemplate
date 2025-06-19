//
// Created by Zéro Cool on 19/06/2025.
//

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>

class Game {
    sf::RenderWindow window;
    void update();
    void render();
public:
    Game();
    void run();
};



#endif //GAME_H
