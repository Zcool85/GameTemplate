//
// Created by Zéro Cool on 22/06/2025.
//

#ifndef TYPES_H
#define TYPES_H

// Source: https://gist.github.com/Lee-R/3839813
constexpr auto fnv1a_32(char const *s, const std::size_t count) -> std::size_t { // NOLINT(*-no-recursion)
    return count ? (fnv1a_32(s, count - 1) ^ static_cast<std::size_t>(s[count - 1])) * 16777619u : 2166136261u;
}

// Concept pour valider les types de tags
template<typename T>
concept ValidTag = std::is_empty_v<T> && std::is_trivially_constructible_v<T>;

template<ValidTag>
struct strong_id {
private:
    std::size_t value_;

public:
    // Constructeurs
    template<std::size_t N>
    constexpr explicit strong_id(const char (&str)[N])
        : value_(fnv1a_32(str, N - 1)) {
        static_assert(N > 1, "String cannot be empty");
        static_assert(N <= 64, "String too long for identifier");
    }

    constexpr strong_id() : value_(0) {
    }

    constexpr explicit strong_id(const std::size_t value) : value_(value) {
    }

    // Opérateurs de comparaison
    constexpr bool operator==(const strong_id &other) const noexcept {
        return value_ == other.value_;
    }

    constexpr bool operator!=(const strong_id &other) const noexcept {
        return value_ != other.value_;
    }

    constexpr bool operator<(const strong_id &other) const noexcept {
        return value_ < other.value_;
    }

    constexpr bool operator<=>(const strong_id &other) const noexcept {
        return value_ <=> other.value_;
    }

    [[nodiscard]] constexpr std::size_t value() const noexcept {
        return value_;
    }

    // // Opérateur de conversion explicite
    // constexpr explicit operator std::size_t() const noexcept {
    //     return value_;
    // }
};

// Spécialisation std::hash pour unordered_map
template<ValidTag TTag>
struct std::hash<strong_id<TTag> > {
    constexpr std::size_t operator()(const strong_id<TTag> &id) const noexcept {
        return id.value();
    }
};

struct SceneTag {
};

struct ActionNameTag {
};

struct ActionTypeTag {
};

using SceneId = strong_id<SceneTag>;
using ActionNameId = strong_id<ActionNameTag>;
using ActionTypeId = strong_id<ActionTypeTag>;

constexpr SceneId operator""_scene(const char *s, const std::size_t count) {
    return SceneId(fnv1a_32(s, count));
}

constexpr ActionNameId operator""_action_name(const char *s, const std::size_t count) {
    return ActionNameId(fnv1a_32(s, count));
}

constexpr ActionTypeId operator""_action_type(const char *s, const std::size_t count) {
    return ActionTypeId(fnv1a_32(s, count));
}


constexpr void compile_time_test() {
    SceneId test{"toto"};
    auto id0{"Wazzaa"_scene};
    auto id1{"toto"_action_name};
    auto id3{"toto"_action_name};

    static_assert(std::is_same_v<decltype(test), decltype(id0)>);
    static_assert(!std::is_same_v<decltype(test), decltype(id1)>);
    assert(test != id0);
    assert(id1 == id3);

    std::unordered_map<SceneId, std::string> ids{{id0, "test"}};

    ids[id0] = "test";

    constexpr auto id = "TestScene"_scene;
    constexpr auto hash_value = id.value();
    static_assert(hash_value != 0, "Hash should not be zero");
}


#endif //TYPES_H
