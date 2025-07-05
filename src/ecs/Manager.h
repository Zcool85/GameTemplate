//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef MANAGER_H
#define MANAGER_H

#include <cassert>

#include "ComponentStorage.h"
#include "Entity.h"
#include "Handle.h"
#include "HandleData.h"
#include "SignatureBitsetsStorage.h"

namespace ecs {
    /**
     * Classe permettant la gestion des entités
     *
     * @tparam TSettings Paramétrage ECS
     */
    template<typename TSettings>
    class Manager {
        // Settings = Settings<ComponentList, TagList, SignatureList>
        using Settings = TSettings;
        // ThisType = Manager<Settings<ComponentList, TagList, SignatureList>>
        using ThisType = Manager;
        // Bitset = std::bitset<componentCount() + tagCount()>
        //using Bitset = typename Settings::Bitset;

        using Entity = impl::Entity<Settings>;
        using HandleData = impl::HandleData;

    public:
        using Handle = impl::Handle;

    private:
        using SignatureBitsetsStorage = impl::SignatureBitsetsStorage<Settings>;
        using ComponentStorage = impl::ComponentStorage<Settings>;

        std::size_t capacity{0}, size{0}, sizeNext{0};

        /**
         * Stockage des entités
         */
        std::vector<Entity> entities;

        /**
         * Stockage des bitset des signatures
         */
        SignatureBitsetsStorage signatureBitsets;

        /**
         * Stockage des composants de toutes les entités
         */
        ComponentStorage components;

        // Handle data will be stored in a vector, like entities.
        std::vector<HandleData> handleData;

