//
// Created by Zéro Cool on 18/06/2025.
//

#ifndef ECS_H
#define ECS_H

// Inspirations = https://github.com/CppCon/CppCon2015/blob/master/Tutorials/Implementation%20of%20a%20component-based%20entity%20system%20in%20modern%20C%2B%2B/Source%20Code/p3.cpp

#include <cassert>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <vector>

namespace ecs
{


// /////////////////////////////////////////////////////////////////////////////////
// /
// / Cette section permet de créer des types de données fortement typés
// /

template<typename T, typename Tag>
class strong_typedef {
    T value_;

public:
    using value_type = T;

    strong_typedef() = default;
    explicit strong_typedef(const T& val) : value_(val) {}
    explicit strong_typedef(T&& val) : value_(std::move(val)) {}

    T& get() & { return value_; }
    [[nodiscard]] const T& get() const& { return value_; }
    T&& get() && { return std::move(value_); }
    [[nodiscard]] const T&& get() const&& { return std::move(value_); }

    strong_typedef& operator+=(const strong_typedef& other)
        requires requires(T a, T b) { a += b; }
    {
        value_ += other.value_;
        return *this;
    }

    strong_typedef operator+(const strong_typedef& other) const
        requires requires(T a, T b) { a + b; }
    {
        return strong_typedef(value_ + other.value_);
    }

    // Opérateurs de comparaison
    auto operator<=>(const strong_typedef&) const = default;

    // Opérateur de pré-incrémentation (++obj)
    strong_typedef& operator++()
        requires requires(T t) { ++t; }
    {
        ++value_;
        return *this;
    }

    // Opérateur de post-incrémentation (obj++)
    strong_typedef operator++(int)
        requires requires(T t) { t++; }
    {
        strong_typedef temp(*this);
        ++value_;
        return temp;
    }

    // Opérateur de pré-décrémentation (--obj)
    strong_typedef& operator--()
        requires requires(T t) { --t; }
    {
        --value_;
        return *this;
    }

    // Opérateur de post-décrémentation (obj--)
    strong_typedef operator--(int)
        requires requires(T t) { t--; }
    {
        strong_typedef temp(*this);
        --value_;
        return temp;
    }

    // Conversion explicite vers le type sous-jacent
    //explicit operator T() const { return value_; }
    // Conversion implicite permettent d'utiliser la structure en temps qu'index de vecteur
    operator T() const { return value_; }

    strong_typedef &operator=(int i)
    {
        value_ = i;
        return *this;
    }

