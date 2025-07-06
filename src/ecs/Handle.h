//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef HANDLE_H
#define HANDLE_H

#include "EcsTypes.h"

namespace ecs::impl {

    struct Handle
    {
        // TODO : Faire en sorte que le contenu ne puisse pas être visible pour le développeur
        HandleDataIndex handleDataIndex;
        Counter counter;
    };

}

#endif //HANDLE_H