        /**
         * Fonction permettant de faire "grossir" la capacité de stockage des
         * entités.
         *
         * @param new_capacity Nouvelle capacité
         */
        void growTo(std::size_t new_capacity) {
            assert(new_capacity > capacity);

            entities.resize(new_capacity);
            components.grow(new_capacity);

            // Do not forget to grow the new container.
            handleData.resize(new_capacity);

            // Initilisation des nouvelles entités
            for (auto i(capacity); i < new_capacity; ++i) {
                auto &e(entities[i]);
                auto &h(handleData[i]);

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

            capacity = new_capacity;
        }

        /**
         * Méthode appelée lors de la création de nouvelle entité.
         * Cette méthode augmentera la capacité si besoin.
         */
        void growIfNeeded() {
            if (capacity > sizeNext) return;
            growTo((capacity + 10) * 2);
        }

        /**
         * Récupère une entité par son index
         * @param index Index de l'entité
         * @return Référence vers l'entité désirée
         */
        auto &getEntity(EntityIndex index) noexcept {
            assert(sizeNext > index);
            return entities[index];
        }

        /**
         * Récupère une entité par son index (const)
         * @param index Index de l'entité
         * @return Référence vers l'entité désirée (const)
         */
        const auto &getEntity(EntityIndex index) const noexcept {
            assert(sizeNext > index);
            return entities[index];
        }

        // We'll need some getters for `handleData`.
        // We can get `HandleData` instances in three ways:
        // * Through an `HandleDataIndex`.
        // * Through an `EntityIndex` -> `HandleDataIndex`.
        // * Through an `Handle` -> `HandleDataIndex`.

        // Some code repetition is necessary...

        auto &getHandleData(HandleDataIndex index) noexcept {
            // The handle for an entity does not necessarily have to
            // be before the `sizeNext` index.
            assert(handleData.size() > index);
            return handleData[index];
        }

        const auto &getHandleData(HandleDataIndex index) const noexcept {
            assert(handleData.size() > index);
            return handleData[index];
        }

        auto &getHandleData(EntityIndex index) noexcept {
            return getHandleData(getEntity(index).handleDataIndex);
        }

        const auto &getHandleData(EntityIndex index) const noexcept {
            return getHandleData(getEntity(index).handleDataIndex);
        }

        auto &getHandleData(const Handle &handle) noexcept {
            return getHandleData(handle.handleDataIndex);
        }

        const auto &getHandleData(const Handle &handle) const noexcept {
            return getHandleData(handle.handleDataIndex);
        }

    public:
        /**
         * Constructeur d'un Manager
         *
         * @param capacity Capacité initiale du Manager (Par défaut 100 entités)
         */
        Manager(std::size_t capacity = 100) { growTo(capacity); }

        // How to check if an handle is valid?
        // Comparing its counter to the corresponding handle data
        // counter is enough.
        auto isHandleValid(const Handle &mX) const noexcept {
            return mX.counter == getHandleData(mX).counter;
        }

        // All methods that we previously could call with `EntityIndex`
        // should also be possible to call using handles.

        // Let's create a method that returns an `EntityIndex` from
        // an handle valid to aid us.

        auto getEntityIndex(const Handle &mX) const noexcept {
            assert(isHandleValid(mX));
            return getHandleData(mX).entityIndex;
        }

        auto isAlive(EntityIndex mI) const noexcept {
            return getEntity(mI).alive;
        }

        auto isAlive(const Handle &mX) const noexcept {
            return isAlive(getEntityIndex(mX));
        }

        void kill(EntityIndex mI) noexcept {
            getEntity(mI).alive = false;
        }

        void kill(const Handle &mX) noexcept {
            kill(getEntityIndex(mX));
        }

        template<typename T>
        auto hasTag(EntityIndex mI) const noexcept {
            static_assert(Settings::template isTag<T>(), "");
            return getEntity(mI).bitset[Settings::template tagBit<T>()];
        }

        template<typename T>
        auto hasTag(const Handle &mX) const noexcept {
            return hasTag<T>(getEntityIndex(mX));
        }

        template<typename T>
        void addTag(EntityIndex mI) noexcept {
            static_assert(Settings::template isTag<T>(), "");
            getEntity(mI).bitset[Settings::template tagBit<T>()] = true;
        }

        template<typename T>
        void addTag(const Handle &mX) noexcept {
            return addTag<T>(getEntityIndex(mX));
        }

        template<typename T>
        void delTag(EntityIndex mI) noexcept {
            static_assert(Settings::template isTag<T>(), "");
            getEntity(mI).bitset[Settings::template tagBit<T>()] = false;
        }

        template<typename T>
        auto delTag(const Handle &mX) noexcept {
            return delTag<T>(getEntityIndex(mX));
        }

        template<typename T>
        auto hasComponent(EntityIndex mI) const noexcept {
            static_assert(Settings::template isComponent<T>(), "");
            return getEntity(mI).bitset[Settings::template componentBit<T>()];
        }

        template<typename T>
        auto hasComponent(const Handle &mX) const noexcept {
            return hasComponent<T>(getEntityIndex(mX));
        }

        template<typename T, typename... TArgs>
        auto &addComponent(EntityIndex mI, TArgs &&... mXs) noexcept {
            static_assert(Settings::template isComponent<T>(), "");

            auto &e(getEntity(mI));
            e.bitset[Settings::template componentBit<T>()] = true;

            auto &c(components.template getComponent<T>(e.dataIndex));
            new(&c) T(FWD(mXs)...);
            return c;
        }

        template<typename T, typename... TArgs>
        auto &addComponent(const Handle &mX, TArgs &&... mXs) noexcept {
            return addComponent<T>(getEntityIndex(mX), FWD(mXs)...);
        }

        // `getComponent` will simply return a reference to the
        // component, after asserting its existance.
        template<typename T>
        auto &getComponent(EntityIndex mI) noexcept {
            static_assert(Settings::template isComponent<T>(), "");
            assert(hasComponent<T>(mI));

            return components.
                    template getComponent<T>(getEntity(mI).dataIndex);
        }

        template<typename T>
        auto &getComponent(const Handle &mX) noexcept {
            return getComponent<T>(getEntityIndex(mX));
        }

        template<typename T>
        void delComponent(EntityIndex mI) noexcept {
            static_assert(Settings::template isComponent<T>(), "");
            getEntity(mI).bitset
                    [Settings::template componentBit<T>()] = false;
        }

        template<typename T>
        void delComponent(const Handle &mX) noexcept {
            return delComponent<T>(getEntityIndex(mX));
        }

        auto createIndex() {
            growIfNeeded();
            EntityIndex freeIndex(sizeNext++);

            assert(!isAlive(freeIndex));
            auto &e(entities[freeIndex]);
            e.alive = true;
            e.bitset.reset();

            return freeIndex;
        }

        // If the user does not need to track a specific entity,
        // `createIndex()` can be used.

        // Otherwise, we'll need to create a new method that
        // returns an handle.

        auto createHandle() {
            // Let's start by creating an entity with
            // `createIndex()`, and storing the result.
            auto freeIndex(createIndex());
            assert(isAlive(freeIndex));

            // We'll need to "match" the new entity
            // and the new handle together.
            auto &e(entities[freeIndex]);
            auto &hd(handleData[e.handleDataIndex]);

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

        void clear() noexcept {
            // Let's re-initialize handles during `clear()`.

            for (auto i(0u); i < capacity; ++i) {
                auto &e(entities[i]);
                auto &hd(handleData[i]);

                e.dataIndex = i;
                e.bitset.reset();
                e.alive = false;
                e.handleDataIndex = i;

                hd.counter = 0;
                hd.entityIndex = i;
            }

            size = sizeNext = 0;
        }

        void refresh() noexcept {
            if (sizeNext == 0) {
                size = 0;
                return;
            }

            size = sizeNext = refreshImpl();
        }

        template<typename T>
        auto matchesSignature(EntityIndex mI) const noexcept {
            static_assert(Settings::template isSignature<T>(), "");

            const auto &entityBitset(getEntity(mI).bitset);
            const auto &signatureBitset(signatureBitsets.template getSignatureBitset<T>());

            return (signatureBitset & entityBitset) == signatureBitset;
        }

        template<typename TF>
        void forEntities(TF &&mFunction) {
            for (EntityIndex i{0}; i < size; ++i)
                mFunction(i);
        }

        template<typename T, typename TF>
        void forEntitiesMatching(TF &&mFunction) {
            static_assert(Settings::template isSignature<T>(), "");

            forEntities([this, &mFunction](auto i) {
                if (matchesSignature<T>(i))
                    expandSignatureCall<T>(i, mFunction);
            });
        }

    private:
        template<typename... Ts>
        struct ExpandCallHelper;

        template<typename T, typename TF>
        void expandSignatureCall(EntityIndex mI, TF &&mFunction) {
            static_assert(Settings::template isSignature<T>(), "");

            using RequiredComponents = typename Settings::SignatureBitsets::template SignatureComponents<T>;

            using Helper = tools::rename_t<ExpandCallHelper, RequiredComponents>;

            Helper::call(mI, *this, mFunction);
        }

        template<typename... Ts>
        struct ExpandCallHelper {
            template<typename TF>
            static void call(EntityIndex mI, ThisType &mMgr, TF &&mFunction) {
                auto di(mMgr.getEntity(mI).dataIndex);

                mFunction(mI, mMgr.components.template getComponent<Ts>(di)...);
            }
        };

        // We'll need to do something when iterating over dead
        // entities during `refreshImpl()` to make sure their
        // handles get invalidated.

        // Invalidating an handle is as simple as incrementing
        // its counter.
        void invalidateHandle(EntityIndex mX) noexcept {
            auto &hd(handleData[entities[mX].handleDataIndex]);
            ++hd.counter;
        }

        // We'll also need that swapped alive entities' handles
        // point to the new correct indices.

        // It is sufficient to get the handle data from the entity,
        // after it has been swapped, and update its entity index
        // with the new one.
        void refreshHandle(EntityIndex mX) noexcept {
            auto &hd(handleData[entities[mX].handleDataIndex]);
            hd.entityIndex = mX;
        }

        auto refreshImpl() noexcept {
            EntityIndex iD{0}, iA{sizeNext - 1};

            while (true) {
                for (; true; ++iD) {
                    if (iD > iA) return iD;
                    if (!entities[iD].alive) break;

                    // There is no need to invalidate or refresh
                    // handles of untouched alive entities.
                }

                for (; true; --iA) {
                    if (entities[iA].alive) break;

                    // New dead entities on the right need to be
                    // invalidated. Their handle index doesn't need
                    // to be changed.
                    invalidateHandle(iA);

                    if (iA <= iD) return iD;
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

                ++iD;
                --iA;
            }

            return iD;
        }

    public:
        auto getEntityCount() const noexcept {
            return size;
        }

        auto getCapacity() const noexcept {
            return capacity;
        }

        auto printState(std::ostream &mOSS) const {
            mOSS << std::endl
                    << "size: " << size << std::endl
                    << "sizeNext: " << sizeNext << std::endl
                    << "capacity: " << capacity << std::endl;

            for (auto i(0u); i < sizeNext; ++i) {
                auto &e(entities[i]);
                mOSS << (e.alive ? "A" : "D");
            }

            mOSS << "\n\n";
        }
    };
}

#endif //MANAGER_H