    // Fonction de hash pour pouvoir utiliser la donnée dans une map par exemple
    struct hash {
        std::size_t operator()(const strong_typedef& st) const {
            return std::hash<T>{}(st.value_);
        }
    };
};



// Template permettant de déclarer une liste de type
// Exemple : using MyListOfTypes = type_sequence<Type1, Type2, Type3, ...>;
template<typename... Ts>
struct type_sequence { };

// Template permettant d'exécuter une fonction pour chaque type d'un tuple
template<typename Tuple, typename F>
constexpr void forTypesInTuple(Tuple&& tuple, F&& f) {
    std::apply([&f](auto&&... elements) {
        (f(elements), ...);
    }, std::forward<Tuple>(tuple));
}



// /////////////////////////////////////////////////////////////////////////////////
// /
// / Cette section permet de vérifier qu'un type est présent dans un type_sequence
// /
template<typename T, typename T2>
struct contains;

// Template permettant d'identifier si un type T est présent dans une liste de type Ts
template<typename T, typename... Ts>
struct contains<T, type_sequence<Ts...>> : std::disjunction<std::is_same<T, Ts>...> { };


// Exemple d'usage :
// /
// / using MyComponentsList = type_sequence<CTransform, CPosition>;
// /
// / bool list_contain_ctransform = contains<CTransform, MyComponentsList>::value;



// /////////////////////////////////////////////////////////////////////////////////
// /
// / Cette section est pour déterminer combien de type il y a dans un type_sequence
// /
template<typename T>
struct size;

template<typename... Ts>
struct size<type_sequence<Ts...>> {
    static constexpr std::size_t value = sizeof...(Ts);
};



// Exemple d'usage :
// /
// / using MyComponentsList = type_sequence<CTransform, CPosition>;
// /
// / int type_count = size<MyComponentsList>>::value;



// /////////////////////////////////////////////////////////////////////////////////
// /
// / Cette section est pour déterminer l'indice du type dans un type_sequence
// /
template<typename T, typename... Ts>
struct index_of;

template<typename T>
struct index_of<T> : std::integral_constant<std::int32_t, -1> {};

template<typename T, typename... Ts>
struct index_of<T, type_sequence<Ts...>> : index_of<T, Ts...> { };

template<typename T, typename U, typename... Us>
struct index_of<T, U, Us...> {
    static constexpr std::int32_t value = std::is_same_v<T, U> ? 0 : (index_of<T, Us...>::value != -1) ? index_of<T, Us...>::value + 1 : -1;
};



// /////////////////////////////////////////////////////////////////////////////////
// /
// / raccourcis pour simplifier les usages
// /
template<typename... Ts> using ComponentList = type_sequence<Ts...>;
template<typename... Ts> using TagList = type_sequence<Ts...>;
template<typename... Ts> using Signature = type_sequence<Ts...>;
template<typename... Ts> using SignatureList = type_sequence<Ts...>;



// /////////////////////////////////////////////////////////////////////////////////
// /
// / Equivalent de MPL::Rename
// /
// template<typename... Ts>
// struct type_sequence {};

// Template générique pour "renommer" (transformer) un type template
template<template<typename...> class Target, typename Source>
struct rename;

// Spécialisation pour type_sequence
// template<template<typename...> class Target, typename... Args>
// struct rename<Target, type_sequence<Args...>> {
//     using type = Target<Args...>;
// };

// Spécialisation pour type_sequence
template<template<typename...> class Target, typename... Args>
struct rename<Target, type_sequence<Args...>> {
    using type = Target<Args...>;
};

// Alias pour faciliter l'utilisation
template<template<typename...> class Target, typename Source>
using rename_t = typename rename<Target, Source>::type;




// /////////////////////////////////////////////////////////////////////////////////
// /
// / Equivalent de MPL::Repeat
// /
// / TODO : Il y a probablement moyen de faire mieux...
template <int N, typename T, typename SeqWithArgs>
struct Append;

template <int N, typename T, template <typename...> class Seq, typename... Args >
struct Append<N, T, Seq<Args...> >
{
    using type = typename Append<N-1, T, Seq<T,Args...> >::type;
};

template <typename T, template<typename...> class Seq, typename... Args>
struct Append<0, T, Seq<Args...> >
{
    using type = Seq<Args...>;
};




// /////////////////////////////////////////////////////////////////////////////////
// /
// / Equivalent de MPL::Filter
// /

// Implémentation simple de filter
template<typename Sequence, template<typename> class Predicate>
struct filter;

// Spécialisation pour type_sequence
template<typename... Types, template<typename> class Predicate>
struct filter<type_sequence<Types...>, Predicate> {
private:
    // Helper pour filtrer récursivement
    template<typename... Results>
    struct filter_impl;

    // Cas de base : plus de types à traiter
    template<typename... Results>
    struct filter_impl<type_sequence<Results...>> {
        using type = type_sequence<Results...>;
    };

    // Cas récursif : traiter le premier type
    template<typename... Results, typename First, typename... Rest>
    struct filter_impl<type_sequence<Results...>, First, Rest...> {
        using type = std::conditional_t<
        Predicate<First>::value,
        typename filter_impl<type_sequence<Results..., First>, Rest...>::type,
        typename filter_impl<type_sequence<Results...>, Rest...>::type
        >;
    };

public:
    using type = typename filter_impl<type_sequence<>, Types...>::type;
};

// Alias pour faciliter l'utilisation
template<typename Sequence, template<typename> class Predicate>
using filter_t = typename filter<Sequence, Predicate>::type;









using DataIndex = strong_typedef<std::size_t, struct DataIndexTag>;
using EntityIndex = strong_typedef<std::size_t, struct EntityIndexTag>;
using HandleDataIndex  = strong_typedef<std::size_t, struct HandleDataIndexTag>;
using Counter = strong_typedef<int, struct CounterTag>;



// TODO : Bien comprendre ce bloc...
namespace Impl
{
template<typename TSettings>
struct SignatureBitsets
{
    using Settings = TSettings;
    using ThisType = SignatureBitsets;
    using SignatureList = typename Settings::SignatureList;
    using Bitset = typename Settings::Bitset;

