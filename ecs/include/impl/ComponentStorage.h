//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef ECS_IMPL_COMPONENT_STORAGE_H
#define ECS_IMPL_COMPONENT_STORAGE_H

#include <tuple>
#include <vector>

#include "../EcsTypes.h"
#include "../tools/ForEachType.h"
#include "../tools/TypeList.h"

namespace ecs::impl {

    /**
     * Classe de stockage de tous les Components
     *
     * @tparam TSettings Paramétrage ECS
     */
    template<typename TSettings>
    class ComponentStorage
    {
        // Settings = Settings<ComponentList, TagList, SignatureList>
        using Settings = TSettings;
        // ComponentList = TypeList<C0, C1, C2, ...>
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
        /**
         * Méthode permettant d'agrandir tous les vecteurs de tous les composants
         * @param new_capacity Nouvelle taille attendue
         */
        auto grow(std::size_t new_capacity) -> void {
            tools::for_each_type(vectors, [new_capacity](auto &v) {
                v.resize(new_capacity);
            });
        }

        /**
         * Méthode permettant de récupérer l'instance du Composant en fonction de son type et de son index
         * @tparam TComponent Type de composant à récupérer
         * @param index Index du composant dans son vecteur de composant
         * @return Référence du composant retrouvé
         */
        template<typename TComponent>
        auto& getComponent(DataIndex index) noexcept
        {
            static_assert(tools::contains_v<TComponent, ComponentList>);

            // We need to get the correct vector, depending on `T`.
            // Compile-time recursion? Nah.

            // Let's use C++14's `std::get` instead!
            return std::get<std::vector<TComponent>>(vectors)[index.get()];
        }
    };

}

#endif //ECS_IMPL_COMPONENT_STORAGE_H
