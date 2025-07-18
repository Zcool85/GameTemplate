//
// Created by Zéro Cool on 21/06/2025.
//

#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include <iostream>
#include <libconfig.h++>

#include "BulletSettings.h"
#include "EnemySettings.h"
#include "FontSettings.h"
#include "GraphicsSettings.h"
#include "PlayerSettings.h"
#include "WindowSettings.h"

namespace tools {
    class ConfigurationManager {
        std::string file_path_;
        libconfig::Config configuration_;
        WindowSettings window_settings_;
        GraphicsSettings graphics_settings_;
        FontSettings font_settings_;
        PlayerSettings player_settings_;
        EnemySettings enemy_settings_;
        BulletSettings bullet_settings_;

        static auto getOrCreateGroup(libconfig::Setting &root, const std::string &key) -> libconfig::Setting &;

        static auto getOrCreateKey(libconfig::Setting &root, const std::string &key,
                                   libconfig::Setting::Type type) -> libconfig::Setting &;

        template<typename T>
        static auto setValue(libconfig::Setting &root, const std::string &key, T &value,
                             libconfig::Setting::Type type) -> void;

    public:
        explicit ConfigurationManager(const std::string &file_path);

        auto getWindowSettings() -> WindowSettings &;

        auto getGraphicsSettings() -> GraphicsSettings &;

        auto getFontSettings() -> FontSettings &;

        auto getPlayerSettings() -> PlayerSettings &;

        auto getEnemySettings() -> EnemySettings &;

        auto getBulletSettings() -> BulletSettings &;

        [[nodiscard]] auto Save() const -> bool;
    };
} // tools

#endif //CONFIGURATIONMANAGER_H