    // TODO : A tester en static_assets std::is_same<..., std::tuple<Bitset, Bitset, ...>>
    using BitsetStorage = Append<Settings::signatureCount(), Bitset, std::tuple<>>::type;
    //    using BitsetStorage = std::tuple
    //    <
    //        MPL::Repeat
    //        <
    //            Settings::signatureCount(),
    //            Bitset
    //        >
    //    >;

    template<typename T>
    using IsComponentFilter = std::integral_constant
    <
        bool, Settings::template isComponent<T>()
    >;

    template<typename T>
    using IsTagFilter = std::integral_constant
    <
        bool, Settings::template isTag<T>()
    >;

    template<typename TSignature>
    using SignatureComponents = filter_t
    <
        TSignature,
        IsComponentFilter
    >;
    //    using SignatureComponents = MPL::Filter
    //    <
    //        IsComponentFilter,
    //        TSignature
    //    >;

    template<typename TSignature>
    using SignatureTags = filter_t
    <
        TSignature,
        IsTagFilter
    >;
    //    using SignatureTags = MPL::Filter
    //    <
    //        IsTagFilter,
    //        TSignature
    //    >;
};

template<typename TSettings>
class SignatureBitsetsStorage
{
    using Settings = TSettings;
    using SignatureBitsets = typename Settings::SignatureBitsets;
    using SignatureList = typename SignatureBitsets::SignatureList;
    using BitsetStorage = typename SignatureBitsets::BitsetStorage;

    BitsetStorage storage;

public:
    template<typename T>
    auto& getSignatureBitset() noexcept
    {
        static_assert(Settings::template isSignature<T>(), "");
        return std::get<Settings::template signatureID<T>()>(storage);
    }

    template<typename T>
    const auto& getSignatureBitset() const noexcept
    {
        static_assert(Settings::template isSignature<T>(), "");
        return std::get<Settings::template signatureID<T>()>(storage);
    }

private:
    template<typename T>
    void initializeBitset() noexcept
    {
        auto& b(this->getSignatureBitset<T>());

        using SignatureComponents = typename SignatureBitsets::template SignatureComponents<T>;

        using SignatureTags = typename SignatureBitsets::template SignatureTags<T>;

        forTypesInTuple<SignatureComponents>([this, &b](auto t)
                                      {
            b[Settings::template componentBit<typename decltype(t)::type>()] = true;
        });

        forTypesInTuple<SignatureTags>([this, &b](auto t)
                                {
            b[Settings::template tagBit<typename decltype(t)::type>()] = true;
        });
    }

public:
    SignatureBitsetsStorage() noexcept
    {
        forTypesInTuple<SignatureList>([this](auto t)
                                {
            this->initializeBitset<typename decltype(t)::type>();
        });
    }
};
}





template
<
    typename TComponentList,
    typename TTagList,
    typename TSignatureList
>
struct Settings
{
    using ComponentList = class TComponentList::type_sequence;
    using TagList = class TTagList::type_sequence;
    using SignatureList = class TSignatureList::type_sequence;
    using ThisType = Settings<ComponentList, TagList, SignatureList>;

    using SignatureBitsets = Impl::SignatureBitsets<ThisType>;
    using SignatureBitsetsStorage = Impl::SignatureBitsetsStorage<ThisType>;

    // "Type traits".
    template<typename T>
    static constexpr bool isComponent() noexcept
    {
        return contains<T, ComponentList>::value;
    }
    template<typename T>
    static constexpr bool isTag() noexcept
    {
        return contains<T, TagList>::value;
    }
    template<typename T>
    static constexpr bool isSignature() noexcept
    {
        return contains<T, SignatureList>::value;
    }

    // Count of the various elements.
    static constexpr std::size_t componentCount() noexcept
    {
        return size<ComponentList>::value;
    }
    static constexpr std::size_t tagCount() noexcept
    {
        return size<TagList>::value;
    }
    static constexpr std::size_t signatureCount() noexcept
    {
        return size<SignatureList>::value;
    }

    // Unique IDs for the various elements.
    template<typename T>
    static constexpr std::int32_t componentID() noexcept
    {
        return index_of<T, ComponentList>::value;
    }
    template<typename T>
    static constexpr std::int32_t tagID() noexcept
    {
        return index_of<T, TagList>::value;
    }
    template<typename T>
    static constexpr std::int32_t signatureID() noexcept
    {
        return index_of<T, SignatureList>::value;
    }

    using Bitset = std::bitset<componentCount() + tagCount()>;

