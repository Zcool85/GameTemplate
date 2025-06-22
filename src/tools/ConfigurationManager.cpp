//
// Created by Zéro Cool on 21/06/2025.
//

#include "ConfigurationManager.h"

namespace tools {
    /**
     * Class de gestion des configurations
     * @param file_path Chemin vers le fichier de configuration à utiliser
     */
    ConfigurationManager::ConfigurationManager(const std::string &file_path)
        : file_path_(file_path) {
        configuration_.setOptions(libconfig::Config::OptionFsync
                                  | libconfig::Config::OptionSemicolonSeparators
                                  | libconfig::Config::OptionColonAssignmentForGroups
                                  | libconfig::Config::OptionOpenBraceOnSeparateLine);

        try {
            configuration_.readFile(file_path);
        } catch (const libconfig::FileIOException &file_io_exception) {
            std::cerr << "I/O error while reading file : " << file_io_exception.what() << std::endl;
        }
        catch (const libconfig::ParseException &parse_exception) {
            std::cerr << "Parse error at " << parse_exception.getFile() << ":" << parse_exception.getLine()
                    << " - " << parse_exception.getError() << std::endl;
            throw;
        }

        configuration_.lookupValue("window.title", window_settings_.title);
        configuration_.lookupValue("window.fullscreen", window_settings_.fullscreen);
        configuration_.lookupValue("window.width", window_settings_.width);
        configuration_.lookupValue("window.height", window_settings_.height);

        configuration_.lookupValue("graphics.anti_aliasing_level", graphics_settings_.anti_aliasing_level);
        configuration_.lookupValue("graphics.framerate_limit", graphics_settings_.framerate_limit);
        configuration_.lookupValue("graphics.vertical_sync_enabled", graphics_settings_.vertical_sync_enabled);
    }

    auto ConfigurationManager::getWindowSettings() -> WindowSettings & {
        return window_settings_;
    }

    auto ConfigurationManager::getGraphicsSettings() -> GraphicsSettings & {
        return graphics_settings_;
    }

    auto ConfigurationManager::Save() const -> bool {
        try {
            libconfig::Setting &root = configuration_.getRoot();
            libconfig::Setting &window_settings = getOrCreateGroup(root, "window");
            libconfig::Setting &graphics_settings = getOrCreateGroup(root, "graphics");

            setValue(window_settings, "title", window_settings_.title, libconfig::Setting::TypeString);
            setValue(window_settings, "fullscreen", window_settings_.fullscreen, libconfig::Setting::TypeBoolean);
            setValue(window_settings, "width", window_settings_.width, libconfig::Setting::TypeInt);
            setValue(window_settings, "height", window_settings_.height, libconfig::Setting::TypeInt);

            setValue(graphics_settings, "anti_aliasing_level", graphics_settings_.anti_aliasing_level,
                     libconfig::Setting::TypeInt);
            setValue(graphics_settings, "anti_aliasing_level", graphics_settings_.anti_aliasing_level,
                     libconfig::Setting::TypeInt);
            setValue(graphics_settings, "framerate_limit", graphics_settings_.framerate_limit,
                     libconfig::Setting::TypeInt);
            setValue(graphics_settings, "vertical_sync_enabled", graphics_settings_.vertical_sync_enabled,
                     libconfig::Setting::TypeBoolean);
        } catch ([[maybe_unused]] const libconfig::ConfigException &config_exception) {
            std::cerr << "Error while saving configuration file: " << config_exception.what() << std::endl;
            return false;
        }
        return true;
    }

    auto ConfigurationManager::getOrCreateGroup(libconfig::Setting &root,
                                                const std::string &key) -> libconfig::Setting & {
        return getOrCreateKey(root, key, libconfig::Setting::TypeGroup);
    }

    auto ConfigurationManager::getOrCreateKey(libconfig::Setting &root, const std::string &key,
                                              const libconfig::Setting::Type type) -> libconfig::Setting & {
        if (!root.exists(key)) {
            root.add(key, type);
        }

        return root[key];
    }

    template<typename T>
    auto ConfigurationManager::setValue(libconfig::Setting &root, const std::string &key, T &value,
                                        const libconfig::Setting::Type type) -> void {
        auto &setting = getOrCreateKey(root, key, type);
        setting = value;
    }
} // tools
