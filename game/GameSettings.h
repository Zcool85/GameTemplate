//
// Created by Zéro Cool on 12/07/2025.
//

#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include "EcsTypes.h"
#include "components/Components.h"
#include "signatures/Signatures.h"
#include "tags/Tags.h"

using GameSettings = ecs::Settings<GameComponentsList, GameTagsList, GameSignaturesList>;

#endif //GAMESETTINGS_H
