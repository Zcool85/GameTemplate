//
// Created by Zéro Cool on 22/06/2025.
//

#ifndef TYPES_H
#define TYPES_H

// Source: https://gist.github.com/Lee-R/3839813
constexpr auto fnv1a_32(char const *s, const std::size_t count) -> std::uint32_t { // NOLINT(*-no-recursion)
    return count ? (fnv1a_32(s, count - 1) ^ static_cast<std::uint32_t>(s[count - 1])) * 16777619u : 2166136261u;
}

constexpr std::uint32_t operator "" _hash(char const *s, const std::size_t count) {
    return fnv1a_32(s, count);
}

using ActionNameId = std::uint32_t;
using ActionTypeId = std::uint32_t;

#endif //TYPES_H
