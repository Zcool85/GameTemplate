//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef ECS_MANAGER_H
#define ECS_MANAGER_H

#include <iostream>
#include <cassert>

#include "impl/ComponentStorage.h"
#include "impl/Entity.h"
#include "impl/Handle.h"
#include "impl/HandleData.h"
#include "impl/SignatureBitsetsStorage.h"

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

            // Initialisation des nouvelles entités
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

                // Set HandleData values.
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
        auto getEntity(const EntityIndex index) noexcept -> Entity & {
            assert(sizeNext > index);
            return entities[index];
        }

        /**
         * Récupère une entité par son index (const)
         * @param index Index de l'entité
         * @return Référence vers l'entité désirée (const)
         */
        auto getEntity(const EntityIndex index) const noexcept -> const Entity & {
            assert(sizeNext > index);
            return entities[index];
        }

        // We'll need some getters for `handleData`.
        // We can get `HandleData` instances in three ways:
        // * Through an `HandleDataIndex`.
        // * Through an `EntityIndex` -> `HandleDataIndex`.
        // * Through an `Handle` -> `HandleDataIndex`.

        // Some code repetition is necessary...

        auto getHandleData(const HandleDataIndex index) noexcept -> HandleData & {
            // The handle for an entity does not necessarily have to
            // be before the `sizeNext` index.
            assert(handleData.size() > index);
            return handleData[index];
        }

        [[nodiscard]] auto getHandleData(const HandleDataIndex index) const noexcept -> const HandleData & {
            assert(handleData.size() > index);
            return handleData[index];
        }

        auto getHandleData(const EntityIndex index) noexcept -> HandleData & {
            return getHandleData(getEntity(index).handleDataIndex);
        }

        [[nodiscard]] auto getHandleData(const EntityIndex index) const noexcept -> const HandleData & {
            return getHandleData(getEntity(index).handleDataIndex);
        }

        auto getHandleData(const Handle &handle) noexcept -> HandleData & {
            return getHandleData(handle.handleDataIndex);
        }

        [[nodiscard]] auto getHandleData(const Handle &handle) const noexcept -> const HandleData & {
            return getHandleData(handle.handleDataIndex);
        }

    public:
        /**
         * Constructeur d'un Manager
         *
         * @param capacity Capacité initiale du Manager (Par défaut 100 entités)
         */
        explicit Manager(const std::size_t capacity = 100) { growTo(capacity); }

        // How to check if a handle is valid?
        // Comparing its counter to the corresponding handle data
        // counter is enough.
        [[nodiscard]] auto isHandleValid(const Handle &handle) const noexcept -> bool {
            return handle.counter == getHandleData(handle).counter;
        }

        // All methods that we previously could call with `EntityIndex`
        // should also be possible to call using handles.

        // Let's create a method that returns an `EntityIndex` from
        // a handle valid to aid us.

        [[nodiscard]] auto getEntityIndex(const Handle &handle) const noexcept -> EntityIndex {
            assert(isHandleValid(handle));
            return getHandleData(handle).entityIndex;
        }

        [[nodiscard]] auto isAlive(const EntityIndex entity_index) const noexcept -> bool {
            return getEntity(entity_index).alive;
        }

        [[nodiscard]] auto isAlive(const Handle &handle) const noexcept -> bool {
            return isAlive(getEntityIndex(handle));
        }

        void kill(const EntityIndex entity_index) noexcept {
            getEntity(entity_index).alive = false;
        }

        void kill(const Handle &handle) noexcept {
            kill(getEntityIndex(handle));
        }

        template<typename TTag>
        [[nodiscard]] auto hasTag(const EntityIndex entity_index) const noexcept -> bool {
            static_assert(Settings::template isTag<TTag>(), "TTag must be a Tag");
            return getEntity(entity_index).bitset[Settings::template tagBit<TTag>()];
        }

        template<typename TTag>
        [[nodiscard]] auto hasTag(const Handle &handle) const noexcept -> bool {
            static_assert(Settings::template isTag<TTag>(), "TTag must be a Tag");
            return hasTag<TTag>(getEntityIndex(handle));
        }

        template<typename TTag>
        auto addTag(const EntityIndex entity_index) noexcept -> void {
            static_assert(Settings::template isTag<TTag>(), "TTag must be a Tag");
            getEntity(entity_index).bitset[Settings::template tagBit<TTag>()] = true;
        }

        template<typename TTag>
        auto addTag(const Handle &handle) noexcept -> void {
            static_assert(Settings::template isTag<TTag>(), "TTag must be a Tag");
            addTag<TTag>(getEntityIndex(handle));
        }

        template<typename TTag>
        auto delTag(const EntityIndex entity_index) noexcept -> void {
            static_assert(Settings::template isTag<TTag>(), "TTag must be a Tag");
            getEntity(entity_index).bitset[Settings::template tagBit<TTag>()] = false;
        }

        template<typename TTag>
        auto delTag(const Handle &handle) noexcept -> void {
            static_assert(Settings::template isTag<TTag>(), "TTag must be a Tag");
            return delTag<TTag>(getEntityIndex(handle));
        }

        template<typename TComponent>
        [[nodiscard]] auto hasComponent(const EntityIndex entity_index) const noexcept -> bool {
            static_assert(Settings::template isComponent<TComponent>(), "TComponent must be a Component");
            return getEntity(entity_index).bitset[Settings::template componentBit<TComponent>()];
        }

        template<typename TComponent>
        [[nodiscard]] auto hasComponent(const Handle &handle) const noexcept -> bool {
            static_assert(Settings::template isComponent<TComponent>(), "TComponent must be a Component");
            return hasComponent<TComponent>(getEntityIndex(handle));
        }

        template<typename TComponent, typename... TArgs>
        auto addComponent(const EntityIndex entity_index, TArgs &&... mXs) noexcept -> TComponent & {
            static_assert(Settings::template isComponent<TComponent>(), "TComponent must be a Component");

            auto &e(getEntity(entity_index));
            e.bitset[Settings::template componentBit<TComponent>()] = true;

            auto &c(components.template getComponent<TComponent>(e.dataIndex));
            new(&c) TComponent(std::forward<TArgs>(mXs)...);
            return c;
        }

        template<typename TComponent, typename... TArgs>
        auto addComponent(const Handle &handle, TArgs &&... mXs) noexcept -> TComponent & {
            static_assert(Settings::template isComponent<TComponent>(), "TComponent must be a Component");
            return addComponent<TComponent>(getEntityIndex(handle), std::forward<TArgs>(mXs)...);
        }

        // `getComponent` will simply return a reference to the
        // component, after asserting its existence.
        template<typename TComponent>
        auto getComponent(const EntityIndex entity_index) noexcept -> TComponent & {
            static_assert(Settings::template isComponent<TComponent>(), "TComponent must be a Component");
            assert(hasComponent<TComponent>(entity_index));

            return components.template getComponent<TComponent>(getEntity(entity_index).dataIndex);
        }

        template<typename TComponent>
        auto getComponent(const Handle &handle) noexcept -> TComponent & {
            static_assert(Settings::template isComponent<TComponent>(), "TComponent must be a Component");
            return getComponent<TComponent>(getEntityIndex(handle));
        }

        template<typename TComponent>
        auto delComponent(const EntityIndex entity_index) noexcept -> void {
            static_assert(Settings::template isComponent<TComponent>(), "TComponent must be a Component");
            getEntity(entity_index).bitset[Settings::template componentBit<TComponent>()] = false;
        }

        template<typename TComponent>
        auto delComponent(const Handle &handle) noexcept -> void {
            static_assert(Settings::template isComponent<TComponent>(), "TComponent must be a Component");
            delComponent<TComponent>(getEntityIndex(handle));
        }

        auto createIndex() -> EntityIndex {
            growIfNeeded();
            EntityIndex freeIndex(sizeNext++);

            assert(!isAlive(freeIndex));
            auto &entity(entities[freeIndex]);
            entity.alive = true;
            entity.bitset.reset();

            return freeIndex;
        }

        // If the user does not need to track a specific entity,
        // `createIndex()` can be used.

        // Otherwise, we'll need to create a new method that
        // returns a handle.

        auto createHandle() -> Handle {
            // Let's start by creating an entity with
            // `createIndex()`, and storing the result.
            auto freeIndex(createIndex());
            assert(isAlive(freeIndex));

            // We'll need to "match" the new entity
            // and the new handle together.
            auto &entity(entities[freeIndex]);
            auto &hd(handleData[entity.handleDataIndex]);

            // Let's update the entity's corresponding
            // handle data to point to the new index.
            hd.entityIndex = freeIndex;

            // Initialize a valid entity handle.
            Handle h;

            // The handle will point to the entity's
            // handle data...
            h.handleDataIndex = entity.handleDataIndex;

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
                auto &entity(entities[i]);
                auto &[entity_index, counter](handleData[i]);

                entity.dataIndex = i;
                entity.bitset.reset();
                entity.alive = false;
                entity.handleDataIndex = i;

                counter = 0;
                entity_index = i;
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

        /**
         * Fonction permettant de déterminer si une entité correspond à une signature.
         *
         * La fonction applique un et binaire entre le bitset de l'entité et le bitset
         * de la signature.
         *
         * @tparam TSignature Signature à utiliser pour la vérification
         * @param entity_index Index de l'entité à contrôler
         * @return True si l'entité colle avec la signature
         */
        template<typename TSignature>
        [[nodiscard]] auto matchesSignature(const EntityIndex entity_index) const noexcept -> bool {
            static_assert(Settings::template isSignature<TSignature>(), "TSignature must be a Signature");

            const auto &entityBitset(getEntity(entity_index).bitset);
            const auto &signatureBitset(signatureBitsets.template getSignatureBitset<TSignature>());

            return (signatureBitset & entityBitset) == signatureBitset;
        }

        /**
         * Méthode permettant d'appliquer une fonction sur toutes les entités
         *
         * @tparam TF Type de la fonction à appliquer (void mFunction(EntityIndex index))
         * @param mFunction Fonction à appliquer
         */
        template<typename TF>
        auto forEntities(TF &&mFunction) -> void {
            for (EntityIndex entity_index{0}; entity_index < size; ++entity_index) {
                mFunction(entity_index);
            }
        }

        /**
         * Méthode permettant d'itérer sur toutes les entités correspondant à la signature.
         *
         * @tparam TSignature Signature à utiliser pour filtrer les entités
         * @tparam TF Type de la fonction à invoquer pour chaque entité
         * @param mFunction Référence de la fonction à invoquer pour chaque entité
         */
        template<typename TSignature, typename TF>
        auto forEntitiesMatching(TF &&mFunction) -> void {
            static_assert(Settings::template isSignature<TSignature>(), "TSignature must be a Signature");

            forEntities([this, &mFunction](auto entity_index) {
                if (this->template matchesSignature<TSignature>(entity_index)) {
                    this->template expandSignatureCall<TSignature>(entity_index, mFunction);
                }
            });
        }

    private:
        template<typename... TSignature>
        struct ExpandCallHelper;

        /**
         * Permet d'appeler une fonction pour une entité donnée en fonction de la signature attendue.
         *
         * @tparam TSignature Signature à utiliser
         * @tparam TF Type de la fonction à appeler
         * @param entity_index Index de l'entité sur laquelle faire l'appel
         * @param mFunction Référence vers la fonction à appeler
         */
        template<typename TSignature, typename TF>
        auto expandSignatureCall(const EntityIndex entity_index, TF &&mFunction) -> void {
            static_assert(Settings::template isSignature<TSignature>(), "TSignature must be a Signature");

            // Tous les composants de la signature
            // ATTENTION : On ne prend pas les Tags !!!
            // RequiredComponents = TypeList<C0, C1, CXX, ...>
            using RequiredComponents = typename Settings::SignatureBitsets::template SignatureComponents<TSignature>;

            // Helper = ExpandCallHelper<
            using Helper = tools::rename_t<ExpandCallHelper, RequiredComponents>;

            // NOTE : For debug purpose. All is OK
            // std::cout << "expandSignatureCall entity " << entity_index << std::endl;
            // std::cout << "   RequiredComponents " << typeid(RequiredComponents).name() << std::endl;
            // std::cout << "   Helper " << typeid(Helper).name() << std::endl;

            Helper::call(entity_index, *this, mFunction);
        }

        /**
         * Permet d'appeler la fonction avec les différents paramètres correspondant
         * aux Components composant la signature attendu (sans les tags)
         *
         * @tparam TSignature Signature à utiliser pour les paramètres de la fonction
         */
        template<typename... TSignature>
        struct ExpandCallHelper {
            template<typename TF>
            static void call(const EntityIndex entity_index, ThisType &manager, TF &&mFunction) {
                auto &data_index(manager.getEntity(entity_index).dataIndex);

                mFunction(entity_index, manager.components.template getComponent<TSignature>(data_index)...);
            }
        };

        // We'll need to do something when iterating over dead
        // entities during `refreshImpl()` to make sure their
        // handles get invalidated.

        // Invalidating a handle is as simple as incrementing
        // its counter.
        auto invalidateHandle(const EntityIndex entity_index) noexcept -> void {
            auto &hd(handleData[entities[entity_index].handleDataIndex]);
            ++hd.counter;
        }

        // We'll also need that swapped alive entities' handles
        // point to the new correct indices.

        // It is sufficient to get the handle data from the entity,
        // after it has been swapped, and update its entity index
        // with the new one.
        auto refreshHandle(const EntityIndex entity_index) noexcept -> void {
            auto &hd(handleData[entities[entity_index].handleDataIndex]);
            hd.entityIndex = entity_index;
        }

        auto refreshImpl() noexcept -> EntityIndex {
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
        [[nodiscard]] auto getEntityCount() const noexcept -> std::size_t {
            return size;
        }

        [[nodiscard]] auto getCapacity() const noexcept -> std::size_t {
            return capacity;
        }

        auto printState(std::ostream &mOSS) const -> std::ostream & {
            mOSS << std::endl
                    << "size: " << size << std::endl
                    << "sizeNext: " << sizeNext << std::endl
                    << "capacity: " << capacity << std::endl;

            for (auto i(0u); i < sizeNext; ++i) {
                auto &e(entities[i]);
                mOSS << (e.alive ? "A" : "D");
            }

            mOSS << std::endl << std::endl;
            return mOSS;
        }
    };
}

#endif //ECS_MANAGER_H
