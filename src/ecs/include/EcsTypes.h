//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef ECS_TYPES_H
#define ECS_TYPES_H

#include "impl/Tags.h"
#include "tools/StrongTypedef.h"
#include "tools/TypeList.h"

namespace ecs {
    template<typename... Ts>
    using ComponentList = tools::TypeList<Ts...>;
    template<typename... Ts>
    using TagList = tools::TypeList<Ts...>;
    template<typename... Ts>
    using Signature = tools::TypeList<Ts...>;
    template<typename... Ts>
    using SignatureList = tools::TypeList<Ts...>;

    using EntityIndex = tools::strong_typedef<std::size_t, impl::EntityIndexTag>;

    // TODO : Pour ces 3 autres types sont internal...
    using DataIndex = tools::strong_typedef<std::size_t, impl::DataIndexTag>;
    using HandleDataIndex = tools::strong_typedef<std::size_t, impl::HandleDataIndexTag>;
    using Counter = tools::strong_typedef<int, impl::CounterTag>;

    struct Handle {
        // TODO : Faire en sorte que le contenu ne puisse pas être visible pour le développeur
        HandleDataIndex handleDataIndex;
        Counter counter;
    };
}

#endif //ECS_TYPES_H
