//
// Created by Zéro Cool on 19/06/2025.
//

#include "GameEngine.h"
#include "scenes/GameScene.h"
#include "scenes/MainMenuScene.h"

GameEngine::GameEngine(const std::string &configuration_file_path, const std::string &assets_configuration_file_path)
    : running_(false),
      configuration_manager_(configuration_file_path, assets_configuration_file_path),
      assets_(configuration_manager_) {
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

    // std::cout << "Window created" << std::endl;
    // if (sf::Vulkan::isAvailable()) {
    //     std::cout << "Vulkan available" << std::endl;
    // } else {
    //     std::cout << "Vulkan not available" << std::endl;
    // }

    if (vertical_sync_enabled) {
        this->window_.setVerticalSyncEnabled(vertical_sync_enabled);
    } else {
        this->window_.setVerticalSyncEnabled(false);
        this->window_.setFramerateLimit(static_cast<unsigned int>(framerate_limit));
    }

    this->window_.setKeyRepeatEnabled(false);

    // Activation du context OpenGL
    if (!this->window_.setActive(true)) {
        std::cerr << "OpenGL context activation failed" << std::endl;
    }

    // Init Scenes
    this->scenes_["GAME"_scene] = std::make_shared<GameScene>(*this);
    this->scenes_["MAIN_MENU"_scene] = std::make_shared<MainMenuScene>(*this);

    // Set scene to game
    this->current_scene_ = this->scenes_["GAME"_scene];

    // Vérifier la version OpenGL obtenue
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    std::cout << "Version OpenGL: " << version << std::endl;
    std::cout << "Carte graphique: " << renderer << std::endl;

    // Vérifier si on a bien obtenu la version demandée
    const sf::ContextSettings actualSettings = this->window_.getSettings();
    std::cout << "Version OpenGL obtenue: "
            << actualSettings.majorVersion << "."
            << actualSettings.minorVersion << std::endl;
    std::cout << "Core Profile: " << (actualSettings.attributeFlags & sf::ContextSettings::Core ? "Oui" : "Non") <<
            std::endl;

    // if (!sf::Shader::isAvailable()) {
    //     std::cerr << "Shader unavailable" << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    // // Load Shaders
    // shader_ = sf::Shader{};
    // if (!shader_.loadFromFile("data/shader.frag", sf::Shader::Type::Fragment))
    // {
    //     std::cerr << "Couldn't load fragment shader" << std::endl;
    // }
}

auto GameEngine::run() -> void {
    if (!ImGui::SFML::Init(this->window_)) {
        std::cerr << "ImGui window init error" << std::endl;
    }

    running_ = true;
    while (running_) {
        this->update();
        this->render();

        running_ = !this->current_scene_->hasEnded();
    }

    ImGui::SFML::Shutdown(this->window_);
}

auto GameEngine::configurationManager() -> tools::ConfigurationManager & {
    return this->configuration_manager_;
}

auto GameEngine::update() -> void {
    const auto delta_clock = this->delta_clock_.restart();
    this->current_scene_->update(delta_clock, window_);
}

auto GameEngine::changeScene(const SceneId id) -> void {
    assert(this->scenes_.contains(id));
    this->current_scene_ = this->scenes_[id];
}

auto GameEngine::mapPixelToCoords(const sf::Vector2i point) const -> sf::Vector2f {
    return window_.mapPixelToCoords(point);
}

auto GameEngine::windowSize() const -> sf::Vector2f {
    return {
        static_cast<float>(this->window_.getSize().x),
        static_cast<float>(this->window_.getSize().y)
    };
}

auto GameEngine::getAssets() -> Assets & {
    return this->assets_;
}

auto GameEngine::render() -> void {
    this->window_.clear();
    this->current_scene_->render(window_);
    this->window_.display();
}
