//
// Created by Zéro Cool on 21/06/2025.
//

#ifndef WINDOWSETTINGS_H
#define WINDOWSETTINGS_H

#include <string>

namespace tools {

struct WindowSettings {
    std::string title = "Default";
    bool fullscreen = false;
    int width = 800;
    int height = 600;
};

} // tools

#endif //WINDOWSETTINGS_H