    template<typename T>
    static constexpr std::size_t componentBit() noexcept
    {
        return componentID<T>();
    }
    template<typename T>
    static constexpr std::size_t tagBit() noexcept
    {
        return componentCount() + tagID<T>();
    }
};



// /////////////////////////////////////////////////////////////////////////////////
// /
// / Section liée à la définition d'une entity
// /
namespace Impl
{
template<typename TSettings>
struct Entity
{
    using Settings = TSettings;
    using Bitset = typename Settings::Bitset;

    DataIndex dataIndex;
    HandleDataIndex handleDataIndex;
    Bitset bitset;
    bool alive;
};

struct HandleData
{
    EntityIndex entityIndex;
    Counter counter;
};

struct Handle
{
    HandleDataIndex handleDataIndex;
    Counter counter;
};


// NOTE : Testé OK
template<typename TSettings>
class ComponentStorage
{
    using Settings = TSettings;
    using ComponentList = typename Settings::ComponentList;

    // We want to have a single `std::vector` for every
    // component type.

    // We know the types of the components at compile-time.
    // Therefore, we can use `std::tuple`.

    //    template<typename... Ts>
    //    using TupleOfVectors = std::tuple<std::vector<Ts>...>;
    template<typename type_sequence>
    struct transform_to_vector;

    template<template<typename...> class List, typename... Ts>
    struct transform_to_vector<List<Ts...>> {
        using type = std::tuple<std::vector<Ts>...>;
    };

    template<typename type_sequence>
    using transformed_tuple_t = typename transform_to_vector<type_sequence>::type;

    // We need to "unpack" the contents of `ComponentList` in
    // `TupleOfVectors`. We can do that using `MPL::Rename`.
    //MPL::Rename<TupleOfVectors, ComponentList> vectors;
    transformed_tuple_t<ComponentList> vectors;

    // That's it!
    // We have separate contiguous storage for all component
    // types.

    // All that's left is defining a public interface for the
    // component storage.

    // We will want to:
    // * Grow every vector.
    // * Get a component of a specific type via `DataIndex`.
public:
    void grow(std::size_t mNewCapacity)
    {
        forTypesInTuple(vectors, [this, mNewCapacity](auto& v)
                        {
            v.resize(mNewCapacity);
        });
    }

    template<typename T>
    auto& getComponent(DataIndex mI) noexcept
    {
        // We need to get the correct vector, depending on `T`.
        // Compile-time recursion? Nah.

        // Let's use C++14's `std::get` instead!

        return std::get<std::vector<T>>(vectors)[mI];
    }
};

}










template<typename TSettings>
class Manager
{
private:
    using Settings = TSettings;
    using ThisType = Manager<Settings>;
    using Bitset = typename Settings::Bitset;
    using Entity = Impl::Entity<Settings>;
    using HandleData = Impl::HandleData;

public:
    using Handle = Impl::Handle;

private:
    using SignatureBitsetsStorage = Impl::SignatureBitsetsStorage<Settings>;
    using ComponentStorage = Impl::ComponentStorage<Settings>;

    std::size_t capacity{0}, size{0}, sizeNext{0};
    std::vector<Entity> entities;
    SignatureBitsetsStorage signatureBitsets;
    ComponentStorage components;

    // Handle data will be stored in a vector, like entities.
    std::vector<HandleData> handleData;

    void growTo(std::size_t mNewCapacity)
    {
        assert(mNewCapacity > capacity);

        entities.resize(mNewCapacity);
        components.grow(mNewCapacity);

        // Do not forget to grow the new container.
        handleData.resize(mNewCapacity);

        for(auto i(capacity); i < mNewCapacity; ++i)
        {
            auto& e(entities[i]);
            auto& h(handleData[i]);

            e.dataIndex = i;
            e.bitset.reset();
            e.alive = false;

            // New entities will need to know what their
            // handle is. During initialization, it will
            // be the handle "directly below them".
            e.handleDataIndex = i;

            // New handle data instances will have to
            // be initialized with a value for their counter
            // and the index of the entity they're pointing
            // at (which will be the one "directly on top of
            // them", at that point in time).

            // Set handledata values.
            h.counter = 0;
            h.entityIndex = i;
        }

        capacity = mNewCapacity;
    }

    void growIfNeeded()
    {
        if(capacity > sizeNext) return;
        growTo((capacity + 10) * 2);
    }

