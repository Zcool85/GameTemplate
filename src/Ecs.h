//
// Created by Zéro Cool on 18/06/2025.
//

#ifndef ECS_H
#define ECS_H

// Inspirations = https://github.com/CppCon/CppCon2015/blob/master/Tutorials/Implementation%20of%20a%20component-based%20entity%20system%20in%20modern%20C%2B%2B/Source%20Code/p3.cpp

#include <cassert>
#include <tuple>
#include <vector>

#include "ecs/Settings.h"
#include "tools/for_each_type.h"
#include "tools/strong_typedef.h"
#include "tools/type_sequence.h"

namespace ecs
{
    // /////////////////////////////////////////////////////////////////////////////////
    // /
    // / raccourcis pour simplifier les usages
    // /
    template<typename... Ts>
    using ComponentList = tools::TypeList<Ts...>;
    template<typename... Ts>
    using TagList = tools::TypeList<Ts...>;
    template<typename... Ts>
    using Signature = tools::TypeList<Ts...>;
    template<typename... Ts>
    using SignatureList = tools::TypeList<Ts...>;


    using DataIndex = tools::strong_typedef<std::size_t, struct DataIndexTag>;
    using EntityIndex = tools::strong_typedef<std::size_t, struct EntityIndexTag>;
    using HandleDataIndex = tools::strong_typedef<std::size_t, struct HandleDataIndexTag>;
    using Counter = tools::strong_typedef<int, struct CounterTag>;









// /////////////////////////////////////////////////////////////////////////////////
// /
// / Section liée à la définition d'une entity
// /
namespace impl {
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

    template<typename... Ts>
    using TupleOfVectors = std::tuple<std::vector<Ts>...>;

    // We need to "unpack" the contents of `ComponentList` in
    // `TupleOfVectors`.
    // On cherche ici à produire un tuple contenant des vecteurs de composants :
    // std::tuple<std::vector<C1>, std::vector<C2>, std::vector<C3>> vectors;
    tools::rename_t<TupleOfVectors, ComponentList> vectors;

    // That's it!
    // We have separate contiguous storage for all component
    // types.

    // All that's left is defining a public interface for the
    // component storage.

    // We will want to:
    // * Grow every vector.
    // * Get a component of a specific type via `DataIndex`.
public:
    void grow(std::size_t mNewCapacity) {
        tools::for_each_type(vectors, [this, mNewCapacity](auto &v) {
            v.resize(mNewCapacity);
        });
    }

    template<typename T>
    auto& getComponent(DataIndex mI) noexcept
    {
        static_assert(tools::contains_v<T, ComponentList>);

        // We need to get the correct vector, depending on `T`.
        // Compile-time recursion? Nah.

        // Let's use C++14's `std::get` instead!
        return std::get<std::vector<T> >(vectors)[mI.get()];
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
    using Entity = impl::Entity<Settings>;
    using HandleData = impl::HandleData;

public:
    using Handle = impl::Handle;

private:
    using SignatureBitsetsStorage = impl::SignatureBitsetsStorage<Settings>;
    using ComponentStorage = impl::ComponentStorage<Settings>;

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

        using Helper = tools::rename_t<ExpandCallHelper, RequiredComponents>;

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


//
// namespace std {
//
//     // template<typename T>
//     // struct tuple_size;
//
//     template<typename... Ts>
//     struct tuple_size<ecs::type_sequence<Ts...>> {
//         static constexpr std::size_t value = sizeof...(Ts);
//     };
//
// }



#endif //ECS_H
