//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef ECS_IMP_HANDLE_DATA_H
#define ECS_IMP_HANDLE_DATA_H

#include "../EcsTypes.h"

namespace ecs::impl {

    // TODO : Pour moi, c'est du internal
    struct HandleData
    {
        EntityIndex entityIndex;
        Counter counter;
    };

}

#endif //ECS_IMP_HANDLE_DATA_H
