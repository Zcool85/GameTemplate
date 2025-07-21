//
// Created by Zéro Cool on 19/07/2025.
//

#include "Assets.h"

Assets::Assets(tools::ConfigurationManager &configuration_manager)
    : configuration_manager_{configuration_manager} {
    const auto &assets_settings = configuration_manager_.getAssetsSettings();

    for (const auto &[id, asset_setting]: assets_settings.fonts_settings) {
        fonts_[id] = sf::Font(asset_setting.file);
    }

    for (const auto &[id, asset_setting]: assets_settings.textures_settings) {
        textures_[id] = sf::Texture(asset_setting.file);
    }
}

auto Assets::getFont(const FontId font_id) -> sf::Font & {
    return this->fonts_[font_id];
}

auto Assets::getTexture(TextureId texture_id) -> sf::Texture & {
    return this->textures_[texture_id];
}
