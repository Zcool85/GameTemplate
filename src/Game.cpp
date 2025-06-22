//
// Created by Zéro Cool on 19/06/2025.
//

#include "Game.h"

Game::Game(const std::string &configuration_file_path)
    : configuration_manager_(configuration_file_path) {
}

auto Game::run() -> void {
    auto &[title, fullscreen, width, height] = configuration_manager_.getWindowSettings();
    auto &[anti_aliasing_level, framerate_limit, vertical_sync_enabled] = configuration_manager_.getGraphicsSettings();

    if (fullscreen) {
        this->window_ = sf::RenderWindow(sf::VideoMode({
                                                 static_cast<unsigned>(width),
                                                 static_cast<unsigned>(height)
                                             }
                                         ),
                                         title,
                                         sf::State::Fullscreen,
                                         {
                                             .antiAliasingLevel = static_cast<unsigned>(anti_aliasing_level)
                                         });
    } else {
        this->window_ = sf::RenderWindow(sf::VideoMode({
                                                 static_cast<unsigned>(width),
                                                 static_cast<unsigned>(height)
                                             }
                                         ),
                                         title,
                                         sf::Style::Titlebar | sf::Style::Close,
                                         sf::State::Windowed,
                                         {
                                             .antiAliasingLevel = static_cast<unsigned>(anti_aliasing_level)
                                         });
    }

    this->window_.setFramerateLimit(framerate_limit);
    this->window_.setVerticalSyncEnabled(vertical_sync_enabled);

    while (this->window_.isOpen()) {
        this->update();
        this->render();
    }
}

auto Game::update() -> void {
    while (const auto event = this->window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            this->window_.close();
        }
    }
}

auto Game::render() -> void {
    this->window_.clear();
    this->window_.display();
}
