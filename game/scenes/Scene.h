//
// Created by Zéro Cool on 22/06/2025.
//

#ifndef SCENE_H
#define SCENE_H

#include "../Game.h"
#include "../Types.h"

class Game;

/// @brief Classe de base pour une scène du jeu
///
/// Stock les données commune de la scène :
/// - Entities
/// - Frame count
/// - Actions
class Scene {
protected:
    const Game &game_;
    // Map sf::Keyboard key to ActionNameId to perform
    std::map<sf::Keyboard::Key, ActionNameId> action_map_;
    int frame_;
    bool paused_;

    auto registerAction(sf::Keyboard::Key inputKey, ActionNameId action) -> void;

public:
    explicit Scene(const Game &game);

    virtual ~Scene() = default;

    virtual auto update(const sf::Time &delta_time) -> void = 0;

    virtual auto render() -> void = 0;

    // Prefix 's' is for System
    //virtual auto sDoAction(action) -> void = 0;


    // Call derived scene's update a given number of times
    //auto simulate(int) -> void;
    //auto doAction(action) -> void;
};


#endif //SCENE_H
