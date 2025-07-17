//
// Created by Zéro Cool on 12/07/2025.
//

#ifndef CTRANSFORM_H
#define CTRANSFORM_H

#include <SFML/Graphics.hpp>

struct CTransform {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float angle{};
};


#endif //CTRANSFORM_H
