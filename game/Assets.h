//
// Created by Zéro Cool on 19/07/2025.
//

#ifndef ASSETS_H
#define ASSETS_H

#include <SFML/Graphics.hpp>
#include <unordered_map>

#include "Types.h"
#include "tools/ConfigurationManager.h"

class Assets {
    tools::ConfigurationManager &configuration_manager_;
    std::unordered_map<FontId, sf::Font> fonts_;
    std::unordered_map<TextureId, sf::Texture> textures_;

public:
    explicit Assets(tools::ConfigurationManager &configuration_manager);

    [[nodiscard]] auto getFont(FontId font_id) -> sf::Font &;
    [[nodiscard]] auto getTexture(TextureId texture_id) -> sf::Texture &;
};

#endif //ASSETS_H
