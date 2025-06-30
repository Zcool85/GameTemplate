//
// Created by Zéro Cool on 30/06/2025.
//

#ifndef TYPE_SEQUENCE_H
#define TYPE_SEQUENCE_H

namespace ecs {
    // Template permettant de déclarer une liste de type
    // Comme un tuple... TODO : Ne faudrait-il pas passer au std::tuple ?
    // Exemple : using MyListOfTypes = type_sequence<Type1, Type2, Type3, ...>;
    template<typename... Ts>
    struct type_sequence {
    };


    // /////////////////////////////////////////////////////////////////////////////////
    // /
    // / Cette section permet de vérifier qu'un type est présent dans un type_sequence
    // /
    template<typename T, typename T2>
    struct contains;

    // Template permettant d'identifier si un type T est présent dans une liste de type Ts
    template<typename T, typename... Ts>
    struct contains<T, type_sequence<Ts...> > : std::disjunction<std::is_same<T, Ts>...> {
    };


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
    struct size<type_sequence<Ts...> > {
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
    struct index_of<T> : std::integral_constant<std::int32_t, -1> {
    };

    template<typename T, typename... Ts>
    struct index_of<T, type_sequence<Ts...> > : index_of<T, Ts...> {
    };

    template<typename T, typename U, typename... Us>
    struct index_of<T, U, Us...> {
        static constexpr std::int32_t value = std::is_same_v<T, U>
                                                  ? 0
                                                  : (index_of<T, Us...>::value != -1)
                                                        ? index_of<T, Us...>::value + 1
                                                        : -1;
    };


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
    struct rename<Target, type_sequence<Args...> > {
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
    template<int N, typename T, typename SeqWithArgs>
    struct Append;

    template<int N, typename T, template <typename...> class Seq, typename... Args>
    struct Append<N, T, Seq<Args...> > {
        using type = typename Append<N - 1, T, Seq<T, Args...> >::type;
    };

    template<typename T, template<typename...> class Seq, typename... Args>
    struct Append<0, T, Seq<Args...> > {
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
        struct filter_impl<type_sequence<Results...> > {
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
}

#endif //TYPE_SEQUENCE_H
