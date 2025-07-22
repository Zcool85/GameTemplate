//
// Created by Zéro Cool on 17/07/2025.
//

#include "GameScene.h"

#include "physics/Physics.h"
#include "scenes/MainMenuScene.h"

static std::string set_score_text(int score) {
    return std::format("Score: {}", score);
}

GameScene::GameScene(GameEngine &game)
    : Scene(game), score_text_(game.getAssets().getFont("DEFAULT"_font)),
      shoot_sound_{game_.getAssets().getSound("SHOOT_BULLET"_sound)},
      death_sound_{game_.getAssets().getSound("DEATH"_sound)},
      game_over_sound_{game_.getAssets().getSound("GAME_OVER"_sound)},
      kill_enemy_sound_{game_.getAssets().getSound("KILL_ENEMY"_sound)},
      spawn_enemy_sound_{game_.getAssets().getSound("SWEEP"_sound)},
      health_{5} {
    const auto &font_settings = game_.configurationManager().getFontSettings();

    score_ = 0;
    score_text_ = sf::Text{
        game.getAssets().getFont("DEFAULT"_font), set_score_text(score_), static_cast<unsigned>(font_settings.size)
    };
    score_text_.setFillColor({
        static_cast<std::uint8_t>(font_settings.color_r),
        static_cast<std::uint8_t>(font_settings.color_g),
        static_cast<std::uint8_t>(font_settings.color_b)
    });
    score_text_.setPosition({0.f, 0.f});
    current_frame_ = 0;

    spawnPlayer();

    entity_manager_.refresh();

    game_.getAssets().getMusic("GAME_LOOP"_music).setLooping(true);
    game_.getAssets().getMusic("GAME_LOOP"_music).play();
}