    auto& getEntity(EntityIndex mI) noexcept
    {
        assert(sizeNext > mI);
        return entities[mI];
    }

    const auto& getEntity(EntityIndex mI) const noexcept
    {
        assert(sizeNext > mI);
        return entities[mI];
    }

    // We'll need some getters for `handleData`.
    // We can get `HandleData` instances in three ways:
    // * Through an `HandleDataIndex`.
    // * Through an `EntityIndex` -> `HandleDataIndex`.
    // * Through an `Handle` -> `HandleDataIndex`.

    // Some code repetition is necessary...

    auto& getHandleData(HandleDataIndex mI) noexcept
    {
        // The handle for an entity does not necessarily have to
        // be before the `sizeNext` index.
        assert(handleData.size() > mI);
        return handleData[mI];
    }
    const auto& getHandleData(HandleDataIndex mI) const noexcept
    {
        assert(handleData.size() > mI);
        return handleData[mI];
    }

    // TODO : Comment faire pour que le compilo ne voit pas EntityIndex comme un HandleDataIndex ??

    auto& getHandleData(EntityIndex mI) noexcept
    {
        return getHandleData(getEntity(mI).handleDataIndex);
    }
    const auto& getHandleData(EntityIndex mI) const noexcept
    {
        return getHandleData(getEntity(mI).handleDataIndex);
    }

    auto& getHandleData(const Handle& mX) noexcept
    {
        return getHandleData(mX.handleDataIndex);
    }
    const auto& getHandleData(const Handle& mX) const noexcept
    {
        return getHandleData(mX.handleDataIndex);
    }

public:
    Manager() { growTo(100); }

    // How to check if an handle is valid?
    // Comparing its counter to the corresponding handle data
    // counter is enough.
    auto isHandleValid(const Handle& mX) const noexcept
    {
        return mX.counter == getHandleData(mX).counter;
    }

    // All methods that we previously could call with `EntityIndex`
    // should also be possible to call using handles.

    // Let's create a method that returns an `EntityIndex` from
    // an handle valid to aid us.

    auto getEntityIndex(const Handle& mX) const noexcept
    {
        assert(isHandleValid(mX));
        return getHandleData(mX).entityIndex;
    }

    auto isAlive(EntityIndex mI) const noexcept
    {
        return getEntity(mI).alive;
    }
    auto isAlive(const Handle& mX) const noexcept
    {
        return isAlive(getEntityIndex(mX));
    }

    void kill(EntityIndex mI) noexcept
    {
        getEntity(mI).alive = false;
    }
    void kill(const Handle& mX) noexcept
    {
        kill(getEntityIndex(mX));
    }

    template<typename T>
    auto hasTag(EntityIndex mI) const noexcept
    {
        static_assert(Settings::template isTag<T>(), "");
        return getEntity(mI).bitset[Settings::template tagBit<T>()];
    }
    template<typename T>
    auto hasTag(const Handle& mX) const noexcept
    {
        return hasTag<T>(getEntityIndex(mX));
    }

    template<typename T>
    void addTag(EntityIndex mI) noexcept
    {
        static_assert(Settings::template isTag<T>(), "");
        getEntity(mI).bitset[Settings::template tagBit<T>()] = true;
    }
    template<typename T>
    void addTag(const Handle& mX) noexcept
    {
        return addTag<T>(getEntityIndex(mX));
    }

    template<typename T>
    void delTag(EntityIndex mI) noexcept
    {
        static_assert(Settings::template isTag<T>(), "");
        getEntity(mI).bitset[Settings::template tagBit<T>()] = false;
    }
    template<typename T>
    auto delTag(const Handle& mX) noexcept
    {
        return delTag<T>(getEntityIndex(mX));
    }

    template<typename T>
    auto hasComponent(EntityIndex mI) const noexcept
    {
        static_assert(Settings::template isComponent<T>(), "");
        return getEntity(mI).bitset
        [Settings::template componentBit<T>()];
    }
    template<typename T>
    auto hasComponent(const Handle& mX) const noexcept
    {
        return hasComponent<T>(getEntityIndex(mX));
    }

