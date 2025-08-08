//
// Created by Zéro Cool on 20/07/2025.
//

#include "Physics.h"

auto Physics::isCollision(const sf::Vector2f &entity1_position, const sf::Vector2f &entity2_position, float entity1_radius, float entity2_radius) -> bool {
    const float radiusSquared = (entity1_radius + entity2_radius) * (entity1_radius + entity2_radius);

    const auto magnitudeSquared = (entity2_position - entity1_position).lengthSquared();

    return magnitudeSquared <= radiusSquared;
}
