//
// Created by Zéro Cool on 06/07/2025.
//

#include <iostream>

#include "Ecs.h"


// Components :
//   Simple, logic-less, data-containing struct.

struct CTransform {
    int x;
    CTransform() : x(0) { }
    explicit CTransform(const int value) : x(value) { }
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


template<typename... Ts>
using TupleOfVectors = std::tuple<std::vector<Ts>...>;

void func() {
    ecs::tools::rename_t<TupleOfVectors, MyComponentsList> vectors;

    static_assert(std::is_same_v<
        std::tuple<std::vector<CTransform>, std::vector<CPosition> >,
        ecs::tools::rename_t<TupleOfVectors, MyComponentsList>
    >);

    std::size_t mNewCapacity = 5;

    ecs::tools::for_each_type(vectors, [mNewCapacity](auto &v) {
        v.resize(mNewCapacity);
        std::cout << typeid(v).name() << " grow to " << mNewCapacity << std::endl;
    });
}


template<typename T>
using IsIntegral = std::is_integral<T>;


template<typename T>
class test_for {
    template<typename U>
    static void test() noexcept {
        std::cout << typeid(U).name() << " IN TEST SIGNATURE " << std::endl;
    }

public:
    test_for() noexcept {
        ecs::tools::for_each_type<T>([this]<typename Z>() {
            std::cout << typeid(Z).name() << " SIGNATURE " << std::endl;
            this->test<Z>();
        });
    }
};

void func2() {
    test_for<MyTagList> test;
}

int main() {
    func();
    func2();

    //
    // Check Settings structure
    //
    using MySettings = ecs::Settings<MyComponentsList, MyTagList, MySignatureList>;

    static_assert(ecs::tools::size<MyComponentsList>::value == 2);
    static_assert(ecs::tools::size<S0>::value == 0);
    static_assert(ecs::tools::size<S1>::value == 2);
    static_assert(ecs::tools::size<S2>::value == 3);
    static_assert(ecs::tools::size<S3>::value == 4);

    static_assert(ecs::tools::size_v<MyComponentsList> == 2);
    static_assert(ecs::tools::size_v<S0> == 0);
    static_assert(ecs::tools::size_v<S1> == 2);
    static_assert(ecs::tools::size_v<S2> == 3);
    static_assert(ecs::tools::size_v<S3> == 4);

    static_assert(std::is_same_v<ecs::tools::TypeList<CTransform, CPosition>, MySettings::ComponentList>,
                  "Must have components list");
    static_assert(std::is_same_v<ecs::tools::TypeList<Tag0, Tag1, TPlayer>, MySettings::TagList>,
                  "Must have tags list");
    static_assert(std::is_same_v<ecs::tools::TypeList<
                      ecs::Signature<>,
                      ecs::Signature<CTransform, CVelocity>,
                      ecs::Signature<CTransform, CPosition, Tag0>,
                      ecs::Signature<CVelocity, Tag0, CPosition, Tag1>
                  >, MySettings::SignatureList>,
                  "Must have signature list");

    static_assert(std::is_same_v<
                      ecs::impl::SignatureBitsets<
                          ecs::Settings<
                              ecs::tools::TypeList<CTransform, CPosition>,
                              ecs::tools::TypeList<Tag0, Tag1, TPlayer>,
                              ecs::tools::TypeList<
                                  ecs::Signature<>,
                                  ecs::Signature<CTransform, CVelocity>,
                                  ecs::Signature<CTransform, CPosition, Tag0>,
                                  ecs::Signature<CVelocity, Tag0, CPosition, Tag1>
                              >
                          >
                      >,
                      MySettings::SignatureBitsets>,
                  "Must have signature bitsets");

    static_assert(std::is_same_v<
                      ecs::impl::SignatureBitsetsStorage<
                          ecs::Settings<
                              ecs::tools::TypeList<CTransform, CPosition>,
                              ecs::tools::TypeList<Tag0, Tag1, TPlayer>,
                              ecs::tools::TypeList<
                                  ecs::Signature<>,
                                  ecs::Signature<CTransform, CVelocity>,
                                  ecs::Signature<CTransform, CPosition, Tag0>,
                                  ecs::Signature<CVelocity, Tag0, CPosition, Tag1>
                              >
                          >
                      >,
                      MySettings::SignatureBitsetsStorage>,
                  "Must have SignatureBitsetsStorage");

    static_assert(MySettings::isComponent<CTransform>(), "");
    static_assert(MySettings::isComponent<CPosition>(), "");
    static_assert(!MySettings::isComponent<Tag0>(), "");
    static_assert(!MySettings::isComponent<Tag1>(), "");
    static_assert(!MySettings::isComponent<TPlayer>(), "");
    static_assert(!MySettings::isComponent<ecs::Signature<> >(), "");
    static_assert(!MySettings::isComponent<ecs::Signature<CTransform, CVelocity> >(), "");
    static_assert(!MySettings::isComponent<ecs::Signature<CTransform, CPosition, Tag0> >(), "");
    static_assert(!MySettings::isComponent<ecs::Signature<CVelocity, Tag0, CPosition, Tag1> >(), "");

    static_assert(!MySettings::isTag<CTransform>(), "");
    static_assert(!MySettings::isTag<CPosition>(), "");
    static_assert(MySettings::isTag<Tag0>(), "");
    static_assert(MySettings::isTag<Tag1>(), "");
    static_assert(MySettings::isTag<TPlayer>(), "");
    static_assert(!MySettings::isTag<ecs::Signature<> >(), "");
    static_assert(!MySettings::isTag<ecs::Signature<CTransform, CVelocity> >(), "");
    static_assert(!MySettings::isTag<ecs::Signature<CTransform, CPosition, Tag0> >(), "");
    static_assert(!MySettings::isTag<ecs::Signature<CVelocity, Tag0, CPosition, Tag1> >(), "");

    static_assert(!MySettings::isSignature<CTransform>(), "");
    static_assert(!MySettings::isSignature<CPosition>(), "");
    static_assert(!MySettings::isSignature<Tag0>(), "");
    static_assert(!MySettings::isSignature<Tag1>(), "");
    static_assert(!MySettings::isSignature<TPlayer>(), "");
    static_assert(MySettings::isSignature<ecs::Signature<> >(), "");
    static_assert(MySettings::isSignature<ecs::Signature<CTransform, CVelocity> >(), "");
    static_assert(MySettings::isSignature<ecs::Signature<CTransform, CPosition, Tag0> >(), "");
    static_assert(MySettings::isSignature<ecs::Signature<CVelocity, Tag0, CPosition, Tag1> >(), "");

    static_assert(MySettings::componentCount() == 2, "");
    static_assert(MySettings::tagCount() == 3, "");
    static_assert(MySettings::signatureCount() == 4, "");

    static_assert(MySettings::componentID<CTransform>() == 0, "");
    static_assert(MySettings::componentID<CPosition>() == 1, "");
    static_assert(MySettings::componentID<Tag0>() == -1, "");
    static_assert(MySettings::componentID<Tag1>() == -1, "");
    static_assert(MySettings::componentID<TPlayer>() == -1, "");
    static_assert(MySettings::componentID<ecs::Signature<> >() == -1, "");
    static_assert(MySettings::componentID<ecs::Signature<CTransform, CVelocity> >() == -1, "");
    static_assert(MySettings::componentID<ecs::Signature<CTransform, CPosition, Tag0> >() == -1, "");
    static_assert(MySettings::componentID<ecs::Signature<CVelocity, Tag0, CPosition, Tag1> >() == -1, "");

    static_assert(MySettings::tagID<CTransform>() == -1, "");
    static_assert(MySettings::tagID<CPosition>() == -1, "");
    static_assert(MySettings::tagID<Tag0>() == 0, "");
    static_assert(MySettings::tagID<Tag1>() == 1, "");
    static_assert(MySettings::tagID<TPlayer>() == 2, "");
    static_assert(MySettings::tagID<ecs::Signature<> >() == -1, "");
    static_assert(MySettings::tagID<ecs::Signature<CTransform, CVelocity> >() == -1, "");
    static_assert(MySettings::tagID<ecs::Signature<CTransform, CPosition, Tag0> >() == -1, "");
    static_assert(MySettings::tagID<ecs::Signature<CVelocity, Tag0, CPosition, Tag1> >() == -1, "");

    static_assert(MySettings::signatureID<CTransform>() == -1, "");
    static_assert(MySettings::signatureID<CPosition>() == -1, "");
    static_assert(MySettings::signatureID<Tag0>() == -1, "");
    static_assert(MySettings::signatureID<Tag1>() == -1, "");
    static_assert(MySettings::signatureID<TPlayer>() == -1, "");
    static_assert(MySettings::signatureID<ecs::Signature<> >() == 0, "");
    static_assert(MySettings::signatureID<ecs::Signature<CTransform, CVelocity> >() == 1, "");
    static_assert(MySettings::signatureID<ecs::Signature<CTransform, CPosition, Tag0> >() == 2, "");
    static_assert(MySettings::signatureID<ecs::Signature<CVelocity, Tag0, CPosition, Tag1> >() == 3, "");

    static_assert(std::is_same_v<std::bitset<5>, MySettings::Bitset>, "");

    static_assert(MySettings::componentBit<CTransform>() == 0, "");
    static_assert(MySettings::componentBit<CPosition>() == 1, "");
    static_assert(MySettings::componentBit<Tag0>() == -1, "");
    static_assert(MySettings::componentBit<Tag1>() == -1, "");
    static_assert(MySettings::componentBit<TPlayer>() == -1, "");
    static_assert(MySettings::componentBit<ecs::Signature<> >() == -1, "");
    static_assert(MySettings::componentBit<ecs::Signature<CTransform, CVelocity> >() == -1, "");
    static_assert(MySettings::componentBit<ecs::Signature<CTransform, CPosition, Tag0> >() == -1, "");
    static_assert(MySettings::componentBit<ecs::Signature<CVelocity, Tag0, CPosition, Tag1> >() == -1, "");

    static_assert(MySettings::tagBit<CTransform>() == -1, "");
    static_assert(MySettings::tagBit<CPosition>() == -1, "");
    static_assert(MySettings::tagBit<Tag0>() == 2, "");
    static_assert(MySettings::tagBit<Tag1>() == 3, "");
    static_assert(MySettings::tagBit<TPlayer>() == 4, "");
    static_assert(MySettings::tagBit<ecs::Signature<> >() == -1, "");
    static_assert(MySettings::tagBit<ecs::Signature<CTransform, CVelocity> >() == -1, "");
    static_assert(MySettings::tagBit<ecs::Signature<CTransform, CPosition, Tag0> >() == -1, "");
    static_assert(MySettings::tagBit<ecs::Signature<CVelocity, Tag0, CPosition, Tag1> >() == -1, "");

    // TODO : Il faudrait faire en sorte que chaque type soit unique dans les listes


    //
    // Check SignatureBitsets structure
    //
    static_assert(std::is_same_v<MySettings, MySettings::SignatureBitsets::Settings>, "");
    static_assert(std::is_same_v<MySettings::SignatureBitsets, MySettings::SignatureBitsets::ThisType>, "");
    static_assert(std::is_same_v<MySettings::SignatureList, MySettings::SignatureBitsets::SignatureList>, "");
    static_assert(std::is_same_v<MySettings::Bitset, MySettings::SignatureBitsets::Bitset>, "");
    static_assert(std::is_same_v<std::bitset<5>, MySettings::Bitset>, "");
    static_assert(std::is_same_v<std::bitset<5>, MySettings::SignatureBitsets::Bitset>, "");

    // Le tuple doit contenir autant de MySettings::Bitset que de signature dans les Settings
    static_assert(std::is_same_v<
                      std::tuple<
                          std::bitset<5>,
                          std::bitset<5>,
                          std::bitset<5>,
                          std::bitset<5>
                      >,
                      MySettings::SignatureBitsets::BitsetStorage>, "");

    // Le tuple doit contenir autant de MySettings::Bitset que de signature dans les Settings
    static_assert(std::is_same_v<
                      std::tuple<
                          MySettings::Bitset,
                          MySettings::Bitset,
                          MySettings::Bitset,
                          MySettings::Bitset
                      >,
                      MySettings::SignatureBitsets::BitsetStorage>, "");

    static_assert(MySettings::SignatureBitsets::IsComponentFilter<CTransform>::value, "");
    static_assert(MySettings::SignatureBitsets::IsComponentFilter<CPosition>::value, "");
    static_assert(!MySettings::SignatureBitsets::IsComponentFilter<Tag0>::value, "");
    static_assert(!MySettings::SignatureBitsets::IsComponentFilter<Tag1>::value, "");
    static_assert(!MySettings::SignatureBitsets::IsComponentFilter<TPlayer>::value, "");
    static_assert(!MySettings::SignatureBitsets::IsComponentFilter<ecs::Signature<> >::value, "");
    static_assert(!MySettings::SignatureBitsets::IsComponentFilter<ecs::Signature<CTransform, CVelocity> >::value, "");
    static_assert(!MySettings::SignatureBitsets::IsComponentFilter<ecs::Signature<CTransform, CPosition, Tag0> >::value,
                  "");
    static_assert(
        !MySettings::SignatureBitsets::IsComponentFilter<ecs::Signature<CVelocity, Tag0, CPosition, Tag1> >::value, "");

    static_assert(!MySettings::SignatureBitsets::IsTagFilter<CTransform>::value, "");
    static_assert(!MySettings::SignatureBitsets::IsTagFilter<CPosition>::value, "");
    static_assert(MySettings::SignatureBitsets::IsTagFilter<Tag0>::value, "");
    static_assert(MySettings::SignatureBitsets::IsTagFilter<Tag1>::value, "");
    static_assert(MySettings::SignatureBitsets::IsTagFilter<TPlayer>::value, "");
    static_assert(!MySettings::SignatureBitsets::IsTagFilter<ecs::Signature<> >::value, "");
    static_assert(!MySettings::SignatureBitsets::IsTagFilter<ecs::Signature<CTransform, CVelocity> >::value, "");
    static_assert(!MySettings::SignatureBitsets::IsTagFilter<ecs::Signature<CTransform, CPosition, Tag0> >::value, "");
    static_assert(!MySettings::SignatureBitsets::IsTagFilter<ecs::Signature<CVelocity, Tag0, CPosition, Tag1> >::value,
                  "");

    static_assert(std::is_same_v<ecs::tools::TypeList<
                      >,
                      MySettings::SignatureBitsets::SignatureComponents<S0> >, "");

    static_assert(std::is_same_v<ecs::tools::TypeList<
                          CTransform
                          // CVelocity // CVelocity n'est pas dans la ComponentList globale, il ne peut donc pas être dans la liste de sortie
                      >,
                      MySettings::SignatureBitsets::SignatureComponents<S1> >, "");

    static_assert(std::is_same_v<ecs::tools::TypeList<
                          CTransform,
                          CPosition
                      >,
                      MySettings::SignatureBitsets::SignatureComponents<S2> >, "");

    static_assert(std::is_same_v<ecs::tools::TypeList<
                          // CVelocity // CVelocity n'est pas dans la ComponentList globale, il ne peut donc pas être dans la liste de sortie
                          CPosition
                      >,
                      MySettings::SignatureBitsets::SignatureComponents<S3> >, "");


    static_assert(std::is_same_v<ecs::tools::TypeList<
                      >,
                      MySettings::SignatureBitsets::SignatureTags<S0> >, "");

    static_assert(std::is_same_v<ecs::tools::TypeList<
                      >,
                      MySettings::SignatureBitsets::SignatureTags<S1> >, "");

    static_assert(std::is_same_v<ecs::tools::TypeList<
                          Tag0
                      >,
                      MySettings::SignatureBitsets::SignatureTags<S2> >, "");

    static_assert(std::is_same_v<ecs::tools::TypeList<
                          Tag0,
                          Tag1
                      >,
                      MySettings::SignatureBitsets::SignatureTags<S3> >, "");


    using MyList = ecs::tools::TypeList<int, char *, char, bool, std::string>;


    using filter_types = ecs::tools::filter_t<MyList, IsIntegral>;

    static_assert(std::is_same_v<
        filter_types,
        ecs::tools::TypeList<int, char, bool>
    >);


    using test_index_of = ecs::tools::TypeList<CTransform, CVelocity, CPosition, int, ecs::Signature<CTransform> >;

    static_assert(ecs::tools::index_of<CTransform, test_index_of>::value == 0);
    static_assert(ecs::tools::index_of<CVelocity, test_index_of>::value == 1);
    static_assert(ecs::tools::index_of<CPosition, test_index_of>::value == 2);
    static_assert(ecs::tools::index_of<int, test_index_of>::value == 3);
    static_assert(ecs::tools::index_of<ecs::Signature<CTransform>, test_index_of>::value == 4);
    static_assert(ecs::tools::index_of<long, test_index_of>::value == -1);

    using repeat_test = ecs::tools::repeat<3, std::bitset<2>, std::tuple<> >::type;

    // tools::repeat
    static_assert(std::is_same_v<
        repeat_test,
        std::tuple<std::bitset<2>, std::bitset<2>, std::bitset<2> >
    >);


    std::cout << "Settings component count : " << MySettings::componentCount() << std::endl;
    std::cout << "Settings tag count : " << MySettings::tagCount() << std::endl;
    std::cout << "Settings signature count : " << MySettings::signatureCount() << std::endl;

    std::cout << "Settings contains CTransform ? " << std::boolalpha
            << MySettings::isComponent<CTransform>()
            << " (" << MySettings::componentID<CTransform>() << ")" << std::endl;

    std::cout << "Settings contains CPosition ? " << std::boolalpha
            << MySettings::isComponent<CPosition>()
            << " (" << MySettings::componentID<CPosition>() << ")" << std::endl;

    std::cout << "Settings contains CVelocity ? " << std::boolalpha
            << MySettings::isComponent<CVelocity>()
            << " (" << MySettings::componentID<CVelocity>() << ")" << std::endl;


    ecs::impl::ComponentStorage<MySettings> storage;

    storage.grow(4);
    for (std::size_t i = 0; i < 4; i++) {
        auto &t = storage.getComponent<CTransform>(ecs::DataIndex{i});

        std::cout << "CTransform(" << i << ").x = " << t.x << std::endl;
        t.x = static_cast<int>(i);
    }
    for (std::size_t i = 0; i < 4; i++) {
        auto &t = storage.getComponent<CTransform>(ecs::DataIndex{i});

        std::cout << "CTransform(" << i << ").x = " << t.x << std::endl;
    }


    // Test strong_typedef

    using TypeA = ecs::tools::strong_typedef<int, struct ATag>;
    using TypeB = ecs::tools::strong_typedef<int, struct BTag>;

    auto val0 = TypeA();
    auto val1 = TypeA{1};
    auto val2 = TypeB{2};
    val0 = 4;
    val1 = 2;
    val2 = 10;
    //val2 += 10;
    val2 += val2;

    static_assert(!std::is_same_v<TypeA, TypeB>, "Type not different");

    val2.get()++;

    std::cout << "val2 : " << val2.get() << std::endl;


    using SignatureBitsets = MySettings::SignatureBitsets;
    using BitsetStorage = SignatureBitsets::BitsetStorage;

    static_assert(
        std::is_same_v<BitsetStorage, std::tuple<std::bitset<5>, std::bitset<5>, std::bitset<5>, std::bitset<5> > >,
        "");

    using EntityManager = ecs::Manager<MySettings>;

    EntityManager mgr;

    auto player(mgr.createIndex());
    mgr.addTag<TPlayer>(player);

    auto& pos(mgr.addComponent<CPosition>(player).value);
    auto &transform(mgr.addComponent<CTransform>(player, 56).x);

    std::cout << "initial pos = " << pos << std::endl;
    std::cout << "initial transform = " << transform << std::endl;

    pos = 42;
    transform = 42;

    std::cout << "pos after set = " << pos << std::endl;
    std::cout << "transform after set = " << transform << std::endl;


    mgr.addTag<Tag0>(player);

    std::cout << "Entity count : " << mgr.getEntityCount() << std::endl;
    std::cout << "Capacity : " << mgr.getCapacity() << std::endl;

    mgr.refresh();
    mgr.printState(std::cout);

    assert(mgr.matchesSignature<S0>(player));
    assert(mgr.matchesSignature<S1>(player)); // CVelocity n'existant pas, S1 match avec tous les autres types existant
    assert(mgr.matchesSignature<S2>(player));
    assert(!mgr.matchesSignature<S3>(player));

    mgr.forEntities([&mgr](const ecs::EntityIndex entity_index) {
        std::cout << "Entity index : " << entity_index << std::endl;
        std::cout << "  Has CTransform : " << mgr.hasComponent<CTransform>(entity_index) << std::endl;
        std::cout << "  Has CPosition : " << mgr.hasComponent<CPosition>(entity_index) << std::endl;
    });


    // Rappel :
    // using S0 = ecs::Signature<>;
    // using S1 = ecs::Signature<CTransform, CVelocity>;
    // using S2 = ecs::Signature<CTransform, CPosition, Tag0>;
    // using S3 = ecs::Signature<CVelocity, Tag0, CPosition, Tag1>;

    // Ici, les paramètres sont les Components dans l'ordre de la signature (sans les tags)
    mgr.forEntitiesMatching<S2>(
        [](const ecs::EntityIndex entity_index, const CTransform &cTransform, const CPosition &cPosition) {
            std::cout << "Entity : " << entity_index << std::endl;
            std::cout << "S2 : " << cTransform.x << std::endl;
            std::cout << "S2 : " << cPosition.value << std::endl;
        });

    return EXIT_SUCCESS;
}