//
// Created by Zéro Cool on 22/06/2025.
//

#include "Scene.h"

Scene::Scene(GameEngine &game)
    : game_{game}, current_frame_{0}, paused_{false}, ended_{false} {
}

auto Scene::hasEnded() const -> bool {
    return ended_;
}

// auto Scene::registerAction(const sf::Keyboard::Key inputKey, const ActionNameId action) -> void {
//     action_map_[inputKey] = action;
// }
