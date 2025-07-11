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

class Scene;


struct CTransform {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f scale;
    float angle{};
};

struct CCollision {
    float radius;
};

struct CScore {
    int score;
};

struct CShape {
    sf::CircleShape circle;
};

struct CLifespan {
    int lifespan{};
    int remaining{};
};

struct CInput {
    int up{};
    int down{};
    int left{};
    int right{};
    int shoot{};
    sf::Vector2i shoot_position{};
};

using GameComponentsList = ecs::ComponentList<
    CTransform,
    CCollision,
    CScore,
    CShape,
    CLifespan,
    CInput
>;

struct TPlayer {
};

struct TBullet {
};

struct TEnemy {
};

struct TLittleEnemy {
};

struct TSpawning {
};

using MyTagList = ecs::TagList<
    TPlayer,
    TBullet,
    TEnemy,
    TLittleEnemy,
    TSpawning
>;

using SPlayers = ecs::Signature<TPlayer, CTransform, CCollision, CShape, CInput>;
using SBullets = ecs::Signature<TBullet, CTransform, CCollision, CShape, CLifespan>;
using SEnemies = ecs::Signature<TEnemy, CTransform, CCollision, CShape, CScore>;
using STransform = ecs::Signature<CTransform>;
using SRendering = ecs::Signature<CTransform, CShape>;
using SLifespan = ecs::Signature<CLifespan>;
using SEnemiesSpawner = ecs::Signature<TSpawning, CLifespan>;

using MySignatureList = ecs::SignatureList<
    SPlayers,
    SBullets,
    SEnemies,
    STransform,
    SRendering,
    SLifespan,
    SEnemiesSpawner
>;

using MySettings = ecs::Settings<GameComponentsList, MyTagList, MySignatureList>;

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
    using EntityManager = ecs::Manager<MySettings>;

    tools::ConfigurationManager configuration_manager_;
    // Pour le moment on se limite à un plan 2D. Donc notre fenêtre sera un sf::RenderWindow et non un sf::Window
    sf::RenderWindow window_;
    std::map<SceneId, std::shared_ptr<Scene> > scenes_;
    bool running_;
    std::shared_ptr<Scene> current_scene_;
    sf::Clock delta_clock_;
    EntityManager entity_manager_;
    ecs::impl::Handle player_entity_handle_{};
    // Seed for random number
    std::random_device random_device_;

    /// @brief Traite les inputs (clavier, souris, joystick...)
    auto processInput() -> void;

    /// @brief Mise à jour des données
    auto update() -> void;

    /// @brief Mise à jour du rendu
    auto render() -> void;

    // Fonctions internes
    auto sMovement(sf::Time delta_clock) -> void;

    auto sUserInput() -> void;

    auto sEnemySpawner() -> void;

    auto sCollision() -> void;

    auto sRender() -> void;

    auto sLifespanKiller() -> void;

    auto spawnBullet(sf::Vector2f initial_position, sf::Vector2f velocity) -> void;

    auto spawnLittleEnemies(std::size_t number_of_little_enemies, sf::Vector2f initial_position, float initial_angle,
                            sf::Vector2f velocity, sf::Color color, std::size_t vertices, int enemy_score) -> void;

public:
    /// @brief Construit une instance du jeu
    /// @param configuration_file_path Chemin d'accès au fichier de configuration du jeu
    explicit Game(const std::string &configuration_file_path);

    /// @brief Fonction principale du jeu
    auto run() -> void;

    auto window() -> sf::RenderWindow &;

    auto spawnPlayer() -> void;

    auto addEnemySpawner() -> void;

    //auto changeScene<T>() -> void;
    //auto getAssets() -> Assets &;
    //auto sUserInput() -> void;
    //    Dans cette fonction on va faire du events_observable_.notify(Action{"UP"_hash, "START"_hash});
};

#endif //GAME_H
