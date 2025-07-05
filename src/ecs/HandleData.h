//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef HANDLE_DATA_H
#define HANDLE_DATA_H

#include "EcsTypes.h"

namespace ecs::impl {

    struct HandleData
    {
        EntityIndex entityIndex;
        Counter counter;
    };

}

#endif //HANDLE_DATA_H
