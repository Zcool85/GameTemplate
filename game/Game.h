//
// Created by Zéro Cool on 19/06/2025.

#ifndef GAME_H
#define GAME_H

#include <map>
#include <random>
#include <SFML/Graphics.hpp>

#include "scenes/Scene.h"
#include "tools/ConfigurationManager.h"
#include "Types.h"
#include "Ecs.h"
#include "GameSettings.h"

class Scene;

/// @brief Classe principale du jeu
///
/// Cette classe embarque toutes les données globales du jeu comme :
/// - sf::Window
/// - Assets
/// - Scenes
///
/// Elle s'occupe des fonctionnalités haut niveaux comme :
///
/// - Changer de scene
/// - Gérer les inputs
///
/// La fonction run() est la boucle principale du jeu
///
/// L'instance de cette classe est passée en paramètre du constructeur
/// de chaque classe Scene.
class Game {
    using EntityManager = ecs::Manager<GameSettings>;

    tools::ConfigurationManager configuration_manager_;
    // Pour le moment on se limite à un plan 2D. Donc notre fenêtre sera un sf::RenderWindow et non un sf::Window
    sf::RenderWindow window_;
    std::map<SceneId, std::shared_ptr<Scene> > scenes_;
    bool running_;
    std::shared_ptr<Scene> current_scene_;
    sf::Clock delta_clock_;
    EntityManager entity_manager_;
    ecs::impl::Handle player_entity_handle_{};
    sf::Font font_;
    sf::Text score_text_;
    // Seed for random number
    std::random_device random_device_;

    int score_;
    int current_frame_;
    bool pause_;

    /// @brief Mise à jour des données
    auto update() -> void;

    // Fonctions système
    auto sMovement(sf::Time delta_clock) -> void;
    auto sUserInput() -> void;

    auto sLifespan() -> void;

    auto sRender() -> void;

    auto sGUI() -> void;

    auto sEnemySpawner() -> void;
    auto sCollision() -> void;

    // Fonctions internes
    auto spawnPlayer() -> void;

    auto spawnBullet(ecs::impl::Handle player_handle, const sf::Vector2f &target) -> void;

    auto spawnSmallEnemies(ecs::EntityIndex enemy) -> void;

public:
    /// @brief Construit une instance du jeu
    /// @param configuration_file_path Chemin d'accès au fichier de configuration du jeu
    explicit Game(const std::string &configuration_file_path);

    /// @brief Fonction principale du jeu
    auto run() -> void;

    auto window() -> sf::RenderWindow &;

    //auto changeScene<T>() -> void;
    //auto getAssets() -> Assets &;
    //auto sUserInput() -> void;
    //    Dans cette fonction on va faire du events_observable_.notify(Action{"UP"_hash, "START"_hash});
};

#endif //GAME_H
