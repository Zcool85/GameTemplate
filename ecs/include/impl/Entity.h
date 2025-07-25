//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef ECS_IMPL_ENTITY_H
#define ECS_IMPL_ENTITY_H

#include "../EcsTypes.h"

namespace ecs::impl {

    /**
     * Structure représentant une entité
     *
     * @tparam TSettings Paramétrage ECS
     */
    template<typename TSettings>
    struct Entity
    {
        // Settings = Settings<ComponentList, TagList, SignatureList>
        using Settings = TSettings;
        // Bitset = std::bitset<componentCount() + tagCount()>
        using Bitset = typename Settings::Bitset;

        /**
         * Index du component dans le stockage des components
         */
        DataIndex dataIndex;

        /**
         *
         */
        HandleDataIndex handleDataIndex;

        /**
         * Bitset permettant de savoir si l'entité dispose d'un Component ou d'un tag.
         * Propriété dynamique qui peut changer au cours de l'exécution du traitement.
         */
        Bitset bitset;

        /**
         *
         */
        bool alive;
    };

}

#endif //ECS_IMPL_ENTITY_H
