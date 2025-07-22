//
// Created by Zéro Cool on 20/07/2025.
//

#ifndef PHYSICS_H
#define PHYSICS_H

#include "pch.h"

struct Physics {
    static auto isCollision(const sf::Vector2f &entity1_position, const sf::Vector2f &entity2_position,
                            float entity1_radius, float entity2_radius) -> bool;
};

#endif //PHYSICS_H
