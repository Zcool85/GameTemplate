//
// Created by Zéro Cool on 19/06/2025.
//

#include "Game.h"

void Game::update() {
    while (const auto event = this->window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            this->window.close();
        }
    }
}

void Game::render() {
    this->window.clear();
    this->window.display();
}

Game::Game() {
    bool fullsreen = false;

    if (fullsreen) {
        this->window = sf::RenderWindow(sf::VideoMode({1728, 1117}),
                                        "Test",
                                        sf::State::Fullscreen,
                                        {.antiAliasingLevel = 0});
    } else {
        this->window = sf::RenderWindow(sf::VideoMode({1728, 1117}),
                                        "Test",
                                        sf::Style::Titlebar | sf::Style::Close,
                                        sf::State::Windowed,
                                        {.antiAliasingLevel = 0});
    }

    this->window.setFramerateLimit(60);
    this->window.setVerticalSyncEnabled(true);
}

void Game::run() {
    while (this->window.isOpen()) {
        this->update();
        this->render();
    }
}