auto GameScene::spawnPlayer() -> void {
    const auto &player_settings = game_.configurationManager().getPlayerSettings();
    player_entity_handle_ = entity_manager_.createHandle();

    entity_manager_.addTag<TPlayer>(player_entity_handle_);

    auto &transform(entity_manager_.addComponent<CTransform>(player_entity_handle_));
    auto &collision(entity_manager_.addComponent<CCollision>(player_entity_handle_));
    auto &shape(entity_manager_.addComponent<CShape>(player_entity_handle_));
    entity_manager_.addComponent<CInput>(player_entity_handle_);

    transform.position = {game_.windowSize().x / 2.f, game_.windowSize().y / 2.f}; // NOLINT(*-narrowing-conversions)
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

auto GameScene::spawnEnemy() -> void {
    const auto &enemy_settings = game_.configurationManager().getEnemySettings();

    const auto enemy_entity_index_ = entity_manager_.createIndex();

    entity_manager_.addTag<TEnemy>(enemy_entity_index_);

    auto &transform(entity_manager_.addComponent<CTransform>(enemy_entity_index_));
    auto &collision(entity_manager_.addComponent<CCollision>(enemy_entity_index_));
    auto &shape(entity_manager_.addComponent<CShape>(enemy_entity_index_));
    auto &score(entity_manager_.addComponent<CScore>(enemy_entity_index_));

    std::mt19937 gen(random_device_());
    std::uniform_real_distribution dis_x(0.f + enemy_settings.shape_radius,
                                         game_.windowSize().x - enemy_settings.
                                         shape_radius);
    std::uniform_real_distribution dis_y(0.f + enemy_settings.shape_radius,
                                         game_.windowSize().y - enemy_settings.
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

auto GameScene::spawnBullet(const ecs::impl::Handle player_handle, const sf::Vector2f &target) -> void {
    const auto &player_transform(entity_manager_.getComponent<CTransform>(player_handle));

    const sf::Vector2f direction = (target - player_transform.position).normalized();

    const auto &bullet_settings = game_.configurationManager().getBulletSettings();
    const auto bullet_entity_index_ = entity_manager_.createIndex();

    entity_manager_.addTag<TBullet>(bullet_entity_index_);

    auto &transform(entity_manager_.addComponent<CTransform>(bullet_entity_index_));
    auto &collision(entity_manager_.addComponent<CCollision>(bullet_entity_index_));
    auto &shape(entity_manager_.addComponent<CShape>(bullet_entity_index_));
    auto &lifespan(entity_manager_.addComponent<CLifespan>(bullet_entity_index_));

    transform.position = player_transform.position;
    transform.velocity = direction * bullet_settings.speed;

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

auto GameScene::spawnSmallEnemies(const ecs::EntityIndex enemy) -> void {
    const auto &enemy_shape(entity_manager_.getComponent<CShape>(enemy));
    const auto &enemy_transform(entity_manager_.getComponent<CTransform>(enemy));
    const auto &enemy_score(entity_manager_.getComponent<CScore>(enemy));

    auto small_enemy_angle = enemy_transform.angle;

    for (std::size_t i = 0; i < enemy_shape.circle.getPointCount(); ++i) {
        auto &enemy_settings = game_.configurationManager().getEnemySettings();
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

auto GameScene::update(const sf::Time &delta_time, sf::RenderWindow &render_window) -> void {
    ImGui::SFML::Update(render_window, delta_time);

    sEnemySpawner();
    sMovement(delta_time);
    sCollision();
    sUserInput(render_window);
    sLifespan();
    sGUI();

    entity_manager_.refresh();

    render(render_window);

    current_frame_++;
}

auto GameScene::sMovement(const sf::Time delta_clock) -> void {
    if (!is_movements_system_active) return;

    const auto &player_settings = game_.configurationManager().getPlayerSettings();

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
        spawnBullet(player_entity_handle_, game_.mapPixelToCoords(input.shoot_position));
        shoot_sound_.play();
    }

    // On calcule les déplacements/mouvements des entités
    entity_manager_.forEntitiesMatching<STransform>(
        [delta_clock](
    [[maybe_unused]] const ecs::EntityIndex entity_index,
    CTransform &entity_transform) {
            // Toutes les entités doivent tourner
            entity_transform.angle += 60.f * delta_clock.asSeconds();
            // Toutes les entités doivent se déplacer suivant leur vélocité
            entity_transform.position += entity_transform.velocity * delta_clock.asSeconds();
        });
}

auto GameScene::sUserInput(sf::Window &window) -> void {
    auto &user_input(entity_manager_.getComponent<CInput>(player_entity_handle_));

    window.handleEvents(
        [&](const sf::Event::Closed &closed) {
            ImGui::SFML::ProcessEvent(window, closed);
            ended_ = true;
        },
        [&](const sf::Event::MouseButtonPressed &mouse_button_pressed) {
            ImGui::SFML::ProcessEvent(window, mouse_button_pressed);

            // Pour le moment on fait simple
            if (mouse_button_pressed.button == sf::Mouse::Button::Left) {
                user_input.shoot = true;
                user_input.shoot_position = mouse_button_pressed.position;
            }
        },
        [&](const sf::Event::KeyPressed &key_pressed) {
            ImGui::SFML::ProcessEvent(window, key_pressed);

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
            ImGui::SFML::ProcessEvent(window, key_released);

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
            ImGui::SFML::ProcessEvent(window, resized);
            // TODO : Tester si c'est utile... En tout cas, je ne vois pas de différence avec ImGui
            glViewport(0, 0, static_cast<GLsizei>(resized.size.x), static_cast<GLsizei>(resized.size.y));
        },
        [&](const auto &event) {
            /* handle all other events */
            ImGui::SFML::ProcessEvent(window, event);
        }
    );
}

auto GameScene::sEnemySpawner() -> void {
    if (!is_enemy_spawning_system_active) return;

    const auto &enemy_settings = game_.configurationManager().getEnemySettings();

    if (current_frame_ % enemy_settings.spawn_interval == 0) {
        spawnEnemy();
        // TODO : Pas fan...
        // spawn_enemy_sound_.play();
    }
}

auto GameScene::sLifespan() -> void {
    if (!is_lifespan_system_active) return;

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

auto GameScene::sCollision() -> void {
    if (!is_collision_system_active) return;

    auto &player_transform(entity_manager_.getComponent<CTransform>(player_entity_handle_));
    auto &player_collision(entity_manager_.getComponent<CCollision>(player_entity_handle_));

    // Les balles percutent-elles les ennemis ?
    // Pour chaque ennemi
    entity_manager_.forEntitiesMatching<SEnemies>(
        [this, &player_transform, &player_collision](
    [[maybe_unused]] const ecs::EntityIndex enemy_entity_index,
    [[maybe_unused]] const CTransform &enemy_transform,
    [[maybe_unused]] const CCollision &enemy_collision,
    [[maybe_unused]] const CShape &shape,
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
                    if (Physics::isCollision(enemy_transform.position, bullet_transform.position,
                                             enemy_collision.radius, bullet_collision.radius)) {
                        score_ += enemy_score.score;

                        if (!entity_manager_.hasTag<TSmallEnemy>(enemy_entity_index)) {
                            spawnSmallEnemies(enemy_entity_index);
                        }
                        entity_manager_.kill(enemy_entity_index);
                        entity_manager_.kill(bullet_entity_index);
                        kill_enemy_sound_.play();
                    }
                });

            if (!entity_manager_.isAlive(enemy_entity_index)) return;

            if (!entity_manager_.isAlive(player_entity_handle_)) return;

            if (Physics::isCollision(enemy_transform.position, player_transform.position, enemy_collision.radius,
                                     player_collision.radius)) {
                if (!entity_manager_.hasTag<TSmallEnemy>(enemy_entity_index)) {
                    spawnSmallEnemies(enemy_entity_index);
                }
                entity_manager_.kill(enemy_entity_index);
                entity_manager_.kill(player_entity_handle_);

                health_ -= 1;
                if (health_ <= 0) {
                    game_over_sound_.play();
                    // TODO : GAME OVER
                    //game_.changeScene("GAME_OVER"_scene);
                    health_ = 5;
                } else {
                    death_sound_.play();
                }

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
    if (player_transform.position.y > game_.windowSize().y - player_collision.radius) {
        player_transform.position.y = game_.windowSize().y - player_collision.radius;
    }
    if (player_transform.position.x > game_.windowSize().y - player_collision.radius) {
        player_transform.position.x = game_.windowSize().y - player_collision.radius;
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
            if (transform.position.y > game_.windowSize().y - collision.radius) {
                transform.position.y = game_.windowSize().y - collision.radius;
                transform.velocity.y = -transform.velocity.y;
            }
            if (transform.position.x > game_.windowSize().x - collision.radius) {
                transform.position.x = game_.windowSize().x - collision.radius;
                transform.velocity.x = -transform.velocity.x;
            }
        });
}

auto GameScene::render(sf::RenderTarget &render_target) -> void {
    //shader_.setUniform("u_resolution", sf::Glsl::Vec2{ window_.getSize() });
    //shader_.setUniform("u_mouse", sf::Glsl::Vec2{ mouse_position });
    //shader_.setUniform("u_time", delta_clock.asSeconds());

    if (is_render_system_active) {
        entity_manager_.forEntitiesMatching<SRendering>(
            [&render_target]([[maybe_unused]] const ecs::EntityIndex entity_index, const CTransform &transform,
                             CShape &shape) {
                shape.circle.setPosition(transform.position);
                shape.circle.setRotation(sf::degrees(transform.angle));

                //shader_.setUniform("u_color", sf::Glsl::Vec4{shape.circle.getFillColor()});
                //render_target.draw(shape.circle, &shader_);
                render_target.draw(shape.circle);
            });

        // Render hearts
        auto heart = sf::Sprite(game_.getAssets().getTexture("HEART"_texture)); // TODO : On peut faire bien mieux...
        for (auto i = 0; i < health_; i++) {
            heart.setPosition({50.f * i, 0.f});
            render_target.draw(heart);
        }

        score_text_.setPosition({0, 50});
        score_text_.setString(set_score_text(score_));

        render_target.draw(score_text_);
    }

    ImGui::SFML::Render(render_target);
}

auto GameScene::sGUI() -> void {
    if (!is_gui_system_active) return;

    auto &enemy_settings = game_.configurationManager().getEnemySettings();

    ImGui::Begin("Geometry Wars");
    ImGui::Text("Nombre d'entités : %lu", entity_manager_.getEntityCount());

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("GeometryWarsTabBar", tab_bar_flags)) {
        if (ImGui::BeginTabItem("Systems")) {
            ImGui::Text("Systems tab\nPermet de désactiver les systèmes du jeu");
            ImGui::Checkbox("Movements", &is_movements_system_active);
            ImGui::Checkbox("Lifespan", &is_lifespan_system_active);
            ImGui::Checkbox("Collision", &is_collision_system_active);
            ImGui::Checkbox("Spawning", &is_enemy_spawning_system_active);
            ImGui::Indent();
            ImGui::SliderInt("Span", &enemy_settings.spawn_interval, 1, 5000);
            if (ImGui::Button("Manual Spawn")) {
                spawnEnemy();
            }
            ImGui::Unindent();
            ImGui::Checkbox("GUI", &is_gui_system_active);
            ImGui::Checkbox("Rendering", &is_render_system_active);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Entities")) {
            ImGui::Text("Entities tab!\nPermet de visualiser toutes les entités du jeu");

            if (ImGui::CollapsingHeader("Entities")) {
                ImGui::Indent();
                if (ImGui::CollapsingHeader("bullets")) {
                    ImGui::Indent();
                    entity_manager_.forEntitiesMatching<SBullets>(
                        [this]([[maybe_unused]] const ecs::EntityIndex entity_index,
                               [[maybe_unused]] const CTransform &transform, [[maybe_unused]] CCollision &collision,
                               [[maybe_unused]] const CShape &shape, [[maybe_unused]] const CLifespan &lifespan) {
                            ImGui::PushID(0);
                            ImGui::PushStyleColor(ImGuiCol_Button,
                                                  static_cast<ImVec4>(ImColor(shape.circle.getFillColor())));
                            if (ImGui::Button(std::format("D##{}", entity_index.get()).c_str())) {
                                this->entity_manager_.kill(entity_index);
                                entity_manager_.refresh();
                            }
                            ImGui::PopStyleColor(1);
                            ImGui::PopID();

                            ImGui::SameLine();
                            ImGui::Text("%4lu (%11.6f, %11.6f) %4d/%4d", entity_index.get(), transform.position.x,
                                        transform.position.y, lifespan.remaining, lifespan.lifespan);
                        });
                    ImGui::Unindent();
                }

                if (ImGui::CollapsingHeader("Enemies")) {
                    ImGui::Indent();
                    entity_manager_.forEntitiesMatching<SEnemies>(
                        [this]([[maybe_unused]] const ecs::EntityIndex entity_index,
                               [[maybe_unused]] const CTransform &transform,
                               [[maybe_unused]] const CCollision &collision,
                               [[maybe_unused]] const CShape &shape,
                               [[maybe_unused]] const CScore &score) {
                            if (entity_manager_.hasTag<TSmallEnemy>(entity_index)) return;

                            ImGui::PushID(0);
                            ImGui::PushStyleColor(ImGuiCol_Button,
                                                  static_cast<ImVec4>(ImColor(shape.circle.getFillColor())));
                            if (ImGui::Button(std::format("D##{}", entity_index.get()).c_str())) {
                                this->entity_manager_.kill(entity_index);
                                entity_manager_.refresh();
                            }
                            ImGui::PopStyleColor(1);
                            ImGui::PopID();

                            ImGui::SameLine();
                            ImGui::Text("%4lu (%11.6f, %11.6f)", entity_index.get(), transform.position.x,
                                        transform.position.y);
                        });
                    ImGui::Unindent();
                }

                if (ImGui::CollapsingHeader("Players")) {
                    ImGui::Indent();
                    entity_manager_.forEntitiesMatching<SPlayers>(
                        [this]([[maybe_unused]] const ecs::EntityIndex entity_index,
                               [[maybe_unused]] const CTransform &transform,
                               [[maybe_unused]] const CShape &shape) {
                            ImGui::PushID(0);
                            ImGui::PushStyleColor(ImGuiCol_Button,
                                                  static_cast<ImVec4>(ImColor(shape.circle.getFillColor())));
                            if (ImGui::Button(std::format("D##{}", entity_index.get()).c_str())) {
                                this->entity_manager_.kill(entity_index);
                                entity_manager_.refresh();
                            }
                            ImGui::PopStyleColor(1);
                            ImGui::PopID();

                            ImGui::SameLine();
                            ImGui::Text("%4lu (%11.6f, %11.6f)", entity_index.get(), transform.position.x,
                                        transform.position.y);
                        });
                    ImGui::Unindent();
                }

                if (ImGui::CollapsingHeader("Small enemies")) {
                    ImGui::Indent();
                    entity_manager_.forEntitiesMatching<SSmallEnemies>(
                        [this]([[maybe_unused]] const ecs::EntityIndex entity_index,
                               [[maybe_unused]] const CTransform &transform,
                               [[maybe_unused]] const CShape &shape,
                               [[maybe_unused]] const CLifespan &lifespan) {
                            ImGui::PushID(0);
                            ImGui::PushStyleColor(ImGuiCol_Button,
                                                  static_cast<ImVec4>(ImColor(shape.circle.getFillColor())));
                            if (ImGui::Button(std::format("D##{}", entity_index.get()).c_str())) {
                                this->entity_manager_.kill(entity_index);
                                entity_manager_.refresh();
                            }
                            ImGui::PopStyleColor(1);
                            ImGui::PopID();

                            ImGui::SameLine();
                            ImGui::Text("%4lu (%11.6f, %11.6f) %4d/%4d", entity_index.get(), transform.position.x,
                                        transform.position.y, lifespan.remaining, lifespan.lifespan);
                        });
                    ImGui::Unindent();
                }
                ImGui::Unindent();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();

    // Just for watching ImGui possibilities
    //ImGui::ShowDemoWindow();
}
