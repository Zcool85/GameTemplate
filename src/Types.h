//
// Created by Zéro Cool on 22/06/2025.
//

#ifndef TYPES_H
#define TYPES_H

#include <cstddef>
#include <cstdint>

// Source: https://gist.github.com/Lee-R/3839813
constexpr auto fnv1a_32(char const *s, std::size_t count) -> std::uint32_t {
    return count ? (fnv1a_32(s, count - 1) ^ s[count - 1]) * 16777619u : 2166136261u;
    //return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u; // NOLINT (hicpp-signed-bitwise)
}

constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count)
{
    return fnv1a_32(s, count);
}

using ActionNameId = std::uint32_t;
using ActionTypeId = std::uint32_t;

#endif //TYPES_H
