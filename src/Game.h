//
// Created by Zéro Cool on 19/06/2025.

#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>

#include "tools/ConfigurationManager.h"

/// @brief Classe principale du jeu
class Game {
    tools::ConfigurationManager configuration_manager_;
    sf::RenderWindow window_;

    /// @brief Boucle de mise à jour des données
    auto update() -> void;

    /// @brief Boucle de mise à jour du rendu
    auto render() -> void;

public:
    /// @brief Construit une instance du jeu
    /// @param configuration_file_path Chemin d'accès au fichier de configuration du jeu
    explicit Game(const std::string &configuration_file_path);

    /// @brief Fonction principale du jeu
    auto run() -> void;
};

#endif //GAME_H
