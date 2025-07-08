//
// Created by Zéro Cool on 22/06/2025.
//

#include "Scene.h"

Scene::Scene(Game &game)
    : game_(game), frame_(0), paused_(false) {
}

auto Scene::registerAction(const sf::Keyboard::Key inputKey, const ActionNameId action) -> void {
    action_map_[inputKey] = action;
}