#include <filesystem>
#include <argparse/argparse.hpp>

#include "game/GameEngine.h"

int main(const int argc, char *argv[]) {
    std::string configuration_file_path;

    argparse::ArgumentParser program(argv[0], "v1.0");

    program.add_argument("-c", "--configuration-file")
            .help("path to configuration file")
            .default_value("config/game.conf")
            .store_into(configuration_file_path);

    program.add_description("Run the game.");
    program.add_epilog("Made by Zéro Cool & Merlou.");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return EXIT_FAILURE;
    }

    if (!std::filesystem::exists(configuration_file_path)) {
        std::cerr << "Configuration file \"" << configuration_file_path << "\" don't exists !" << std::endl;
        return EXIT_FAILURE;
    }

    GameEngine game(configuration_file_path);

    game.run();

    return EXIT_SUCCESS;

}



// #include "tools/Observable.h"

// struct ButtonClickEvent {
//     int x, y;
//     std::string button_name;
// };
//
// DEFINE_EVENT(WindowResizeEvent,
//              int width, height;
// );
//
// DEFINE_EVENT(KeyPressEvent,
//              char key;
//              bool ctrl_pressed;
// );
//
// // Différents types d'observers pour démontrer la flexibilité
// class UILogger {
// public:
//     void notify(const ButtonClickEvent &event) {
//         std::println("Button '{}' clicked at ({}, {})", event.button_name, event.x, event.y);
//     }
//
//     void notify(const WindowResizeEvent &event) {
//         std::println("Window resized to {}x{}", event.width, event.height);
//     }
// };
//
// class StatisticsCollector {
// public:
//     int click_count = 0;
//
//     void operator()(const ButtonClickEvent &event) {
//         ++click_count;
//         std::println("Total clicks: {}", click_count);
//     }
// };
//
// // Fonction lambda comme observer
// inline auto key_handler = [](const KeyPressEvent &event) {
//     std::println("Key '{}' pressed (Ctrl: {})", event.key, event.ctrl_pressed ? "Yes" : "No");
// };
//
// // Classe avec template pour observer multiple types
// template<typename... EventTypes>
// class MultiEventHandler {
// public:
//     template<typename EventType>
//     void notify(const EventType &event) {
//         std::println("Generic handler processing event");
//         // Traitement générique ou spécialisé via if constexpr
//         if constexpr (std::same_as<EventType, ButtonClickEvent>) {
//             std::println("  -> Button event details: {}", event.button_name);
//         }
//     }
// };


//// Création d'un observable supportant plusieurs types d'événements
//auto ui_observable = tools::make_observable<ButtonClickEvent, WindowResizeEvent, KeyPressEvent>();
//
//// Enregistrement de différents types d'observers
//UILogger logger;
//StatisticsCollector stats;
//MultiEventHandler<ButtonClickEvent, WindowResizeEvent> multi_handler;
//
//ui_observable.subscribe<ButtonClickEvent>(logger);
//ui_observable.subscribe<WindowResizeEvent>(logger);
//ui_observable.subscribe<ButtonClickEvent>(stats);
//ui_observable.subscribe<KeyPressEvent>(key_handler);
//ui_observable.subscribe<ButtonClickEvent>(multi_handler);
//
//std::println("=== Testing Observer Pattern ===");
//
//// Émission d'événements
//ui_observable.notify(ButtonClickEvent{100, 200, "OK Button"});
//ui_observable.notify(WindowResizeEvent{1920, 1080});
//ui_observable.notify(KeyPressEvent{'A', true});
//ui_observable.notify(ButtonClickEvent{50, 75, "Cancel Button"});
//
//std::println("\n=== Observer Counts ===");
//std::println("ButtonClickEvent observers: {}", ui_observable.observer_count<ButtonClickEvent>());
//std::println("WindowResizeEvent observers: {}", ui_observable.observer_count<WindowResizeEvent>());
//std::println("KeyPressEvent observers: {}", ui_observable.observer_count<KeyPressEvent>());



// int wazaa = 42;
// int mNewCapacity = 3;
//
// std::tuple<std::vector<int>, std::vector<int>, std::vector<int> > tuples;
//
// ecs::forTypesInTuple(tuples, [wazaa, mNewCapacity](auto &v) {
//     std::cout << "yo " << wazaa << "! new capa : " << mNewCapacity << std::endl;
//     v.resize(mNewCapacity);
// });
