//
// Created by Zéro Cool on 12/07/2025.
//

#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

#include "EcsTypes.h"
#include "components/Components.h"
#include "signatures/Signatures.h"
#include "tags/Tags.h"

using GameSettings = ecs::Settings<GameComponentsList, GameTagsList, GameSignaturesList>;

#endif //GAME_SETTINGS_H
