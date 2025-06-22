//
// Created by Zéro Cool on 21/06/2025.
//

#ifndef GRAPHICSSETTINGS_H
#define GRAPHICSSETTINGS_H

namespace tools {
    struct GraphicsSettings {
        int anti_aliasing_level = 0;
        int framerate_limit = 60;
        bool vertical_sync_enabled = false;
    };
} // tools

#endif //GRAPHICSSETTINGS_H
