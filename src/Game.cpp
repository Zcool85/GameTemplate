//
// Created by Zéro Cool on 19/06/2025.
//

#include <SFML/OpenGL.hpp>
#include <imgui-SFML.h>

#include "Game.h"

#include <ranges>

#include "imgui.h"
#include "scenes/MainMenuScene.h"

Game::Game(const std::string &configuration_file_path)
    : configuration_manager_(configuration_file_path), running_(false) {
    auto &[title, fullscreen, width, height] = configuration_manager_.getWindowSettings();
    auto &[depth_bits, stencil_bits, anti_aliasing_level, major_version, minor_version, attribute_flags, framerate_limit
        , vertical_sync_enabled] = configuration_manager_.getGraphicsSettings();

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
                                         sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize,
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

    if (vertical_sync_enabled) {
        this->window_.setVerticalSyncEnabled(vertical_sync_enabled);
    } else {
        this->window_.setVerticalSyncEnabled(false);
        this->window_.setFramerateLimit(framerate_limit);
    }

    //this->window_.setKeyRepeatEnabled(false);

    // Activation du context OpenGL
    if (!this->window_.setActive(true)) {
        std::cerr << "OpenGL context activation failed" << std::endl;
    }

    this->scenes_["MAIN_MENU"_hash] = std::make_shared<MainMenuScene>(*this);
    this->current_scene_ = this->scenes_["MAIN_MENU"_hash];

    // Vérifier la version OpenGL obtenue
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    std::cout << "Version OpenGL: " << version << std::endl;
    std::cout << "Carte graphique: " << renderer << std::endl;

    // Vérifier si on a bien obtenu la version demandée
    sf::ContextSettings actualSettings = this->window_.getSettings();
    std::cout << "Version OpenGL obtenue: "
            << actualSettings.majorVersion << "."
            << actualSettings.minorVersion << std::endl;
}

auto Game::run() -> void {
    // Init ImGui
    if (!ImGui::SFML::Init(this->window_)) {
        std::cerr << "ImGui window init error" << std::endl;
    }

    // Init First Scene
    scenes_["MAIN_MENU"_hash] = std::make_shared<MainMenuScene>(*this);
    current_scene_ = this->scenes_["MAIN_MENU"_hash];

    running_ = true;
    // ReSharper disable once CppDFAConstantConditions
    // ReSharper disable once CppDFAEndlessLoop
    while (running_) {
        this->processInput();
        this->update();
        this->render();
    }

    // Shutdown ImGui
    // ReSharper disable once CppDFAUnreachableCode
    ImGui::SFML::Shutdown(this->window_);
}

auto Game::window() -> sf::RenderWindow & {
    return this->window_;
}

auto Game::processInput() -> void {
    this->window_.handleEvents(
        [&](const sf::Event::Closed &closed) {
            ImGui::SFML::ProcessEvent(this->window_, closed);
            running_ = false;
        },
        [&](const sf::Event::KeyPressed &key_pressed) {
            ImGui::SFML::ProcessEvent(this->window_, key_pressed);
            std::cout << "KeyPress " << static_cast<int>(key_pressed.code) << std::endl;
        },
        [&](const sf::Event::Resized &resized) {
            ImGui::SFML::ProcessEvent(this->window_, resized);
            // TODO : Tester si c'est utile... En tout cas, je ne vois pas de différence avec ImGui
            glViewport(0, 0, static_cast<GLsizei>(resized.size.x), static_cast<GLsizei>(resized.size.y));
        },
        [&](const sf::Event::TextEntered &text_entered) {
            ImGui::SFML::ProcessEvent(this->window_, text_entered);
            std::cout << "Text entered : " << sf::String(text_entered.unicode).toAnsiString() << std::endl;
        },
        [&](const auto &event) {
            /* handle all other events */
            ImGui::SFML::ProcessEvent(this->window_, event);
        }
    );
}

auto Game::update() -> void {
    const auto delta_clock = this->delta_clock_.restart();

    ImGui::SFML::Update(this->window_, delta_clock);

    for (const auto &scene: this->scenes_ | std::views::values) {
        scene->update(delta_clock);
    }


    ImGui::Begin("Window title");
    ImGui::Text("Window Text rendered in %f sec!", this->delta_clock_.getElapsedTime().asSeconds());
    // ImGui::Checkbox("Circle", &circleExists);
    // ImGui::SliderFloat("Raduis", &circleRadius, 0.0f, 100.0f);
    ImGui::End();

    // TODO : Just for whatching ImGui possibilities
    ImGui::ShowDemoWindow();
}

auto Game::render() -> void {
    this->window_.clear();

    for (const auto &scene: this->scenes_ | std::views::values) {
        scene->render();
    }

    ImGui::SFML::Render(this->window_);

    this->window_.display();
}
