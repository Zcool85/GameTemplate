//
// Created by Zéro Cool on 19/06/2025.

#ifndef GAME_H
#define GAME_H

#include <map>
#include <SFML/Graphics.hpp>

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
class Game {
    tools::ConfigurationManager configuration_manager_;
    // Pour le moment on se limite à un plan 2D. Donc notre fenêtre sera un sf::RenderWindow et non un sf::Window
    sf::RenderWindow window_;
    // TODO : Faire un Id fortement typé pour les identifiants de scène
    std::map<SceneId, std::shared_ptr<Scene> > scenes_;
    bool running_;
    std::shared_ptr<Scene> current_scene_;
    sf::Clock delta_clock_;

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

    //auto changeScene<T>() -> void;
    //auto getAssets() -> Assets &;
    //auto sUserInput() -> void;
    //    Dans cette fonction on va faire du events_observable_.notify(Action{"UP"_hash, "START"_hash});
};

#endif //GAME_H
