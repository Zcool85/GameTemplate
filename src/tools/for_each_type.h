//
// Created by Zéro Cool on 29/06/2025.
//

#ifndef FOR_TUPLE_H
#define FOR_TUPLE_H

#include <tuple>

#include "type_sequence.h"

namespace tools {

    namespace impl {
        // Implémentation de for_each_type
        template<typename TypeList, typename Func>
        struct for_each_type;

        // Spécialisation pour TypeList vide (cas de base)
        template<typename Func>
        struct for_each_type<TypeList<>, Func> {
            static void apply(Func&& func) {
                // Rien à faire pour une liste vide
            }
        };

        // Spécialisation pour TypeList non-vide (récursion)
        template<typename Head, typename... Tail, typename Func>
        struct for_each_type<TypeList<Head, Tail...>, Func> {
            static void apply(Func&& func) {
                // Appliquer la fonction au premier type
                func.template operator()<Head>();

                // Récursion sur le reste de la liste
                for_each_type<TypeList<Tail...>, Func>::apply(std::forward<Func>(func));
            }
        };
    }

    // Interface publique
    template<typename TypeList, typename Func>
    void for_each_type(Func&& func) {
        impl::for_each_type<TypeList, Func>::apply(std::forward<Func>(func));
    }



    namespace impl {

        // Helper pour l'itération récursive sur les éléments du tuple
        template<std::size_t I = 0, typename FuncT, typename... Tp>
        std::enable_if_t<I == sizeof...(Tp), void>
        for_each_type_impl(std::tuple<Tp...>& t, FuncT&& f) {
            // Cas de base : arrêt de la récursion
        }

        template<std::size_t I = 0, typename FuncT, typename... Tp>
        std::enable_if_t<I < sizeof...(Tp), void>
        for_each_type_impl(std::tuple<Tp...>& t, FuncT&& f) {
            // Applique la fonction à l'élément courant
            f(std::get<I>(t));
            // Récursion sur l'élément suivant
            for_each_type_impl<I + 1, FuncT, Tp...>(t, std::forward<FuncT>(f));
        }

    }

    // Fonction principale qui démarre l'itération
    template<typename... Tp, typename FuncT>
    void for_each_type(std::tuple<Tp...>& t, FuncT&& f) {
        impl::for_each_type_impl(t, std::forward<FuncT>(f));
    }

}

#endif //FOR_TUPLE_H
