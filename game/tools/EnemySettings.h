//
// Created by Zéro Cool on 08/07/2025.
//

#ifndef ENEMY_SETTINGS_H
#define ENEMY_SETTINGS_H

namespace tools {
    struct EnemySettings {
        float shape_radius = 32.f;
        float collision_radius = 32.f;
        float min_speed = 5.f;
        float max_speed = 15.f;
        int outline_color_r = 255;
        int outline_color_g = 255;
        int outline_color_b = 255;
        float outline_thickness = 2.f;
        int min_vertices = 3;
        int max_vertices = 7;
        int small_lifespan = 7;
        int spawn_interval = 7;
    };
}

#endif //ENEMY_SETTINGS_H
