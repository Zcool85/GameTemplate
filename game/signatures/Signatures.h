//
// Created by Zéro Cool on 12/07/2025.
//

#ifndef SIGNATURES_H
#define SIGNATURES_H

#include "EcsTypes.h"
#include "components/Components.h"
#include "tags/Tags.h"

using SPlayers = ecs::Signature<TPlayer, CTransform, CShape>;
using SBullets = ecs::Signature<TBullet, CTransform, CCollision, CShape, CLifespan>;
using SEnemies = ecs::Signature<TEnemy, CTransform, CCollision, CShape, CScore>;
using STransform = ecs::Signature<CTransform>;
using SRendering = ecs::Signature<CTransform, CShape>;
using SLifespan = ecs::Signature<CLifespan, CShape>;
using SSmallEnemies = ecs::Signature<TSmallEnemy, CTransform, CShape, CLifespan>;

using GameSignaturesList = ecs::SignatureList<
    SPlayers,
    SBullets,
    SEnemies,
    STransform,
    SRendering,
    SLifespan,
    SSmallEnemies
>;

#endif //SIGNATURES_H
