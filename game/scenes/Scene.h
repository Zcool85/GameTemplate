//
// Created by Zéro Cool on 22/06/2025.
//

#ifndef SCENE_H
#define SCENE_H

#include "Ecs.h"
#include "GameSettings.h"

#include "../GameEngine.h"
#include "../Types.h"

class GameEngine;

/// @brief Classe de base pour une scène du jeu
///
/// Stock les données commune de la scène :
/// - Entities
/// - Frame count
/// - Actions
class Scene {
protected:
    GameEngine &game_;

    using EntityManager = ecs::Manager<GameSettings>;
    EntityManager entity_manager_;
    int current_frame_;
    // Map sf::Keyboard key to ActionNameId to perform
    std::map<sf::Keyboard::Key, ActionNameId> action_map_;
    bool paused_;
    bool ended_;

    //auto registerAction(sf::Keyboard::Key inputKey, ActionNameId action) -> void;

public:
    explicit Scene(GameEngine &game);

    virtual ~Scene() = default;

    virtual auto update(const sf::Time &delta_time, sf::RenderWindow &render_window) -> void = 0;

    virtual auto render(sf::RenderTarget &render_target) -> void = 0;

    [[nodiscard]] auto hasEnded() const -> bool;
};

#endif //SCENE_H
