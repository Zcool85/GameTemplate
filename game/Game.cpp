//
// Created by Zéro Cool on 19/06/2025.
//

#include <SFML/OpenGL.hpp>
#include <imgui-SFML.h>

#include "Game.h"

#include <ranges>

#include "imgui.h"
#include "scenes/MainMenuScene.h"

static std::string set_score_text(int score) {
    return std::format("Score: {}", score);
}

static auto checkCollision(const sf::Vector2f position1, const sf::Vector2f position2, const float radius1,
                           const float radius2) -> bool {
    const float distanceX = position1.x - position2.x;
    const float distanceY = position1.y - position2.y;
    const float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    const float radiusSquared = radius1 * radius1 + radius2 * radius2;

    return distanceSquared <= radiusSquared;
}

Game::Game(const std::string &configuration_file_path)
    : configuration_manager_(configuration_file_path), running_(false), score_text_(font_) {
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

    const auto &font_settings = configuration_manager_.getFontSettings();

    font_ = sf::Font(std::filesystem::path{font_settings.file});
    score_ = 0;
    score_text_ = sf::Text{font_, set_score_text(score_), static_cast<unsigned>(font_settings.size)};
    score_text_.setFillColor({
        static_cast<std::uint8_t>(font_settings.color_r),
        static_cast<std::uint8_t>(font_settings.color_g),
        static_cast<std::uint8_t>(font_settings.color_b)
    });
    score_text_.setPosition({0.f, 0.f});
    current_frame_ = 0;
    pause_ = false;

    spawnPlayer();

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
        this->update();
    }

    // Shutdown ImGui
    // ReSharper disable once CppDFAUnreachableCode
    ImGui::SFML::Shutdown(this->window_);
}

auto Game::window() -> sf::RenderWindow & {
    return this->window_;
}

