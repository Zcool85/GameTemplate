//
// Created by Zéro Cool on 17/07/2025.
//

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <random>

#include "Scene.h"

class GameScene final
        : public Scene {
    ecs::impl::Handle player_entity_handle_{};
    sf::Font font_;
    sf::Text score_text_;
    // Seed for random number
    std::random_device random_device_;

    int score_;

    bool is_movements_system_active = true;
    bool is_lifespan_system_active = true;
    bool is_collision_system_active = true;
    bool is_enemy_spawning_system_active = true;
    bool is_gui_system_active = true;
    bool is_render_system_active = true;

    sf::Shader shader_;

    // Fonctions système
    auto sMovement(sf::Time delta_clock) -> void;

    auto sUserInput(sf::Window &window) -> void;

    auto sLifespan() -> void;

    auto sGUI() -> void;

    auto sEnemySpawner() -> void;

    auto sCollision() -> void;

    // Fonctions internes
    auto spawnPlayer() -> void;

    auto spawnEnemy() -> void;

    auto spawnBullet(ecs::impl::Handle player_handle, const sf::Vector2f &target) -> void;

    auto spawnSmallEnemies(ecs::EntityIndex enemy) -> void;

public:
    explicit GameScene(GameEngine &game);

    auto update(const sf::Time &delta_time, sf::RenderWindow &render_window) -> void override;

    auto render(sf::RenderTarget &render_target) -> void override;
};

#endif //GAME_SCENE_H
