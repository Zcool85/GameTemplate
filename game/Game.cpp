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
        this->window_.setFramerateLimit(static_cast<unsigned int>(framerate_limit));
    }

    this->window_.setKeyRepeatEnabled(false);

    // Activation du context OpenGL
    if (!this->window_.setActive(true)) {
        std::cerr << "OpenGL context activation failed" << std::endl;
    }

    this->scenes_["MAIN_MENU"_scene] = std::make_shared<MainMenuScene>(*this);
    this->current_scene_ = this->scenes_["MAIN_MENU"_scene];

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


    spawnPlayer();
    addEnemySpawner();

    entity_manager_.refresh();
}

auto Game::run() -> void {
    // Init ImGui
    if (!ImGui::SFML::Init(this->window_)) {
        std::cerr << "ImGui window init error" << std::endl;
    }

    // Init First Scene
    scenes_["MAIN_MENU"_scene] = std::make_shared<MainMenuScene>(*this);
    current_scene_ = this->scenes_["MAIN_MENU"_scene];

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

auto Game::addEnemySpawner() -> void {
    auto &enemy_settings = configuration_manager_.getEnemySettings();
    auto enemy_spawner_entity_index = entity_manager_.createIndex();

    entity_manager_.addTag<TSpawning>(enemy_spawner_entity_index);

    auto &lifespan(entity_manager_.addComponent<CLifespan>(enemy_spawner_entity_index));
    lifespan.lifespan = lifespan.remaining = enemy_settings.spawn_interval;
}

auto Game::spawnPlayer() -> void {
    auto &player_settings = configuration_manager_.getPlayerSettings();
    player_entity_handle_ = entity_manager_.createHandle();

    entity_manager_.addTag<TPlayer>(player_entity_handle_);

    auto &transform(entity_manager_.addComponent<CTransform>(player_entity_handle_));
    auto &collision(entity_manager_.addComponent<CCollision>(player_entity_handle_));
    auto &shape(entity_manager_.addComponent<CShape>(player_entity_handle_));
    entity_manager_.addComponent<CInput>(player_entity_handle_);

    transform.position = {window_.getSize().x / 2.f, window_.getSize().y / 2.f}; // NOLINT(*-narrowing-conversions)
    transform.scale = {1.f, 1.f};
    transform.velocity = {};

    collision.radius = player_settings.collision_radius;

    shape.circle = sf::CircleShape(player_settings.shape_radius);
    shape.circle.setOrigin({player_settings.shape_radius, player_settings.shape_radius});
    shape.circle.setFillColor({
        static_cast<std::uint8_t>(player_settings.fill_color_r),
        static_cast<std::uint8_t>(player_settings.fill_color_g),
        static_cast<std::uint8_t>(player_settings.fill_color_b)
    });
    shape.circle.setOutlineColor({
        static_cast<std::uint8_t>(player_settings.outline_color_r),
        static_cast<std::uint8_t>(player_settings.outline_color_g),
        static_cast<std::uint8_t>(player_settings.outline_color_b)
    });
    shape.circle.setOutlineThickness(player_settings.outline_thickness);
    shape.circle.setPointCount(static_cast<std::size_t>(player_settings.shape_vertices));
}

auto Game::spawnBullet(const sf::Vector2f initial_position, const sf::Vector2f velocity) -> void {
    auto &bullet_settings = configuration_manager_.getBulletSettings();
    const auto bullet_entity_index_ = entity_manager_.createIndex();

    entity_manager_.addTag<TBullet>(bullet_entity_index_);

    // CTransform, CCollision, CShape, CLifespan

    auto &transform(entity_manager_.addComponent<CTransform>(bullet_entity_index_));
    auto &collision(entity_manager_.addComponent<CCollision>(bullet_entity_index_));
    auto &shape(entity_manager_.addComponent<CShape>(bullet_entity_index_));
    auto &lifespan(entity_manager_.addComponent<CLifespan>(bullet_entity_index_));

    transform.position = initial_position;
    transform.scale = {1.f, 1.f};
    transform.velocity = velocity * bullet_settings.speed;

    collision.radius = bullet_settings.collision_radius;

    shape.circle = sf::CircleShape(bullet_settings.shape_radius);
    shape.circle.setOrigin({bullet_settings.shape_radius, bullet_settings.shape_radius});
    shape.circle.setFillColor({
        static_cast<std::uint8_t>(bullet_settings.fill_color_r),
        static_cast<std::uint8_t>(bullet_settings.fill_color_g),
        static_cast<std::uint8_t>(bullet_settings.fill_color_b)
    });
    shape.circle.setOutlineColor({
        static_cast<std::uint8_t>(bullet_settings.outline_color_r),
        static_cast<std::uint8_t>(bullet_settings.outline_color_g),
        static_cast<std::uint8_t>(bullet_settings.outline_color_b)
    });
    shape.circle.setOutlineThickness(bullet_settings.outline_thickness);
    shape.circle.setPointCount(static_cast<std::size_t>(bullet_settings.shape_vertices));

    lifespan.lifespan = lifespan.remaining = bullet_settings.lifespan;
}

auto Game::spawnLittleEnemies(const std::size_t number_of_little_enemies, const sf::Vector2f initial_position,
                              const float initial_angle, const sf::Vector2f velocity, const sf::Color color,
                              const std::size_t vertices, const int enemy_score) -> void {
    auto small_enemy_angle = initial_angle;

    for (std::size_t i = 0; i < number_of_little_enemies; ++i) {
        auto &enemy_settings = configuration_manager_.getEnemySettings();
        const auto little_enemy_entity_index_ = entity_manager_.createIndex();

        entity_manager_.addTag<TEnemy>(little_enemy_entity_index_);
        entity_manager_.addTag<TLittleEnemy>(little_enemy_entity_index_);

        // CTransform, CCollision, CShape, CLifespan, CScore

        auto &transform(entity_manager_.addComponent<CTransform>(little_enemy_entity_index_));
        auto &collision(entity_manager_.addComponent<CCollision>(little_enemy_entity_index_));
        auto &shape(entity_manager_.addComponent<CShape>(little_enemy_entity_index_));
        auto &lifespan(entity_manager_.addComponent<CLifespan>(little_enemy_entity_index_));
        auto &score(entity_manager_.addComponent<CScore>(little_enemy_entity_index_));

        transform.position = initial_position;
        transform.scale = {1.f, 1.f};
        transform.velocity = velocity.rotatedBy(sf::degrees(small_enemy_angle));

        collision.radius = enemy_settings.collision_radius * 0.5f;

        shape.circle = sf::CircleShape(enemy_settings.shape_radius * 0.5f);
        shape.circle.setOrigin({enemy_settings.shape_radius * 0.5f, enemy_settings.shape_radius * 0.5f});
        shape.circle.setFillColor(color);
        shape.circle.setOutlineColor({
            static_cast<std::uint8_t>(enemy_settings.outline_color_r),
            static_cast<std::uint8_t>(enemy_settings.outline_color_g),
            static_cast<std::uint8_t>(enemy_settings.outline_color_b)
        });
        shape.circle.setOutlineThickness(enemy_settings.outline_thickness);
        shape.circle.setPointCount(vertices);

        lifespan.lifespan = lifespan.remaining = enemy_settings.small_lifespan;

        score.score = enemy_score * 2;

        small_enemy_angle += 360.f / static_cast<float>(vertices);
    }
}

auto Game::processInput() -> void {
    auto &user_input(entity_manager_.getComponent<CInput>(player_entity_handle_));

    this->window_.handleEvents(
        [&](const sf::Event::Closed &closed) {
            ImGui::SFML::ProcessEvent(this->window_, closed);
            running_ = false;
        },
        [&](const sf::Event::MouseButtonPressed &mouse_button_pressed) {
            ImGui::SFML::ProcessEvent(this->window_, mouse_button_pressed);

            // Pour le moment on fait simple
            if (mouse_button_pressed.button == sf::Mouse::Button::Left) {
                user_input.shoot = true;
                user_input.shoot_position = mouse_button_pressed.position;
            }
        },
        [&](const sf::Event::KeyPressed &key_pressed) {
            ImGui::SFML::ProcessEvent(this->window_, key_pressed);

            // Pour le moment on fait simple...
            if (key_pressed.code == sf::Keyboard::Key::Z) {
                user_input.up = true;
            }
            if (key_pressed.code == sf::Keyboard::Key::Q) {
                user_input.left = true;
            }
            if (key_pressed.code == sf::Keyboard::Key::S) {
                user_input.down = true;
            }
            if (key_pressed.code == sf::Keyboard::Key::D) {
                user_input.right = true;
            }
        },
        [&](const sf::Event::KeyReleased &key_released) {
            ImGui::SFML::ProcessEvent(this->window_, key_released);

            // Pour le moment on fait simple...
            if (key_released.code == sf::Keyboard::Key::Z) {
                user_input.up = false;
            }
            if (key_released.code == sf::Keyboard::Key::Q) {
                user_input.left = false;
            }
            if (key_released.code == sf::Keyboard::Key::S) {
                user_input.down = false;
            }
            if (key_released.code == sf::Keyboard::Key::D) {
                user_input.right = false;
            }
        },
        [&](const sf::Event::Resized &resized) {
            ImGui::SFML::ProcessEvent(this->window_, resized);
            // TODO : Tester si c'est utile... En tout cas, je ne vois pas de différence avec ImGui
            glViewport(0, 0, static_cast<GLsizei>(resized.size.x), static_cast<GLsizei>(resized.size.y));
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

    sUserInput();
    sMovement(delta_clock);
    sCollision();
    sEnemySpawner();
    sLifespanKiller();

    entity_manager_.refresh();

    ImGui::Begin("Window title");
    ImGui::Text("Window Text rendered in %f sec!", this->delta_clock_.getElapsedTime().asSeconds());
    // ImGui::Checkbox("Circle", &circleExists);
    // ImGui::SliderFloat("Raduis", &circleRadius, 0.0f, 100.0f);
    ImGui::End();

    // TODO : Just for whatching ImGui possibilities
    //ImGui::ShowDemoWindow();
}

auto Game::render() -> void {
    this->window_.clear();

    for (const auto &scene: this->scenes_ | std::views::values) {
        scene->render();
    }

    sRender();

    ImGui::SFML::Render(this->window_);

    this->window_.display();
}

auto Game::sMovement(const sf::Time delta_clock) -> void {
    // Toutes les entités doivent tourner
    entity_manager_.forEntitiesMatching<STransform>(
        [delta_clock](
    [[maybe_unused]] const ecs::EntityIndex entity_index,
    CTransform &transform) {
            // Toutes les entités doivent tourner
            transform.angle += 60.f * delta_clock.asSeconds();

            // Toutes les entités doivent se déplacer suivant leur vélocité
            transform.position += transform.velocity * delta_clock.asSeconds();
        });

    // On met à jour les entités qui ont un lifespan
    entity_manager_.forEntitiesMatching<SLifespan>(
        [](
    [[maybe_unused]] const ecs::EntityIndex entity_index,
    CLifespan &lifespan
) {
            lifespan.remaining -= 1;
        });
}

auto Game::sUserInput() -> void {
    auto &player_settings = configuration_manager_.getPlayerSettings();

    auto &transform(entity_manager_.getComponent<CTransform>(player_entity_handle_));
    auto &input(entity_manager_.getComponent<CInput>(player_entity_handle_));

    sf::Vector2f direction;

    if (input.up) {
        direction.y = -1.f;
    }
    if (input.down) {
        direction.y = 1.f;
    }
    if (input.left) {
        direction.x = -1.f;
    }
    if (input.right) {
        direction.x = 1.f;
    }

    // TODO : Je ne sais pas comment faire mieux à date...
    if (direction != sf::Vector2f(0.f, 0.f)) {
        transform.velocity = direction.normalized() * player_settings.speed;
    } else {
        transform.velocity = sf::Vector2f(0.f, 0.f);
    }

    if (input.shoot) {
        input.shoot = false;
        spawnBullet(transform.position,
                    (window_.mapPixelToCoords(input.shoot_position) - transform.position).normalized());
    }
}

auto Game::sEnemySpawner() -> void {
    // Mise à jour des entités permettant de spawner des ennemis
    entity_manager_.forEntitiesMatching<SEnemiesSpawner>(
        [this](
    [[maybe_unused]] const ecs::EntityIndex entity_index,
    CLifespan &lifespan
) {
            if (lifespan.remaining == 0) {
                lifespan.remaining = lifespan.lifespan;

                auto &enemy_settings = configuration_manager_.getEnemySettings();
                auto enemy_entity_index_ = entity_manager_.createIndex();

                entity_manager_.addTag<TEnemy>(enemy_entity_index_);

                auto &transform(entity_manager_.addComponent<CTransform>(enemy_entity_index_));
                auto &collision(entity_manager_.addComponent<CCollision>(enemy_entity_index_));
                auto &shape(entity_manager_.addComponent<CShape>(enemy_entity_index_));
                auto &score(entity_manager_.addComponent<CScore>(enemy_entity_index_));

                std::mt19937 gen(random_device_());
                std::uniform_real_distribution dis_x(0.f + enemy_settings.shape_radius,
                                                     static_cast<float>(window_.getSize().x) - enemy_settings.
                                                     shape_radius);
                std::uniform_real_distribution dis_y(0.f + enemy_settings.shape_radius,
                                                     static_cast<float>(window_.getSize().y) - enemy_settings.
                                                     shape_radius);
                std::uniform_real_distribution dis_speed(enemy_settings.min_speed, enemy_settings.max_speed);
                std::uniform_int_distribution dis_color(0, 256);
                std::uniform_int_distribution
                        dis_vertices(enemy_settings.min_vertices, enemy_settings.max_vertices + 1);

                transform.position = {dis_x(gen), dis_y(gen)};
                transform.scale = {1.f, 1.f};
                transform.velocity = {dis_speed(gen), dis_speed(gen)};

                collision.radius = enemy_settings.collision_radius;

                shape.circle = sf::CircleShape(enemy_settings.shape_radius);
                shape.circle.setOrigin({enemy_settings.shape_radius, enemy_settings.shape_radius});
                shape.circle.setFillColor({
                    static_cast<std::uint8_t>(dis_color(gen)),
                    static_cast<std::uint8_t>(dis_color(gen)),
                    static_cast<std::uint8_t>(dis_color(gen))
                });
                shape.circle.setOutlineColor({
                    static_cast<std::uint8_t>(enemy_settings.outline_color_r),
                    static_cast<std::uint8_t>(enemy_settings.outline_color_g),
                    static_cast<std::uint8_t>(enemy_settings.outline_color_b)
                });
                shape.circle.setOutlineThickness(enemy_settings.outline_thickness);
                shape.circle.setPointCount(static_cast<std::size_t>(dis_vertices(gen)));

                score.score = 100 * static_cast<int>(shape.circle.getPointCount());
            }
        });
}

auto Game::sLifespanKiller() -> void {
    entity_manager_.forEntitiesMatching<SLifespan>(
    [this](
const ecs::EntityIndex entity_index,
const CLifespan &lifespan
) {
            if (lifespan.remaining == 0) {
                entity_manager_.kill(entity_index);
            }
        });
}

static auto checkColision(const sf::Vector2f position1, const sf::Vector2f position2, const float radius1,
                          const float radius2) -> bool {
    float distanceX = position1.x - position2.x;
    float distanceY = position1.y - position2.y;
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    float radiusSquared = radius1 * radius1 + radius2 * radius2;

    return distanceSquared <= radiusSquared;
}

auto Game::sCollision() -> void {
    // Les balles percutent-elles les ennemis ?
    // Pour chaque ennemi
    entity_manager_.forEntitiesMatching<SEnemies>(
        [this](
    [[maybe_unused]] const ecs::EntityIndex enemy_entity_index,
    [[maybe_unused]] const CTransform &enemy_transform,
    [[maybe_unused]] const CCollision &enemy_collision,
    [[maybe_unused]] const CShape &enemy_shape,
    [[maybe_unused]] const CScore &enemy_score
) {
            if (!entity_manager_.isAlive(enemy_entity_index)) return;
            entity_manager_.forEntitiesMatching<SBullets>(
                [this, &enemy_shape, &enemy_transform, &enemy_score, &enemy_entity_index](
            [[maybe_unused]] const ecs::EntityIndex bullet_entity_index,
            [[maybe_unused]] const CTransform &bullet_transform,
            [[maybe_unused]] const CCollision &bullet_collision,
            [[maybe_unused]] const CShape &bullet_shape,
            [[maybe_unused]] const CLifespan &bullet_lifespan
        ) {
                    if (!entity_manager_.isAlive(bullet_entity_index)) return;
                    if (checkColision(enemy_transform.position, bullet_transform.position,
                                      enemy_shape.circle.getRadius(), bullet_shape.circle.getRadius())) {
                        // TODO : Add score
                        if (!entity_manager_.hasTag<TLittleEnemy>(enemy_entity_index)) {
                            spawnLittleEnemies(
                                enemy_shape.circle.getPointCount(),
                                enemy_transform.position,
                                enemy_transform.angle,
                                enemy_transform.velocity,
                                enemy_shape.circle.getFillColor(),
                                enemy_shape.circle.getPointCount(),
                                enemy_score.score
                            );
                        }
                        entity_manager_.kill(enemy_entity_index);
                        entity_manager_.kill(bullet_entity_index);
                    }
                });

            if (!entity_manager_.isAlive(enemy_entity_index)) return;

            if (!entity_manager_.isAlive(player_entity_handle_)) return;

            auto &player_transform(entity_manager_.getComponent<CTransform>(player_entity_handle_));
            auto &player_shape(entity_manager_.getComponent<CShape>(player_entity_handle_));

            if (checkColision(enemy_transform.position, player_transform.position, enemy_shape.circle.getRadius(),
                              player_shape.circle.getRadius())) {
                if (!entity_manager_.hasTag<TLittleEnemy>(enemy_entity_index)) {
                    spawnLittleEnemies(
                        enemy_shape.circle.getPointCount(),
                        enemy_transform.position,
                        enemy_transform.angle,
                        enemy_transform.velocity,
                        enemy_shape.circle.getFillColor(),
                        enemy_shape.circle.getPointCount(),
                        enemy_score.score
                    );
                }
                entity_manager_.kill(enemy_entity_index);
                entity_manager_.kill(player_entity_handle_);

                spawnPlayer();
            }

    });

    // Any object cannot cross window
    entity_manager_.forEntitiesMatching<SPlayers>(
        [this](
    [[maybe_unused]] const ecs::EntityIndex entity_index,
    [[maybe_unused]] CTransform &transform,
    [[maybe_unused]] const CCollision &collision,
    [[maybe_unused]] const CShape &shape,
    [[maybe_unused]] const CInput &input
) {
            if (transform.position.y < shape.circle.getRadius()) {
                transform.position.y = shape.circle.getRadius();
            }
            if (transform.position.x < shape.circle.getRadius()) {
                transform.position.x = shape.circle.getRadius();
            }
            if (transform.position.y > static_cast<float>(window_.getSize().y) - shape.circle.getRadius()) {
                transform.position.y = static_cast<float>(window_.getSize().y) - shape.circle.getRadius();
            }
            if (transform.position.x > static_cast<float>(window_.getSize().x) - shape.circle.getRadius()) {
                transform.position.x = static_cast<float>(window_.getSize().x) - shape.circle.getRadius();
            }
        });

    // The enemies cannot cross window and bounce in the opposite way
    entity_manager_.forEntitiesMatching<SEnemies>(
        [this](
    [[maybe_unused]] const ecs::EntityIndex entity_index,
    [[maybe_unused]] CTransform &transform,
    [[maybe_unused]] const CCollision &collision,
    [[maybe_unused]] const CShape &shape,
    [[maybe_unused]] const CScore &score
) {
            if (transform.position.y < shape.circle.getRadius()) {
                transform.position.y = shape.circle.getRadius();
                transform.velocity.y = -transform.velocity.y;
            }
            if (transform.position.x < shape.circle.getRadius()) {
                transform.position.x = shape.circle.getRadius();
                transform.velocity.x = -transform.velocity.x;
            }
            if (transform.position.y > static_cast<float>(window_.getSize().y) - shape.circle.getRadius()) {
                transform.position.y = static_cast<float>(window_.getSize().y) - shape.circle.getRadius();
                transform.velocity.y = -transform.velocity.y;
            }
            if (transform.position.x > static_cast<float>(window_.getSize().x) - shape.circle.getRadius()) {
                transform.position.x = static_cast<float>(window_.getSize().x) - shape.circle.getRadius();
                transform.velocity.x = -transform.velocity.x;
            }
        });
}

auto Game::sRender() -> void {
    entity_manager_.forEntitiesMatching<SRendering>(
        [this]([[maybe_unused]] const ecs::EntityIndex entity_index, const CTransform &transform, CShape &shape) {
            shape.circle.setPosition(transform.position);
            shape.circle.setScale(transform.scale);
            shape.circle.setRotation(sf::degrees(transform.angle));

            if (entity_manager_.hasComponent<CLifespan>(entity_index)) {
                auto &lifespan(entity_manager_.getComponent<CLifespan>(entity_index));

                const auto &fill_color = shape.circle.getFillColor();
                const auto alpha = static_cast<unsigned char>((255 * lifespan.remaining) / lifespan.lifespan);
                shape.circle.setFillColor(sf::Color{fill_color.r, fill_color.g, fill_color.b, alpha});
            }

            this->window_.draw(shape.circle);
        });
}
