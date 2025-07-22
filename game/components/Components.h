//
// Created by Zéro Cool on 12/07/2025.
//

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "pch.h"

#include "EcsTypes.h"
#include "CTransform.h"
#include "CCollision.h"
#include "CScore.h"
#include "CShape.h"
#include "CLifespan.h"
#include "CInput.h"

using GameComponentsList = ecs::ComponentList<
    CTransform,
    CCollision,
    CScore,
    CShape,
    CLifespan,
    CInput
>;

#endif //COMPONENTS_H
