//
// Created by Zéro Cool on 21/06/2025.
//

#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include "pch.h"

#include "AssetsSettings.h"
#include "BulletSettings.h"
#include "EnemySettings.h"
#include "FontSettings.h"
#include "GraphicsSettings.h"
#include "PlayerSettings.h"
#include "WindowSettings.h"

namespace tools {
    class ConfigurationManager {
        std::string file_path_;
        std::string assets_file_path_;
        libconfig::Config configuration_;
        libconfig::Config assets_configuration_;
        WindowSettings window_settings_;
        GraphicsSettings graphics_settings_;
        FontSettings font_settings_;
        PlayerSettings player_settings_;
        EnemySettings enemy_settings_;
        BulletSettings bullet_settings_;
        AssetsSettings asset_settings_;

        static auto getOrCreateGroup(libconfig::Setting &root, const std::string &key) -> libconfig::Setting &;

        static auto getOrCreateKey(libconfig::Setting &root, const std::string &key,
                                   libconfig::Setting::Type type) -> libconfig::Setting &;

        template<typename T>
        static auto setValue(libconfig::Setting &root, const std::string &key, T &value,
                             libconfig::Setting::Type type) -> void;

    public:
        explicit ConfigurationManager(const std::string &file_path, const std::string &assets_file_path);

        auto getWindowSettings() -> WindowSettings &;

        auto getGraphicsSettings() -> GraphicsSettings &;

        auto getFontSettings() -> FontSettings &;

        auto getPlayerSettings() -> PlayerSettings &;

        auto getEnemySettings() -> EnemySettings &;

        auto getBulletSettings() -> BulletSettings &;

        auto getAssetsSettings() -> AssetsSettings &;

        [[nodiscard]] auto Save() const -> bool;
    };
} // tools

#endif //CONFIGURATION_MANAGER_H
