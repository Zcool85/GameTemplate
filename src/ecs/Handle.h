//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef HANDLE_H
#define HANDLE_H

#include "EcsTypes.h"

namespace ecs::impl {

    struct Handle
    {
        HandleDataIndex handleDataIndex;
        Counter counter;
    };

}

#endif //HANDLE_H
