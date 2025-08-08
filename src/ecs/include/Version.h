//
// Created by Zéro Cool on 24/07/2025.
//

#ifndef VERSION_H
#define VERSION_H

#define ECS_VER_MAJOR 0
#define ECS_VER_MINOR 1
#define ECS_VER_PATCH 0

#define ECS_TO_VERSION(major, minor, patch) (major * 10000 + minor * 100 + patch)
#define ECS_VERSION ECS_TO_VERSION(ECS_VER_MAJOR, ECS_VER_MINOR, ECS_VER_PATCH)

#endif //VERSION_H
