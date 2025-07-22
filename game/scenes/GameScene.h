//
// Created by Zéro Cool on 17/07/2025.
//

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "pch.h"

#include "Scene.h"

class GameScene final
        : public Scene {
    ecs::impl::Handle player_entity_handle_{};
    sf::Text score_text_;
    // Seed for random number
    std::random_device random_device_;
    sf::Sound shoot_sound_;
    sf::Sound death_sound_;
    sf::Sound game_over_sound_;
    sf::Sound kill_enemy_sound_;
    sf::Sound spawn_enemy_sound_;
    sf::Sound game_loop_sound_;

    int score_;
    int health_;

    bool is_movements_system_active = true;
    bool is_lifespan_system_active = true;
    bool is_collision_system_active = true;
    bool is_enemy_spawning_system_active = true;
    bool is_gui_system_active = true;
    bool is_render_system_active = true;

    sf::Shader shader_;

    // Fonctions système
    auto sMovement(sf::Time delta_clock) -> void;

    // TODO : sUserInput devrait être dans Scene.h. Ici, on devrait avoir un sDoAction(action)
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

    // TODO : A priori en privé ça serait mieux
    auto render(sf::RenderTarget &render_target) -> void override;
};

#endif //GAME_SCENE_H
