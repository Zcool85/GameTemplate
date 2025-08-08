//
// Created by Zéro Cool on 21/06/2025.
//

#include "ConfigurationManager.h"

#include "libconfig.h++"

namespace tools {
    /**
     * Class de gestion des configurations
     * @param file_path Chemin vers le fichier de configuration à utiliser
     * @param assets_file_path Chemin vers le fichier de configuration des assets
     */
    ConfigurationManager::ConfigurationManager(const std::string &file_path, const std::string &assets_file_path)
        : file_path_(file_path), assets_file_path_(assets_file_path)
    {
        configuration_.setOptions(libconfig::Config::OptionFsync
                                  | libconfig::Config::OptionSemicolonSeparators
                                  | libconfig::Config::OptionColonAssignmentForGroups
                                  | libconfig::Config::OptionOpenBraceOnSeparateLine);

        try
        {
            configuration_.readFile(file_path);
        }
        catch (const libconfig::FileIOException &file_io_exception)
        {
            std::cerr << "I/O error while reading file : " << file_io_exception.what() << std::endl;
        }
        catch (const libconfig::ParseException &parse_exception)
        {
            std::cerr << "Parse error at " << parse_exception.getFile() << ":" << parse_exception.getLine()
                    << " - " << parse_exception.getError() << std::endl;
            throw;
        }

        configuration_.lookupValue("window.title", window_settings_.title);
        configuration_.lookupValue("window.fullscreen", window_settings_.fullscreen);
        configuration_.lookupValue("window.width", window_settings_.width);
        configuration_.lookupValue("window.height", window_settings_.height);

        configuration_.lookupValue("graphics.depth_bits", graphics_settings_.depth_bits);
        configuration_.lookupValue("graphics.stencil_bits", graphics_settings_.stencil_bits);
        configuration_.lookupValue("graphics.anti_aliasing_level", graphics_settings_.anti_aliasing_level);
        configuration_.lookupValue("graphics.major_version", graphics_settings_.major_version);
        configuration_.lookupValue("graphics.minor_version", graphics_settings_.minor_version);
        configuration_.lookupValue("graphics.attribute_flags", graphics_settings_.attribute_flags);
        configuration_.lookupValue("graphics.framerate_limit", graphics_settings_.framerate_limit);
        configuration_.lookupValue("graphics.vertical_sync_enabled", graphics_settings_.vertical_sync_enabled);

        // TODO : Code saving values
        configuration_.lookupValue("font.size", font_settings_.size);
        configuration_.lookupValue("font.color.r", font_settings_.color_r);
        configuration_.lookupValue("font.color.g", font_settings_.color_g);
        configuration_.lookupValue("font.color.b", font_settings_.color_b);

        // TODO : Code saving values
        configuration_.lookupValue("player.shape_radius", player_settings_.shape_radius);
        configuration_.lookupValue("player.collision_radius", player_settings_.collision_radius);
        configuration_.lookupValue("player.speed", player_settings_.speed);
        configuration_.lookupValue("player.fill_color.r", player_settings_.fill_color_r);
        configuration_.lookupValue("player.fill_color.g", player_settings_.fill_color_g);
        configuration_.lookupValue("player.fill_color.b", player_settings_.fill_color_b);
        configuration_.lookupValue("player.outline_color.r", player_settings_.outline_color_r);
        configuration_.lookupValue("player.outline_color.g", player_settings_.outline_color_g);
        configuration_.lookupValue("player.outline_color.b", player_settings_.outline_color_b);
        configuration_.lookupValue("player.outline_thickness", player_settings_.outline_thickness);
        configuration_.lookupValue("player.shape_vertices", player_settings_.shape_vertices);

        // TODO : Code saving values
        configuration_.lookupValue("enemy.shape_radius", enemy_settings_.shape_radius);
        configuration_.lookupValue("enemy.collision_radius", enemy_settings_.collision_radius);
        configuration_.lookupValue("enemy.min_speed", enemy_settings_.min_speed);
        configuration_.lookupValue("enemy.max_speed", enemy_settings_.max_speed);
        configuration_.lookupValue("enemy.outline_color.r", enemy_settings_.outline_color_r);
        configuration_.lookupValue("enemy.outline_color.g", enemy_settings_.outline_color_g);
        configuration_.lookupValue("enemy.outline_color.b", enemy_settings_.outline_color_b);
        configuration_.lookupValue("enemy.outline_thickness", enemy_settings_.outline_thickness);
        configuration_.lookupValue("enemy.min_vertices", enemy_settings_.min_vertices);
        configuration_.lookupValue("enemy.max_vertices", enemy_settings_.max_vertices);
        configuration_.lookupValue("enemy.small_lifespan", enemy_settings_.small_lifespan);
        configuration_.lookupValue("enemy.spawn_interval", enemy_settings_.spawn_interval);

        // TODO : Code saving values
        configuration_.lookupValue("bullet.shape_radius", bullet_settings_.shape_radius);
        configuration_.lookupValue("bullet.collision_radius", bullet_settings_.collision_radius);
        configuration_.lookupValue("bullet.speed", bullet_settings_.speed);
        configuration_.lookupValue("bullet.fill_color.r", bullet_settings_.fill_color_r);
        configuration_.lookupValue("bullet.fill_color.g", bullet_settings_.fill_color_g);
        configuration_.lookupValue("bullet.fill_color.b", bullet_settings_.fill_color_b);
        configuration_.lookupValue("bullet.outline_color.r", bullet_settings_.outline_color_r);
        configuration_.lookupValue("bullet.outline_color.g", bullet_settings_.outline_color_g);
        configuration_.lookupValue("bullet.outline_color.b", bullet_settings_.outline_color_b);
        configuration_.lookupValue("bullet.outline_thickness", bullet_settings_.outline_thickness);
        configuration_.lookupValue("bullet.shape_vertices", bullet_settings_.shape_vertices);
        configuration_.lookupValue("bullet.lifespan", bullet_settings_.lifespan);

        // TODO : Faire mieux... Je duplique dans tous les sens...
        assets_configuration_.setOptions(libconfig::Config::OptionFsync
                                         | libconfig::Config::OptionSemicolonSeparators
                                         | libconfig::Config::OptionColonAssignmentForGroups
                                         | libconfig::Config::OptionOpenBraceOnSeparateLine);

        try
        {
            assets_configuration_.readFile(assets_file_path);
        }
        catch (const libconfig::FileIOException &file_io_exception)
        {
            std::cerr << "I/O error while reading file : " << file_io_exception.what() << std::endl;
        }
        catch (const libconfig::ParseException &parse_exception)
        {
            std::cerr << "Parse error at " << parse_exception.getFile() << ":" << parse_exception.getLine()
                    << " - " << parse_exception.getError() << std::endl;
            throw;
        }

        for (const auto &asset: assets_configuration_.lookup("fonts"))
        {
            AssetSettings asset_settings;
            asset.lookupValue("key", asset_settings.key);
            asset.lookupValue("file", asset_settings.file);

            asset_settings_.fonts_settings[FontId{asset_settings.key}] = asset_settings;
        }

        for (const auto &asset: assets_configuration_.lookup("textures"))
        {
            AssetSettings asset_settings;
            asset.lookupValue("key", asset_settings.key);
            asset.lookupValue("file", asset_settings.file);

            asset_settings_.textures_settings[TextureId{asset_settings.key}] = asset_settings;
        }

        for (const auto &asset: assets_configuration_.lookup("sounds"))
        {
            AssetSettings asset_settings;
            asset.lookupValue("key", asset_settings.key);
            asset.lookupValue("file", asset_settings.file);

            asset_settings_.sounds_settings[SoundId{asset_settings.key}] = asset_settings;
        }

        for (const auto &asset: assets_configuration_.lookup("musics"))
        {
            AssetSettings asset_settings;
            asset.lookupValue("key", asset_settings.key);
            asset.lookupValue("file", asset_settings.file);

            asset_settings_.musics_settings[MusicId{asset_settings.key}] = asset_settings;
        }
    }

