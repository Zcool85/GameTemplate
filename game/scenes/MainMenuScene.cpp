//
// Created by Zéro Cool on 22/06/2025.
//

#include "MainMenuScene.h"

MainMenuScene::MainMenuScene(Game &game)
    : Scene(game) {
    // TODO : On peut faire mieux :p
    registerAction(sf::Keyboard::Key::Z, "UP"_action_name);
    registerAction(sf::Keyboard::Key::D, "RIGHT"_action_name);
    registerAction(sf::Keyboard::Key::Q, "LEFT"_action_name);
    registerAction(sf::Keyboard::Key::S, "DOWN"_action_name);
}

auto MainMenuScene::update([[maybe_unused]] const sf::Time& delta_time) -> void {

}

auto MainMenuScene::render() -> void {

}
