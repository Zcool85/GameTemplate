//
// Created by Zéro Cool on 08/07/2025.
//

#ifndef FONTSETTINGS_H
#define FONTSETTINGS_H
#include <string>

namespace tools {
    struct FontSettings {
        std::string file;
        int size = 12;
        int color_r = 200;
        int color_g = 200;
        int color_b = 200;
    };
} // tools

#endif //FONTSETTINGS_H
