//
// Created by Zéro Cool on 24/07/2025.
//

#ifndef VERSION_H
#define VERSION_H

#define LOG_VER_MAJOR 0
#define LOG_VER_MINOR 1
#define LOG_VER_PATCH 0

#define LOG_TO_VERSION(major, minor, patch) (major * 10000 + minor * 100 + patch)
#define LOG_VERSION LOG_TO_VERSION(LOG_VER_MAJOR, LOG_VER_MINOR, LOG_VER_PATCH)

#endif //VERSION_H
