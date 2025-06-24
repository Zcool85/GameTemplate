//
// Created by Zéro Cool on 21/06/2025.
//

#ifndef GRAPHICSSETTINGS_H
#define GRAPHICSSETTINGS_H

namespace tools {
    struct GraphicsSettings {
        int depth_bits = 24;
        int stencil_bits = 8;
        int anti_aliasing_level = 0;
        int major_version = 2;
        int minor_version = 1;
        int attribute_flags = 0;
        int framerate_limit = 60;
        bool vertical_sync_enabled = false;
    };
} // tools

#endif //GRAPHICSSETTINGS_H
