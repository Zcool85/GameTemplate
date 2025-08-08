//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef ECS_IMPL_SIGNATURE_BITSETS_H
#define ECS_IMPL_SIGNATURE_BITSETS_H

#include <tuple>

#include "../tools/TypeList.h"

namespace ecs::impl {

    /**
     * Classe permettant de filtrer sur les bitset des signatures
     *
     * @tparam TSettings Paramétrage ECS
     */
    template<typename TSettings>
    struct SignatureBitsets
    {
        // Settings = Settings<ComponentList, TagList, SignatureList>
        using Settings = TSettings;
        // ThisType = SignatureBitsets<Settings<ComponentList, TagList, SignatureList>>
        using ThisType = SignatureBitsets;
        // SignatureList = TypeList<ecs::Signature<C0, C1>, ecs::Signature<C0, C3, ...>, ...>
        using SignatureList = typename Settings::SignatureList;
        // Bitset = std::bitset<componentCount() + tagCount()>
        using Bitset = typename Settings::Bitset;

        // BitsetStorage = TypeList<Bitset, Bitset, ...>>
        // BitsetStorage = TypeList<std::bitset<componentCount() + tagCount()>, std::bitset<componentCount() + tagCount()>, ...>>
        using BitsetStorage = typename tools::repeat<Settings::signatureCount(), Bitset, std::tuple<>>::type;

        /**
         * Indique si le type permet un filtre sur les composants
         * @tparam TComponent Type de composant
         */
        template<typename TComponent>
        using IsComponentFilter = std::integral_constant
        <
            bool, Settings::template isComponent<TComponent>()
        >;

        /**
         * Indique si le type permet un filtre sur les tags
         * @tparam TTag Type de tag
         */
        template<typename TTag>
        using IsTagFilter = std::integral_constant
        <
            bool, Settings::template isTag<TTag>()
        >;

        /**
         * Retourne le tools::TypeList des composants appartenant à la signature.
         *
         * @tparam TSignature Type de signature à utiliser
         */
        template<typename TSignature>
        using SignatureComponents = tools::filter_t
        <
            TSignature,
            IsComponentFilter
        >;

        /**
         * Retourne le tools::TypeList des Tags appartenant à la signature.
         *
         * @tparam TSignature Type de signature à utiliser
         */
        template<typename TSignature>
        using SignatureTags = tools::filter_t
        <
            TSignature,
            IsTagFilter
        >;
    };

}

#endif //ECS_IMPL_SIGNATURE_BITSETS_H
