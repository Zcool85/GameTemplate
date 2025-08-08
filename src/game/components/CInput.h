//
// Created by Zéro Cool on 12/07/2025.
//

#ifndef CINPUT_H
#define CINPUT_H

#include <SFML/Graphics.hpp>

struct CInput {
    int up{};
    int down{};
    int left{};
    int right{};
    int shoot{};
    sf::Vector2i shoot_position{};
};

#endif //CINPUT_H
