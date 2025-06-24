//
// Created by Zéro Cool on 19/06/2025.
//

#include <SFML/OpenGL.hpp>

#include "Game.h"

#include "scenes/MainMenuScene.h"

Game::Game(const std::string &configuration_file_path)
: configuration_manager_(configuration_file_path), running_(false) {
    auto &[title, fullscreen, width, height] = configuration_manager_.getWindowSettings();
    auto &[depth_bits, stencil_bits, anti_aliasing_level, major_version, minor_version, attribute_flags, framerate_limit, vertical_sync_enabled] = configuration_manager_.getGraphicsSettings();

    if (fullscreen) {
        this->window_ = sf::RenderWindow(sf::VideoMode({
                                                 static_cast<unsigned>(width),
                                                 static_cast<unsigned>(height)
                                             }
                                         ),
                                         title,
                                         sf::State::Fullscreen,
                                         {
                                             .depthBits = static_cast<unsigned>(depth_bits),
                                             .stencilBits = static_cast<unsigned>(stencil_bits),
                                             .antiAliasingLevel = static_cast<unsigned>(anti_aliasing_level),
                                             .majorVersion = static_cast<unsigned>(major_version),
                                             .minorVersion = static_cast<unsigned>(minor_version),
                                             .attributeFlags = static_cast<unsigned>(attribute_flags)
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
                                            .depthBits = static_cast<unsigned>(depth_bits),
                                            .stencilBits = static_cast<unsigned>(stencil_bits),
                                            .antiAliasingLevel = static_cast<unsigned>(anti_aliasing_level),
                                            .majorVersion = static_cast<unsigned>(major_version),
                                            .minorVersion = static_cast<unsigned>(minor_version),
                                            .attributeFlags = static_cast<unsigned>(attribute_flags)
                                         });
    }

    this->window_.setFramerateLimit(framerate_limit);
    this->window_.setVerticalSyncEnabled(vertical_sync_enabled);

    // Activation du context OpenGL
    if (!this->window_.setActive(true)) {
        std::cerr << "OpenGL context activation failed" << std::endl;
    }

    this->scenes_["MAIN_MENU"_hash] = std::make_shared<MainMenuScene>(*this);
    this->current_scene_ = this->scenes_["MAIN_MENU"_hash];

    // Vérifier la version OpenGL obtenue
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    std::cout << "Version OpenGL: " << version << std::endl;
    std::cout << "Carte graphique: " << renderer << std::endl;

    // Vérifier si on a bien obtenu la version demandée
    sf::ContextSettings actualSettings = this->window_.getSettings();
    std::cout << "Version OpenGL obtenue: "
              << actualSettings.majorVersion << "."
              << actualSettings.minorVersion << std::endl;
}

auto Game::run() -> void {
    running_ = true;
    while (running_) {
        this->update();
        this->render();
    }
}

auto Game::window() -> sf::RenderWindow & {
    return this->window_;
}

auto Game::update() -> void {
    while (const auto& event = this->window_.pollEvent()) {
        const auto closed_event = event->getIf<sf::Event::Closed>();
        const auto resize_event = event->getIf<sf::Event::Resized>();

        if (closed_event) {
            running_ = false;
            break;
        }

        if (resize_event) {
            // on ajuste le viewport lorsque la fenêtre est redimensionnée
            glViewport(0, 0, static_cast<GLsizei>(resize_event->size.x), static_cast<GLsizei>(resize_event->size.y));
            break;
        }
    }
}

auto Game::render() -> void {
    this->window_.clear();
    this->window_.display();
}
