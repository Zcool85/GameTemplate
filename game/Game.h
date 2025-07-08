//
// Created by Zéro Cool on 19/06/2025.

#ifndef GAME_H
#define GAME_H

#include <map>
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
    int lifespan;
    int remaining;
};

struct CInput {
    int up;
    int down;
    int left;
    int right;
    int shoot;
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

struct TSmallEnemy {
};

struct TSpawning {
};

using MyTagList = ecs::TagList<
    TPlayer,
    TBullet,
    TEnemy,
    TSmallEnemy,
    TSpawning
>;

using SPlayers = ecs::Signature<TPlayer, CTransform, CCollision, CShape, CInput>;
using SBullets = ecs::Signature<TBullet, CTransform, CCollision, CShape, CLifespan>;
using SEnemies = ecs::Signature<TEnemy, CTransform, CCollision, CShape, CScore>;
using SSmallEnemies = ecs::Signature<TSmallEnemy, CTransform, CCollision, CShape, CLifespan, CScore>;

using MySignatureList = ecs::SignatureList<SPlayers, SBullets, SEnemies, SSmallEnemies>;

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
    ecs::EntityIndex player_entity_index_{};

    /// @brief Traite les inputs (clavier, souris, joystick...)
    auto processInput() -> void;

    /// @brief Mise à jour des données
    auto update() -> void;

    /// @brief Mise à jour du rendu
    auto render() -> void;

public:
    /// @brief Construit une instance du jeu
    /// @param configuration_file_path Chemin d'accès au fichier de configuration du jeu
    explicit Game(const std::string &configuration_file_path);

    /// @brief Fonction principale du jeu
    auto run() -> void;

    auto window() -> sf::RenderWindow &;

    auto spawnPlayer() -> void;

    //auto changeScene<T>() -> void;
    //auto getAssets() -> Assets &;
    //auto sUserInput() -> void;
    //    Dans cette fonction on va faire du events_observable_.notify(Action{"UP"_hash, "START"_hash});
};

#endif //GAME_H
