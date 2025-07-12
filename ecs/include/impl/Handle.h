//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef ECS_IMPL_HANDLE_H
#define ECS_IMPL_HANDLE_H

#include "../EcsTypes.h"

namespace ecs::impl {

    // TODO : Ce type de donnée ne DOIT PAS être dans ecs::impl...
    struct Handle
    {
        // TODO : Faire en sorte que le contenu ne puisse pas être visible pour le développeur
        HandleDataIndex handleDataIndex;
        Counter counter;
    };

}

#endif //ECS_IMPL_HANDLE_H
