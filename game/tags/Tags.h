//
// Created by Zéro Cool on 12/07/2025.
//

#ifndef TAGS_H
#define TAGS_H

#include "Ecs.h"

struct TPlayer {};
struct TBullet {};
struct TEnemy {};
struct TSmallEnemy {};
struct TSpawning {};

using GameTagsList = ecs::TagList<
    TPlayer,
    TBullet,
    TEnemy,
    TSmallEnemy,
    TSpawning
>;

#endif //TAGS_H
