//
// Created by Zéro Cool on 22/06/2025.
//

#ifndef MAINMENUSCENE_H
#define MAINMENUSCENE_H

#include "Scene.h"

/// @brief Classe représentant la scène du menu principal du jeu
///
/// Cette classe stock les données spécifiques à la scène :
/// - Level
/// - Pointeur vers le joueur
/// - Configuration...
///
/// Tous les System spécifiques à la classe sont accessibles ici
///
///
class MainMenuScene final
        : public Scene {
public:
    explicit MainMenuScene(GameEngine &game);

    auto update(const sf::Time &delta_time, sf::RenderWindow &render_window) -> void override;

    auto render(sf::RenderTarget &render_target) -> void override;
};



#endif //MAINMENUSCENE_H