    template<typename T, typename... TArgs>
    auto& addComponent(EntityIndex mI, TArgs&&... mXs) noexcept
    {
        static_assert(Settings::template isComponent<T>(), "");

        auto& e(getEntity(mI));
        e.bitset[Settings::template componentBit<T>()] = true;

        auto& c(components.template getComponent<T>(e.dataIndex));
        new (&c) T(FWD(mXs)...);
        return c;
    }
    template<typename T, typename... TArgs>
    auto& addComponent(const Handle& mX, TArgs&&... mXs) noexcept
    {
        return addComponent<T>(getEntityIndex(mX), FWD(mXs)...);
    }

    // `getComponent` will simply return a reference to the
    // component, after asserting its existance.
    template<typename T>
    auto& getComponent(EntityIndex mI) noexcept
    {
        static_assert(Settings::template isComponent<T>(), "");
        assert(hasComponent<T>(mI));

        return components.
        template getComponent<T>(getEntity(mI).dataIndex);
    }
    template<typename T>
    auto& getComponent(const Handle& mX) noexcept
    {
        return getComponent<T>(getEntityIndex(mX));
    }

    template<typename T>
    void delComponent(EntityIndex mI) noexcept
    {
        static_assert(Settings::template isComponent<T>(), "");
        getEntity(mI).bitset
        [Settings::template componentBit<T>()] = false;
    }
    template<typename T>
    void delComponent(const Handle& mX) noexcept
    {
        return delComponent<T>(getEntityIndex(mX));
    }

    auto createIndex()
    {
        growIfNeeded();
        EntityIndex freeIndex(sizeNext++);

        assert(!isAlive(freeIndex));
        auto& e(entities[freeIndex]);
        e.alive = true;
        e.bitset.reset();

        return freeIndex;
    }

    // If the user does not need to track a specific entity,
    // `createIndex()` can be used.

    // Otherwise, we'll need to create a new method that
    // returns an handle.

    auto createHandle()
    {
        // Let's start by creating an entity with
        // `createIndex()`, and storing the result.
        auto freeIndex(createIndex());
        assert(isAlive(freeIndex));

        // We'll need to "match" the new entity
        // and the new handle together.
        auto& e(entities[freeIndex]);
        auto& hd(handleData[e.handleDataIndex]);

        // Let's update the entity's corresponding
        // handle data to point to the new index.
        hd.entityIndex = freeIndex;

        // Initialize a valid entity handle.
        Handle h;

        // The handle will point to the entity's
        // handle data...
        h.handleDataIndex = e.handleDataIndex;

        // ...and its validity counter will be set
        // to the handle data's current counter.
        h.counter = hd.counter;

        // Assert entity handle validity.
        assert(isHandleValid(h));

        // Return a copy of the entity handle.
        return h;
    }

    void clear() noexcept
    {
        // Let's re-initialize handles during `clear()`.

        for(auto i(0u); i < capacity; ++i)
        {
            auto& e(entities[i]);
            auto& hd(handleData[i]);

            e.dataIndex = i;
            e.bitset.reset();
            e.alive = false;
            e.handleDataIndex = i;

            hd.counter = 0;
            hd.entityIndex = i;
        }

        size = sizeNext = 0;
    }

    void refresh() noexcept
    {
        if(sizeNext == 0)
        {
            size = 0;
            return;
        }

        size = sizeNext = refreshImpl();
    }

    template<typename T>
    auto matchesSignature(EntityIndex mI) const noexcept
    {
        static_assert(Settings::template isSignature<T>(), "");

        const auto& entityBitset(getEntity(mI).bitset);
        const auto& signatureBitset(signatureBitsets.template getSignatureBitset<T>());

        return (signatureBitset & entityBitset) == signatureBitset;
    }

    template<typename TF>
    void forEntities(TF&& mFunction)
    {
        for(EntityIndex i{0}; i < size; ++i)
            mFunction(i);
    }

    template<typename T, typename TF>
    void forEntitiesMatching(TF&& mFunction)
    {
        static_assert(Settings::template isSignature<T>(), "");

        forEntities([this, &mFunction](auto i)
        {
            if(matchesSignature<T>(i))
                expandSignatureCall<T>(i, mFunction);
        });
    }

private:
    template<typename... Ts>
    struct ExpandCallHelper;

    template<typename T, typename TF>
    void expandSignatureCall(EntityIndex mI, TF&& mFunction)
    {
        static_assert(Settings::template isSignature<T>(), "");

        using RequiredComponents = typename Settings::SignatureBitsets::template SignatureComponents<T>;

        using Helper = rename_t<ExpandCallHelper, RequiredComponents>;

//        using Helper = MPL::Rename
// /        <
// /            ExpandCallHelper, RequiredComponents
// /        >;

        Helper::call(mI, *this, mFunction);
    }

