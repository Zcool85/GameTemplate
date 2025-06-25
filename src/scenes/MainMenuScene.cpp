//
// Created by Zéro Cool on 22/06/2025.
//

#include "MainMenuScene.h"

MainMenuScene::MainMenuScene(const Game &game)
        : Scene(game) {
    // TODO : On peut faire mieux :p
    registerAction(sf::Keyboard::Key::Z, "UP"_hash);
    registerAction(sf::Keyboard::Key::D, "RIGHT"_hash);
    registerAction(sf::Keyboard::Key::Q, "LEFT"_hash);
    registerAction(sf::Keyboard::Key::S, "DOWN"_hash);
}

auto MainMenuScene::update(const sf::Time& delta_time) -> void {

}

auto MainMenuScene::render() -> void {

}
