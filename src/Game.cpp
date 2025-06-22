//
// Created by Zéro Cool on 19/06/2025.
//

#include "Game.h"

void Game::update() {
    while (const auto event = this->window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            this->window_.close();
        }
    }
}

void Game::render() {
    this->window_.clear();
    this->window_.display();
}

Game::Game(const std::string &configuration_file_path)
    : configuration_manager_(configuration_file_path) {
    auto &window_settings = configuration_manager_.get_window_settings();
    auto &graphics_settings = configuration_manager_.get_graphics_settings();

    if (window_settings.fullscreen) {
        this->window_ = sf::RenderWindow(sf::VideoMode({
                                                 static_cast<unsigned>(window_settings.width),
                                                 static_cast<unsigned>(window_settings.height)
                                             }
                                         ),
                                         window_settings.title,
                                         sf::State::Fullscreen,
                                         {
                                             .antiAliasingLevel = static_cast<unsigned>(graphics_settings.
                                                 anti_aliasing_level)
                                         });
    } else {
        this->window_ = sf::RenderWindow(sf::VideoMode({
                                                 static_cast<unsigned>(window_settings.width),
                                                 static_cast<unsigned>(window_settings.height)
                                             }
                                         ),
                                         window_settings.title,
                                         sf::Style::Titlebar | sf::Style::Close,
                                         sf::State::Windowed,
                                         {
                                             .antiAliasingLevel = static_cast<unsigned>(graphics_settings.
                                                 anti_aliasing_level)
                                         });
    }

    this->window_.setFramerateLimit(graphics_settings.framerate_limit);
    this->window_.setVerticalSyncEnabled(graphics_settings.vertical_sync_enabled);
}

void Game::run() {
    while (this->window_.isOpen()) {
        this->update();
        this->render();
    }
}
