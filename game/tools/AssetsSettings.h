//
// Created by Zéro Cool on 20/07/2025.
//

#ifndef ASSETS_SETTINGS_H
#define ASSETS_SETTINGS_H

#include "pch.h"

#include "Types.h"

namespace tools {
    struct AssetSettings {
        std::string key;
        std::string file;
    };

    struct AssetsSettings {
        std::unordered_map<FontId, AssetSettings> fonts_settings{};
        std::unordered_map<TextureId, AssetSettings> textures_settings{};
    };
}

#endif //ASSETS_SETTINGS_H
