#include <filesystem>
#include <argparse/argparse.hpp>

#include "Ecs.h"
#include "Game.h"
#include "tools/strong_typedef.h"
#include "tools/Observable.h"

struct ButtonClickEvent {
    int x, y;
    std::string button_name;
};

DEFINE_EVENT(WindowResizeEvent,
             int width, height;
);

DEFINE_EVENT(KeyPressEvent,
             char key;
             bool ctrl_pressed;
);

// Différents types d'observers pour démontrer la flexibilité
class UILogger {
public:
    void notify(const ButtonClickEvent &event) {
        std::println("Button '{}' clicked at ({}, {})", event.button_name, event.x, event.y);
    }

    void notify(const WindowResizeEvent &event) {
        std::println("Window resized to {}x{}", event.width, event.height);
    }
};

class StatisticsCollector {
public:
    int click_count = 0;

    void operator()(const ButtonClickEvent &event) {
        ++click_count;
        std::println("Total clicks: {}", click_count);
    }
};

// Fonction lambda comme observer
inline auto key_handler = [](const KeyPressEvent &event) {
    std::println("Key '{}' pressed (Ctrl: {})", event.key, event.ctrl_pressed ? "Yes" : "No");
};

// Classe avec template pour observer multiple types
template<typename... EventTypes>
class MultiEventHandler {
public:
    template<typename EventType>
    void notify(const EventType &event) {
        std::println("Generic handler processing event");
        // Traitement générique ou spécialisé via if constexpr
        if constexpr (std::same_as<EventType, ButtonClickEvent>) {
            std::println("  -> Button event details: {}", event.button_name);
        }
    }
};


//
// / Usage
// /

// Components :
//   Simple, logic-less, data-containing struct.

struct CTransform {
    int x;
};

struct CVelocity {
    int value;
};

struct CPosition {
    int value;
};

// ComponentList :
//   Compile-time list of component types.

using MyComponentsList = ecs::ComponentList<
    CTransform,
    CPosition
>;

// Tag :
//   Simple, logic-less and data-less struct.

struct Tag0 {
};

struct Tag1 {
};
struct TPlayer {
};

// TagList :
//   Compile-time list of tag types.

using MyTagList = ecs::TagList<
    Tag0,
    Tag1,
    TPlayer
>;

// Signature
//   Compile-time mixed list of required components and tags.

using S0 = ecs::Signature<>;
using S1 = ecs::Signature<CTransform, CVelocity>;
using S2 = ecs::Signature<CTransform, CPosition, Tag0>;
using S3 = ecs::Signature<CVelocity, Tag0, CPosition, Tag1>;

// SignatureList
//   Compile-time list of signature types.

using MySignatureList = ecs::SignatureList<S0, S1, S2, S3>;


//
// template<typename T>
// T& lookup_or_default(libconfig::Config& config, std::string path, T& default_value) {
//
// }


template<typename T>
struct transform_to_vector;

template<template<typename...> class List, typename... Ts>
struct transform_to_vector<List<Ts...> > {
    using type = std::tuple<std::vector<Ts>...>;
};

template<typename type_sequence>
using transformed_tuple_t = typename transform_to_vector<type_sequence>::type;


