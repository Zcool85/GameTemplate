//
// Created by Zéro Cool on 30/06/2025.
//

#ifndef ECS_SETTINGS_H
#define ECS_SETTINGS_H

#include <bitset>

#include "impl/SignatureBitsets.h"
#include "impl/SignatureBitsetsStorage.h"
#include "tools/TypeList.h"

namespace ecs {

    /**
     * @brief Classe de paramétrage utilisée par le Framework ECS
     *
     * Ce template permet de créer les settings utilisé par le Framework ECS.
     * Les settings contiennent la déclaration de tous les components, tous
     * les tags et toutes les signatures utilisées pour les filtres.
     *
     * @tparam TComponentList Liste des composants
     * @tparam TTagList Liste des tags
     * @tparam TSignatureList Liste des signatures (ecs::Signature<C0, C1, C2, ...>)
     */
    template
    <
        typename TComponentList,
        typename TTagList,
        typename TSignatureList
    >
    struct Settings
    {
        // ComponentList = TypeList<C0, C1, C2, ...>
        using ComponentList = struct TComponentList::TypeList;
        // TagList = TypeList<T0, T1, T2, ...>
        using TagList = struct TTagList::TypeList;
        // SignatureList = TypeList<ecs:Signature<>, ecs:Signature<S0, S1>, ecs:Signature<S0, S3, ...>, ...>
        using SignatureList = struct TSignatureList::TypeList;
        using ThisType = Settings<ComponentList, TagList, SignatureList>;

        // SignatureBitsets = SignatureBitsets<
        //    Settings<
        //       TypeList<C0, C1, C2, ...>,
        //       TypeList<T0, T1, T2, ...>,
        //       TypeList<ecs:Signature<>, ecs:Signature<S0, S1>, ecs:Signature<S0, S3, ...>, ...>
        //    >
        // >
        using SignatureBitsets = impl::SignatureBitsets<ThisType>;
        // SignatureBitsetsStorage = SignatureBitsetsStorage<
        //    Settings<
        //       TypeList<C0, C1, C2, ...>,
        //       TypeList<T0, T1, T2, ...>,
        //       TypeList<ecs:Signature<>, ecs:Signature<S0, S1>, ecs:Signature<S0, S3, ...>, ...>
        //    >
        // >
        using SignatureBitsetsStorage = impl::SignatureBitsetsStorage<ThisType>;

        /**
         * Vérifie si un type donné est présent dans la liste des composants
         * @tparam T Type à contrôler
         * @return true si le type donné est bien présent dans la liste des composants
         */
        template<typename T>
        static constexpr bool isComponent() noexcept
        {
            return tools::contains_v<T, ComponentList>;
        }

        /**
         * Vérifie si un type donné est présent dans la liste des tags
         * @tparam T Type à contrôler
         * @return true si le type donné est bien présent dans la liste des tags
         */
        template<typename T>
        static constexpr bool isTag() noexcept
        {
            return tools::contains_v<T, TagList>;
        }

        /**
         * Vérifie si un type donné est présent dans la liste des signatures
         * @tparam T Type à contrôler
         * @return true si le type donné est bien présent dans la liste des signatures
         */
        template<typename T>
        static constexpr bool isSignature() noexcept
        {
            return tools::contains_v<T, SignatureList>;
        }

        /**
         * Récupère le nombre de composants
         * @return Nombre de composants
         */
        static constexpr std::int32_t componentCount() noexcept
        {
            return tools::size<ComponentList>::value;
        }

        /**
         * Récupère le nombre de tags
         * @return Nombre de tags
         */
        static constexpr std::int32_t tagCount() noexcept
        {
            return tools::size<TagList>::value;
        }

        /**
         * Récupère le nombre de signatures
         * @return Nombre signatures
         */
        static constexpr std::int32_t signatureCount() noexcept
        {
            return tools::size<SignatureList>::value;
        }

        /**
         * Récupère l'indice du composant dans la liste des composants
         * @tparam T Type de composant
         * @return Identifiant unique du composant dans la liste (son indice); -1 si non
         * présent dans la liste
         */
        template<typename T>
        static constexpr std::int32_t componentID() noexcept
        {
            return tools::index_of<T, ComponentList>::value;
        }

        /**
         * Récupère l'indice du tag dans la liste des tags
         * @tparam T Type de tag
         * @return Identifiant unique du tag dans la liste (son indice); -1 si non
         * présent dans la liste
         */
        template<typename T>
        static constexpr std::int32_t tagID() noexcept
        {
            return tools::index_of<T, TagList>::value;
        }

        /**
         * Récupère l'indice de la signature dans la liste des signatures
         * @tparam T Type de signature
         * @return Identifiant unique de la signature dans la liste (son indice); -1 si non
         * présent dans la liste
         */
        template<typename T>
        static constexpr std::int32_t signatureID() noexcept
        {
            return tools::index_of<T, SignatureList>::value;
        }

        using Bitset = std::bitset<componentCount() + tagCount()>;

        /**
         * Récupère l'indice du bit correspondant au composant dans le Bitset
         * @tparam T Type de composant
         * @return Indice du bit dans le Bitset; -1 si non trouvé
         */
        template<typename T>
        static constexpr std::int32_t componentBit() noexcept
        {
            return componentID<T>();
        }

        /**
         * Récupère l'indice du bit correspondant au tag dans le Bitset
         * @tparam T Type de tag
         * @return Indice du bit dans le Bitset; -1 si non trouvé
         */
        template<typename T>
        static constexpr std::int32_t tagBit() noexcept
        {
            if constexpr (tagID<T>() < 0) {
                return -1;
            }

            return componentCount() + tagID<T>();
        }
    };

}

#endif //ECS_SETTINGS_H
