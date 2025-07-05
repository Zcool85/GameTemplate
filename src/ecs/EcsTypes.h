//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef ECS_TYPES_H
#define ECS_TYPES_H

#include "StrongTypedefTags.h"
#include "tools/strong_typedef.h"
#include "tools/type_sequence.h"

namespace ecs {

    template<typename... Ts>
    using ComponentList = tools::TypeList<Ts...>;
    template<typename... Ts>
    using TagList = tools::TypeList<Ts...>;
    template<typename... Ts>
    using Signature = tools::TypeList<Ts...>;
    template<typename... Ts>
    using SignatureList = tools::TypeList<Ts...>;


    using DataIndex = tools::strong_typedef<std::size_t, impl::DataIndexTag>;
    using EntityIndex = tools::strong_typedef<std::size_t, impl::EntityIndexTag>;
    using HandleDataIndex = tools::strong_typedef<std::size_t, impl::HandleDataIndexTag>;
    using Counter = tools::strong_typedef<int, impl::CounterTag>;

}

#endif //ECS_TYPES_H