auto Game::spawnPlayer() -> void {
    const auto &player_settings = configuration_manager_.getPlayerSettings();
    player_entity_handle_ = entity_manager_.createHandle();

    entity_manager_.addTag<TPlayer>(player_entity_handle_);

    auto &transform(entity_manager_.addComponent<CTransform>(player_entity_handle_));
    auto &collision(entity_manager_.addComponent<CCollision>(player_entity_handle_));
    auto &shape(entity_manager_.addComponent<CShape>(player_entity_handle_));
    entity_manager_.addComponent<CInput>(player_entity_handle_);

    transform.position = {window_.getSize().x / 2.f, window_.getSize().y / 2.f}; // NOLINT(*-narrowing-conversions)
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

// TODO : auto Game::spawnBullet(const ecs::EntityIndex player_index, const sf::Vector2f &target) -> void
auto Game::spawnBullet(const sf::Vector2f initial_position, const sf::Vector2f velocity) -> void {
    const auto &bullet_settings = configuration_manager_.getBulletSettings();
    const auto bullet_entity_index_ = entity_manager_.createIndex();

    entity_manager_.addTag<TBullet>(bullet_entity_index_);

    // CTransform, CCollision, CShape, CLifespan

    auto &transform(entity_manager_.addComponent<CTransform>(bullet_entity_index_));
    auto &collision(entity_manager_.addComponent<CCollision>(bullet_entity_index_));
    auto &shape(entity_manager_.addComponent<CShape>(bullet_entity_index_));
    auto &lifespan(entity_manager_.addComponent<CLifespan>(bullet_entity_index_));

    transform.position = initial_position;
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

auto Game::spawnSmallEnemies(const ecs::EntityIndex enemy) -> void {
    // const std::size_t number_of_small_enemies, const sf::Vector2f initial_position,
    // const float initial_angle, const sf::Vector2f velocity, const sf::Color color,
    // const std::size_t vertices, const int enemy_score


    // number_of_small_enemies      shape.circle.getPointCount(),
    // initial_position             transform.position,
    // initial_angle                transform.angle,
    // velocity                     transform.velocity,
    // color                        shape.circle.getFillColor(),
    // vertices                     shape.circle.getPointCount(),
    // enemy_score                  score.score

    const auto &enemy_shape(entity_manager_.getComponent<CShape>(enemy));
    const auto &enemy_transform(entity_manager_.getComponent<CTransform>(enemy));
    const auto &enemy_score(entity_manager_.getComponent<CScore>(enemy));

    auto small_enemy_angle = enemy_transform.angle;

    for (std::size_t i = 0; i < enemy_shape.circle.getPointCount(); ++i) {
        auto &enemy_settings = configuration_manager_.getEnemySettings();
        const auto small_enemy_entity_index_ = entity_manager_.createIndex();

        entity_manager_.addTag<TEnemy>(small_enemy_entity_index_);
        entity_manager_.addTag<TSmallEnemy>(small_enemy_entity_index_);

        // CTransform, CCollision, CShape, CLifespan, CScore

        auto &transform(entity_manager_.addComponent<CTransform>(small_enemy_entity_index_));
        auto &collision(entity_manager_.addComponent<CCollision>(small_enemy_entity_index_));
        auto &shape(entity_manager_.addComponent<CShape>(small_enemy_entity_index_));
        auto &lifespan(entity_manager_.addComponent<CLifespan>(small_enemy_entity_index_));
        auto &score(entity_manager_.addComponent<CScore>(small_enemy_entity_index_));

        transform.position = enemy_transform.position;
        transform.velocity = enemy_transform.velocity.rotatedBy(sf::degrees(small_enemy_angle));

        collision.radius = enemy_settings.collision_radius * 0.5f;

        shape.circle = sf::CircleShape(enemy_settings.shape_radius * 0.5f);
        shape.circle.setOrigin({enemy_settings.shape_radius * 0.5f, enemy_settings.shape_radius * 0.5f});
        shape.circle.setFillColor(enemy_shape.circle.getFillColor());
        shape.circle.setOutlineColor({
            static_cast<std::uint8_t>(enemy_settings.outline_color_r),
            static_cast<std::uint8_t>(enemy_settings.outline_color_g),
            static_cast<std::uint8_t>(enemy_settings.outline_color_b)
        });
        shape.circle.setOutlineThickness(enemy_settings.outline_thickness);
        shape.circle.setPointCount(enemy_shape.circle.getPointCount());

        lifespan.lifespan = lifespan.remaining = enemy_settings.small_lifespan;

        score.score = enemy_score.score * 2;

        small_enemy_angle += 360.f / static_cast<float>(shape.circle.getPointCount());
    }
}

auto Game::update() -> void {
    const auto delta_clock = this->delta_clock_.restart();

    ImGui::SFML::Update(this->window_, delta_clock);

    for (const auto &scene: this->scenes_ | std::views::values) {
        scene->update(delta_clock);
    }

    sEnemySpawner();
    sMovement(delta_clock);
    sCollision();
    sUserInput();
    sLifespan();
    sGUI();

    entity_manager_.refresh();

    sRender();

    current_frame_++;
}

auto Game::sMovement(const sf::Time delta_clock) -> void {
    const auto &player_settings = configuration_manager_.getPlayerSettings();

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
}

auto Game::sUserInput() -> void {
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

auto Game::sEnemySpawner() -> void {
    const auto &enemy_settings = configuration_manager_.getEnemySettings();

    if (current_frame_ % enemy_settings.spawn_interval == 0) {
        const auto enemy_entity_index_ = entity_manager_.createIndex();

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
}

auto Game::sLifespan() -> void {
    entity_manager_.forEntitiesMatching<SLifespan>(
        [this](
    const ecs::EntityIndex entity_index,
    CLifespan &lifespan,
    CShape &shape
) {
            lifespan.remaining -= 1;
            if (lifespan.remaining <= 0) {
                entity_manager_.kill(entity_index);
            } else {
                const auto &fill_color = shape.circle.getFillColor();
                const auto alpha = static_cast<unsigned char>((255 * lifespan.remaining) / lifespan.lifespan);
                shape.circle.setFillColor(sf::Color{fill_color.r, fill_color.g, fill_color.b, alpha});
            }
        });
}

auto Game::sCollision() -> void {
    auto &player_transform(entity_manager_.getComponent<CTransform>(player_entity_handle_));
    auto &player_collision(entity_manager_.getComponent<CCollision>(player_entity_handle_));

    // Les balles percutent-elles les ennemis ?
    // Pour chaque ennemi
    entity_manager_.forEntitiesMatching<SEnemies>(
        [this, &player_transform, &player_collision](
    [[maybe_unused]] const ecs::EntityIndex enemy_entity_index,
    [[maybe_unused]] const CTransform &enemy_transform,
    [[maybe_unused]] const CCollision &enemy_collision,
    [[maybe_unused]] const CShape &enemy_shape,
    [[maybe_unused]] const CScore &enemy_score
) {
            if (!entity_manager_.isAlive(enemy_entity_index)) return;
            entity_manager_.forEntitiesMatching<SBullets>(
                [this, &enemy_transform, &enemy_score, &enemy_entity_index, &enemy_collision](
            [[maybe_unused]] const ecs::EntityIndex bullet_entity_index,
            [[maybe_unused]] const CTransform &bullet_transform,
            [[maybe_unused]] const CCollision &bullet_collision,
            [[maybe_unused]] const CShape &bullet_shape,
            [[maybe_unused]] const CLifespan &bullet_lifespan
        ) {
                    if (!entity_manager_.isAlive(bullet_entity_index)) return;
                    if (checkCollision(enemy_transform.position, bullet_transform.position,
                                       enemy_collision.radius, bullet_collision.radius)) {
                        score_ += enemy_score.score;

                        if (!entity_manager_.hasTag<TSmallEnemy>(enemy_entity_index)) {
                            spawnSmallEnemies(enemy_entity_index);
                        }
                        entity_manager_.kill(enemy_entity_index);
                        entity_manager_.kill(bullet_entity_index);
                    }
                });

            if (!entity_manager_.isAlive(enemy_entity_index)) return;

            if (!entity_manager_.isAlive(player_entity_handle_)) return;

            if (checkCollision(enemy_transform.position, player_transform.position, enemy_collision.radius,
                               player_collision.radius)) {
                if (!entity_manager_.hasTag<TSmallEnemy>(enemy_entity_index)) {
                    spawnSmallEnemies(enemy_entity_index);
                }
                entity_manager_.kill(enemy_entity_index);
                entity_manager_.kill(player_entity_handle_);

                spawnPlayer();
            }
        });

    // Any object cannot cross window
    if (player_transform.position.y < player_collision.radius) {
        player_transform.position.y = player_collision.radius;
    }
    if (player_transform.position.x < player_collision.radius) {
        player_transform.position.x = player_collision.radius;
    }
    if (player_transform.position.y > static_cast<float>(window_.getSize().y) - player_collision.radius) {
        player_transform.position.y = static_cast<float>(window_.getSize().y) - player_collision.radius;
    }
    if (player_transform.position.x > static_cast<float>(window_.getSize().x) - player_collision.radius) {
        player_transform.position.x = static_cast<float>(window_.getSize().x) - player_collision.radius;
    }

    // The enemies cannot cross window and bounce in the opposite way
    entity_manager_.forEntitiesMatching<SEnemies>(
        [this](
    [[maybe_unused]] const ecs::EntityIndex entity_index,
    [[maybe_unused]] CTransform &transform,
    [[maybe_unused]] const CCollision &collision,
    [[maybe_unused]] const CShape &shape,
    [[maybe_unused]] const CScore &score
) {
            if (transform.position.y < collision.radius) {
                transform.position.y = collision.radius;
                transform.velocity.y = -transform.velocity.y;
            }
            if (transform.position.x < collision.radius) {
                transform.position.x = collision.radius;
                transform.velocity.x = -transform.velocity.x;
            }
            if (transform.position.y > static_cast<float>(window_.getSize().y) - collision.radius) {
                transform.position.y = static_cast<float>(window_.getSize().y) - collision.radius;
                transform.velocity.y = -transform.velocity.y;
            }
            if (transform.position.x > static_cast<float>(window_.getSize().x) - collision.radius) {
                transform.position.x = static_cast<float>(window_.getSize().x) - collision.radius;
                transform.velocity.x = -transform.velocity.x;
            }
        });
}

auto Game::sRender() -> void {
    this->window_.clear();

    for (const auto &scene: this->scenes_ | std::views::values) {
        scene->render();
    }

    entity_manager_.forEntitiesMatching<SRendering>(
        [this]([[maybe_unused]] const ecs::EntityIndex entity_index, const CTransform &transform, CShape &shape) {
            shape.circle.setPosition(transform.position);
            shape.circle.setRotation(sf::degrees(transform.angle));

            this->window_.draw(shape.circle);
        });

    // TODO : Render score
    this->window_.draw(score_text_);

    ImGui::SFML::Render(this->window_);

    this->window_.display();
}

auto Game::sGUI() -> void {
    score_text_.setString(set_score_text(score_));

    ImGui::Begin("Geometry Wars");
    ImGui::Text("Window Text rendered in %f sec!", this->delta_clock_.getElapsedTime().asSeconds());
    // ImGui::Checkbox("Circle", &circleExists);
    // ImGui::SliderFloat("Raduis", &circleRadius, 0.0f, 100.0f);
    ImGui::End();

    // TODO : Just for whatching ImGui possibilities
    //ImGui::ShowDemoWindow();
}
