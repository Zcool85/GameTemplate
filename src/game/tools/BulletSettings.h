//
// Created by Zéro Cool on 08/07/2025.
//

#ifndef BULLET_SETTINGS_H
#define BULLET_SETTINGS_H

namespace tools {
    struct BulletSettings {
        float shape_radius = 5.f;
        float collision_radius = 5.f;
        float speed = 5.f;
        int fill_color_r = 255;
        int fill_color_g = 255;
        int fill_color_b = 255;
        int outline_color_r = 255;
        int outline_color_g = 255;
        int outline_color_b = 255;
        float outline_thickness = 2.f;
        int shape_vertices = 8;
        int lifespan = 50;
    };
}

#endif //BULLET_SETTINGS_H
