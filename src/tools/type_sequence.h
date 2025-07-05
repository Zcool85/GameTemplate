//
// Created by Zéro Cool on 30/06/2025.
//

#ifndef TYPE_SEQUENCE_H
#define TYPE_SEQUENCE_H

#include <type_traits>

namespace tools {

    // Template permettant de déclarer une liste de type
    // Exemple : using MyListOfTypes = TypeList<Type1, Type2, Type3, ...>;
    template<typename... Elements>
    struct TypeList
    {
    };

    // /////////////////////////////////////////////////////////////////////////////////
    // /
    // / Cette section permet de vérifier qu'un type est présent dans un type_sequence
    // / Testé OK
    template<typename T, typename Ts>
    struct contains;

    // Template permettant d'identifier si un type T est présent dans une liste de type Ts
    // Le principe est d'appliquer le test std::is_same<> sur tous les couples de type
    // et d'appliquer un 'or' sur toutes les valeurs trouvé
    template<typename T, typename... Ts>
    struct contains<T, TypeList<Ts...> > : std::disjunction<std::is_same<T, Ts>...> {
    };

    template<typename T, typename... Ts>
    constexpr bool contains_v = contains<T, Ts...>::value;



    // Exemple d'usage :
    // /
    // / using MyComponentsList = TypeList<CTransform, CPosition>;
    // /
    // / bool list_contain_ctransform = contains<CTransform, MyComponentsList>::value;
    // / bool list_contain_ctransform = contains_v<CTransform, MyComponentsList>;


    // /////////////////////////////////////////////////////////////////////////////////
    // /
    // / Cette section est pour déterminer combien de type il y a dans un TypeList
    // /
    template<typename T>
    struct size;

    template<typename... Ts>
    struct size<TypeList<Ts...> > {
        static constexpr std::size_t value = sizeof...(Ts);
    };

    template<typename... Ts>
    constexpr std::size_t size_v = size<Ts...>::value;


    // Exemple d'usage :
    // /
    // / using MyComponentsList = TypeList<CTransform, CPosition>;
    // /
    // / int type_count = size<MyComponentsList>>::value;
    // / int type_count = size_v<MyComponentsList>>;


    // /////////////////////////////////////////////////////////////////////////////////
    // /
    // / Cette section est pour déterminer l'indice du type T dans un TypeList
    // /
    template<typename T, typename... Ts>
    struct index_of;

    template<typename T>
    struct index_of<T> : std::integral_constant<std::int32_t, -1> {
    };

    // Spécialisation pour TypeList : on recherche T dans les éléments Ts...
    template<typename T, typename... Ts>
    struct index_of<T, TypeList<Ts...>> {
    private:
        template<typename U, typename First, typename... Rest>
        static constexpr std::int32_t find_index() {
            if constexpr (std::is_same_v<U, First>) {
                return 0;
            } else if constexpr (sizeof...(Rest) == 0) {
                return -1;
            } else {
                constexpr auto rest_index = find_index<U, Rest...>();
                return rest_index == -1 ? -1 : rest_index + 1;
            }
        }

    public:
        static constexpr std::int32_t value = find_index<T, Ts...>();
    };

    // Spécialisation récursive pour la recherche dans une liste de types
    template<typename T, typename U, typename... Us>
    struct index_of<T, U, Us...> {
        static constexpr std::int32_t value = std::is_same_v<T, U>
                                              ? 0
                                              : index_of<T, Us...>::value != -1
                                                    ? index_of<T, Us...>::value + 1
                                                    : -1;
    };

    // /////////////////////////////////////////////////////////////////////////////////
    // /
    // / Equivalent de MPL::Rename
    // /

    namespace impl {
        // Template générique pour "renommer" (transformer) un type template
        template<template<typename...> class Target, typename Source>
        struct rename;

        // Spécialisation pour TypeList
        template<template<typename...> class Target, typename... Args>
        struct rename<Target, TypeList<Args...>> {
            using type = Target<Args...>;
        };
    }

    // Alias pour faciliter l'utilisation
    template<template<typename...> class Target, typename Source>
    using rename_t = typename impl::rename<Target, Source>::type;

    // /////////////////////////////////////////////////////////////////////////////////
    // /
    // / Equivalent de MPL::Repeat
    // /
    template<int N, typename T, typename SeqWithArgs>
    struct repeat;

    template<typename T, template<typename...> class Seq, typename... Args>
    struct repeat<0, T, Seq<Args...> > {
        using type = Seq<Args...>;
    };

    template<int N, typename T, template <typename...> class Seq, typename... Args>
    struct repeat<N, T, Seq<Args...> > {
        using type = typename repeat<N - 1, T, Seq<T, Args...> >::type;
    };

    // /////////////////////////////////////////////////////////////////////////////////
    // /
    // / Equivalent de MPL::Filter
    // /

    template<typename List1, typename List2>
    struct concat;

    template<typename... T1, typename... T2>
    struct concat<TypeList<T1...>, TypeList<T2...>> {
        using type = TypeList<T1..., T2...>;
    };

    template<typename Tuple, template<typename> typename Predicate>
    struct filter;

    template<template<typename> typename Predicate>
    struct filter<TypeList<>, Predicate> {
        using type = TypeList<>;
    };

    // Spécialisation récursive pour TypeList non-vide
    template<typename Head, typename... Tail, template<typename> typename Predicate>
    struct filter<TypeList<Head, Tail...>, Predicate> {
        using type = std::conditional_t<
            Predicate<Head>::value,
            typename concat<
                TypeList<Head>,
                typename filter<TypeList<Tail...>, Predicate>::type
            >::type,
            typename filter<TypeList<Tail...>, Predicate>::type
        >;
    };

    // Alias pour faciliter l'utilisation
    template<typename Sequence, template<typename> typename Predicate>
    using filter_t = typename filter<Sequence, Predicate>::type;

}

#endif //TYPE_SEQUENCE_H