    auto ConfigurationManager::getWindowSettings() -> WindowSettings &
    {
        return window_settings_;
    }

    auto ConfigurationManager::getGraphicsSettings() -> GraphicsSettings &
    {
        return graphics_settings_;
    }

    auto ConfigurationManager::getFontSettings() -> FontSettings &
    {
        return font_settings_;
    }

    auto ConfigurationManager::getPlayerSettings() -> PlayerSettings &
    {
        return player_settings_;
    }

    auto ConfigurationManager::getEnemySettings() -> EnemySettings &
    {
        return enemy_settings_;
    }

    auto ConfigurationManager::getBulletSettings() -> BulletSettings &
    {
        return bullet_settings_;
    }

    auto ConfigurationManager::getAssetsSettings() -> AssetsSettings &
    {
        return asset_settings_;
    }

    auto ConfigurationManager::Save() const -> bool
    {
        try
        {
            libconfig::Setting &root = configuration_.getRoot();
            libconfig::Setting &window_settings = getOrCreateGroup(root, "window");
            libconfig::Setting &graphics_settings = getOrCreateGroup(root, "graphics");

            setValue(window_settings, "title", window_settings_.title, libconfig::Setting::TypeString);
            setValue(window_settings, "fullscreen", window_settings_.fullscreen, libconfig::Setting::TypeBoolean);
            setValue(window_settings, "width", window_settings_.width, libconfig::Setting::TypeInt);
            setValue(window_settings, "height", window_settings_.height, libconfig::Setting::TypeInt);

            setValue(graphics_settings, "depth_bits", graphics_settings_.depth_bits,
                     libconfig::Setting::TypeInt);
            setValue(graphics_settings, "stencil_bits", graphics_settings_.stencil_bits,
                     libconfig::Setting::TypeInt);
            setValue(graphics_settings, "anti_aliasing_level", graphics_settings_.anti_aliasing_level,
                     libconfig::Setting::TypeInt);
            setValue(graphics_settings, "major_version", graphics_settings_.major_version,
                     libconfig::Setting::TypeInt);
            setValue(graphics_settings, "minor_version", graphics_settings_.minor_version,
                     libconfig::Setting::TypeInt);
            setValue(graphics_settings, "attribute_flags", graphics_settings_.attribute_flags,
                     libconfig::Setting::TypeInt);
            setValue(graphics_settings, "framerate_limit", graphics_settings_.framerate_limit,
                     libconfig::Setting::TypeInt);
            setValue(graphics_settings, "vertical_sync_enabled", graphics_settings_.vertical_sync_enabled,
                     libconfig::Setting::TypeBoolean);
        }
        catch ([[maybe_unused]] const libconfig::ConfigException &config_exception)
        {
            std::cerr << "Error while saving configuration file: " << config_exception.what() << std::endl;
            return false;
        }
        return true;
    }

    auto ConfigurationManager::getOrCreateGroup(libconfig::Setting &root,
                                                const std::string &key) -> libconfig::Setting &
    {
        return getOrCreateKey(root, key, libconfig::Setting::TypeGroup);
    }

    auto ConfigurationManager::getOrCreateKey(libconfig::Setting &root, const std::string &key,
                                              const libconfig::Setting::Type type) -> libconfig::Setting &
    {
        if (!root.exists(key))
        {
            root.add(key, type);
        }

        return root[key];
    }

    template<typename T>
    auto ConfigurationManager::setValue(libconfig::Setting &root, const std::string &key, T &value,
                                        const libconfig::Setting::Type type) -> void
    {
        auto &setting = getOrCreateKey(root, key, type);
        setting = value;
    }
} // tools