int main(const int argc, char *argv[]) {
    // Création d'un observable supportant plusieurs types d'événements
    auto ui_observable = tools::make_observable<ButtonClickEvent, WindowResizeEvent, KeyPressEvent>();

    // Enregistrement de différents types d'observers
    UILogger logger;
    StatisticsCollector stats;
    MultiEventHandler<ButtonClickEvent, WindowResizeEvent> multi_handler;

    ui_observable.subscribe<ButtonClickEvent>(logger);
    ui_observable.subscribe<WindowResizeEvent>(logger);
    ui_observable.subscribe<ButtonClickEvent>(stats);
    ui_observable.subscribe<KeyPressEvent>(key_handler);
    ui_observable.subscribe<ButtonClickEvent>(multi_handler);

    std::println("=== Testing Observer Pattern ===");

    // Émission d'événements
    ui_observable.notify(ButtonClickEvent{100, 200, "OK Button"});
    ui_observable.notify(WindowResizeEvent{1920, 1080});
    ui_observable.notify(KeyPressEvent{'A', true});
    ui_observable.notify(ButtonClickEvent{50, 75, "Cancel Button"});

    std::println("\n=== Observer Counts ===");
    std::println("ButtonClickEvent observers: {}", ui_observable.observer_count<ButtonClickEvent>());
    std::println("WindowResizeEvent observers: {}", ui_observable.observer_count<WindowResizeEvent>());
    std::println("KeyPressEvent observers: {}", ui_observable.observer_count<KeyPressEvent>());


    using MySettings = ecs::Settings<MyComponentsList, MyTagList, MySignatureList>;

    std::cout << "Settings component count : " << MySettings::componentCount() << std::endl;

    std::cout << "Settings contains CTransform ? " << std::boolalpha
            << MySettings::isComponent<CTransform>()
            << " (" << MySettings::componentID<CTransform>() << ")" << std::endl;

    std::cout << "Settings contains CPosition ? " << std::boolalpha
            << MySettings::isComponent<CPosition>()
            << " (" << MySettings::componentID<CPosition>() << ")" << std::endl;

    std::cout << "Settings contains CVelocity ? " << std::boolalpha
            << MySettings::isComponent<CVelocity>()
            << " (" << MySettings::componentID<CVelocity>() << ")" << std::endl;


    // int wazaa = 42;
    // int mNewCapacity = 3;
    //
    // std::tuple<std::vector<int>, std::vector<int>, std::vector<int> > tuples;
    //
    // ecs::forTypesInTuple(tuples, [wazaa, mNewCapacity](auto &v) {
    //     std::cout << "yo " << wazaa << "! new capa : " << mNewCapacity << std::endl;
    //     v.resize(mNewCapacity);
    // });


    ecs::Impl::ComponentStorage<MySettings> storage;
    storage.grow(4);
    auto &t = storage.getComponent<CTransform>(ecs::DataIndex{1});

    std::cout << "CTransform(1).x = " << t.x << std::endl;


    //ecs::Impl::SignatureBitsets<MySettings> signature_bitsets;

    // static_assert(std::is_same_v<
    //                   typeof(signature_bitsets),
    //                   std::tuple<
    //               MySettings::Bitset,
    //               MySettings::Bitset,
    //               MySettings::Bitset,
    //               MySettings::Bitset
    //                   >
    //               >, "");


    // ecs::Impl::SignatureBitsetsStorage<MySettings> storage2;
    // auto &bs = storage2.getSignatureBitset<S2>();

    // Test strong_typedef

    using TypeA = tools::strong_typedef<int, struct ATag>;
    using TypeB = tools::strong_typedef<int, struct BTag>;

    auto val0 = TypeA();
    auto val1 = TypeA{1};
    auto val2 = TypeB{2};
    val2 = 10;
    //val2 += 10;
    val2 += val2;

    static_assert(!std::is_same_v<TypeA, TypeB>, "Type not different");

    val2.get()++;

    std::cout << "val2 : " << val2.get() << std::endl;


    //assert(val1 != val2, "Values must be different");

    // using TypeSequence = ecs::type_sequence<int, float, std::string>;
    //
    //
    // // We need to "unpack" the contents of `ComponentList` in
    // // `TupleOfVectors`. We can do that using `MPL::Rename`.
    // //MPL::Rename<TupleOfVectors, ComponentList> vectors;
    // // On cherche ici à produire un tuple contenant des vecteurs de composants :
    // // std::tuple<std::vector<C1>, std::vector<C2>, std::vector<C3>> vectors;
    // transformed_tuple_t<TypeSequence> vectors;
    //
    //
    // ecs::for_each_type(vectors, [](auto &v) {
    //     v.resize(3);
    //     std::cout << "Type : " << typeid(v).name() << std::endl;
    // });

    using SignatureBitsets = typename MySettings::SignatureBitsets;
    using BitsetStorage = typename SignatureBitsets::BitsetStorage;

    static_assert(
        std::is_same_v<BitsetStorage, std::tuple<std::bitset<5>, std::bitset<5>, std::bitset<5>, std::bitset<5> > >,
        "");

    using EntityManager = ecs::Manager<MySettings>;

    // EntityManager mgr;
    //
    // auto player(mgr.createIndex());
    // mgr.addTag<TPlayer>(player);
    //
    // auto& pos(mgr.addComponent<CPosition>(player).value);


    /////////////////////////////////////////////////////////////////////////
    ///// THE GAME
    /////////////////////////////////////////////////////////////////////////

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

    Game game(configuration_file_path);

    game.run();

    return EXIT_SUCCESS;
}