    template<typename... Ts>
    struct ExpandCallHelper
    {
        template<typename TF>
        static void call(EntityIndex mI, ThisType& mMgr, TF&& mFunction)
        {
            auto di(mMgr.getEntity(mI).dataIndex);

            mFunction(mI, mMgr.components.template getComponent<Ts>(di)...);
        }
    };

private:
    // We'll need to do something when iterating over dead
    // entities during `refreshImpl()` to make sure their
    // handles get invalidated.

    // Invalidating an handle is as simple as incrementing
    // its counter.
    void invalidateHandle(EntityIndex mX) noexcept
    {
        auto& hd(handleData[entities[mX].handleDataIndex]);
        ++hd.counter;
    }

    // We'll also need that swapped alive entities' handles
    // point to the new correct indices.

    // It is sufficient to get the handle data from the entity,
    // after it has been swapped, and update its entity index
    // with the new one.
    void refreshHandle(EntityIndex mX) noexcept
    {
        auto& hd(handleData[entities[mX].handleDataIndex]);
        hd.entityIndex = mX;
    }

    auto refreshImpl() noexcept
    {
        EntityIndex iD{0}, iA{sizeNext - 1};

        while(true)
        {
            for(; true; ++iD)
            {
                if(iD > iA) return iD;
                if(!entities[iD].alive) break;

                // There is no need to invalidate or refresh
                // handles of untouched alive entities.
            }

            for(; true; --iA)
            {
                if(entities[iA].alive) break;

                // New dead entities on the right need to be
                // invalidated. Their handle index doesn't need
                // to be changed.
                invalidateHandle(iA);

                if(iA <= iD) return iD;
            }

            assert(entities[iA].alive);
            assert(!entities[iD].alive);

            std::swap(entities[iA], entities[iD]);

            // After swap, the alive entity's handle must be
            // refreshed, but not invalidated.
            refreshHandle(iD);

            // After swap, the dead entity's handle must be
            // both refreshed and invalidated.
            invalidateHandle(iA);
            refreshHandle(iA);

            ++iD; --iA;
        }

        return iD;
    }

public:
    auto getEntityCount() const noexcept
    {
        return size;
    }

    auto getCapacity() const noexcept
    {
        return capacity;
    }

    auto printState(std::ostream& mOSS) const
    {
        mOSS << "\nsize: " << size
        << "\nsizeNext: " << sizeNext
        << "\ncapacity: " << capacity
        << "\n";

        for(auto i(0u); i < sizeNext; ++i)
        {
            auto& e(entities[i]);
            mOSS << (e.alive ? "A" : "D");
        }

        mOSS << "\n\n";
    }
};

}   // End namespace ecs

// /////////////////////////////////////////////////////////////////////////////////
// / /////////////////////////////////////////////////////////////////////////////////
// / /////////////////////////////////////////////////////////////////////////////////
// / /////////////////////////////////////////////////////////////////////////////////



// TODO : Try to implement this : https://github.com/CppCon/CppCon2015/blob/master/Tutorials/Implementation%20of%20a%20component-based%20entity%20system%20in%20modern%20C%2B%2B/Source%20Code/p7.cpp



// Entity
// /   Simple struct "pointing" to the correct component data.
// /   Contains a bitset to keep track of available components
// /   and tags, and a flag to check if the entity is alive.

// Manager
// /   "Context" class. Will contain entities, components,
// /   signatures, metadata.
// /   It will deal with entity/component creation/removal,
// /   with entity iteration and much more.

// Handle
// /   Layer of indirection between the entities and the user.
// /   Handles will be used to keep track and access an entity.
// /   It will be possible to check if the entity is still valid
// /   through an existing handle.



// std::is_same_v<
// ecs::Impl::SignatureBitsets<ecs::Settings<ecs::type_sequence<CTransform, CPosition>, ecs::type_sequence<Tag0, Tag1>, ecs::type_sequence<ecs::type_sequence<>, ecs::type_sequence<CTransform, CVelocity>, ecs::type_sequence<CTransform, CPosition, Tag0>, ecs::type_sequence<CVelocity, Tag0, CPosition, Tag1>>>>, std::tuple<std::bitset<4>, std::bitset<4>, std::bitset<4>, std::bitset<4>>>









#endif //ECS_H
