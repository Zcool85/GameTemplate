//
// Created by Zéro Cool on 19/06/2025.

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "pch.h"

#include "Assets.h"
#include "scenes/Scene.h"
#include "tools/ConfigurationManager.h"
#include "Types.h"

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
class GameEngine {
    // Pour le moment, on se limite à un plan 2D. Donc notre fenêtre sera un sf::RenderWindow et non un sf::Window
    sf::RenderWindow window_;
    std::map<SceneId, std::shared_ptr<Scene> > scenes_;
    std::shared_ptr<Scene> current_scene_;
    bool running_;
    tools::ConfigurationManager configuration_manager_;
    sf::Clock delta_clock_;
    Assets assets_;

    auto update() -> void;

    auto render() -> void;

    // TODO : sUserInput devrait se trouver ici...
public:
    /// @brief Construit une instance du jeu
    /// @param configuration_file_path Chemin d'accès au fichier de configuration du jeu
    /// @param assets_configuration_file_path Chemin d'accès au fichier de configuration des assets
    explicit GameEngine(const std::string &configuration_file_path, const std::string &assets_configuration_file_path);

    /// @brief Fonction principale du jeu
    auto run() -> void;

    auto configurationManager() -> tools::ConfigurationManager &;

    auto changeScene(SceneId id) -> void;

    auto mapPixelToCoords(sf::Vector2i point) const -> sf::Vector2f;

    auto windowSize() const -> sf::Vector2f;

    auto getAssets() -> Assets &;
};

#endif //GAME_ENGINE_H
