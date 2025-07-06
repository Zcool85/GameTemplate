//
// Created by Zéro Cool on 27/06/2025.
//

#ifndef ECS_TOOLS_STRONG_TYPEDEF_H
#define ECS_TOOLS_STRONG_TYPEDEF_H

#include <algorithm>
#include <type_traits>

namespace ecs::tools {
    template<typename T, typename>
    class strong_typedef {
        T value_;

    public:
        strong_typedef() = default;
        explicit strong_typedef(const T& val);
        explicit strong_typedef(T&& val)
            noexcept(std::is_nothrow_move_constructible_v<T>);

        T& get() &;

        [[nodiscard]] const T& get() const&;

        T&& get() &&;

        [[nodiscard]] const T&& get() const&&;

        // Opérateurs de comparaison
        auto operator<=>(const strong_typedef&) const = default;
        auto operator<=>(const size_t& other) const;

        strong_typedef& operator+=(const strong_typedef& other)
            requires requires(T a, T b) { a += b; };

        strong_typedef operator+(const strong_typedef& other) const
            requires requires(T a, T b) { a + b; };

        // Opérateur de pré-incrémentation (++obj)
        strong_typedef& operator++()
            requires requires(T t) { ++t; };

        // Opérateur de pré-décrémentation (--obj)
        strong_typedef& operator--()
            requires requires(T t) { --t; };

        // Conversion explicite vers le type sous-jacent
        //explicit operator T() const;
        // TODO : Un jour, on trouvera une solution pour ne plus être en implicite...
        operator std::size_t() const { return static_cast<std::size_t>(value_); };

        strong_typedef &operator=(T i);

        // Fonction de hash pour pouvoir utiliser la donnée dans une map par exemple
        struct hash {
            std::size_t operator()(const strong_typedef& st) const;
        };
    };

    template<typename T, typename Tag>
    strong_typedef<T, Tag>::strong_typedef(const T &val): value_(val) {}

    template<typename T, typename Tag>
    strong_typedef<T, Tag>::strong_typedef(T &&val)
        noexcept(std::is_nothrow_move_constructible_v<T>): value_(std::move(val)) {}

    template<typename T, typename Tag>
    T & strong_typedef<T, Tag>::get() & { return value_; }

    template<typename T, typename Tag>
    const T & strong_typedef<T, Tag>::get() const & { return value_; }

    template<typename T, typename Tag>
    T && strong_typedef<T, Tag>::get() && { return std::move(value_); }

    template<typename T, typename Tag>
    const T && strong_typedef<T, Tag>::get() const && { return std::move(value_); }

    template<typename T, typename Tag>
    auto strong_typedef<T, Tag>::operator<=>(const size_t &other) const {
        return value_ <=> other;
    }

    template<typename T, typename Tag>
    strong_typedef<T, Tag> & strong_typedef<T, Tag>::operator+=(const strong_typedef &other) requires requires (T a, T b
        ) { a += b; } {
        value_ += other.value_;
        return *this;
    }

    template<typename T, typename Tag>
    strong_typedef<T, Tag> strong_typedef<T, Tag>::operator+(const strong_typedef &other) const requires requires (T a,
        T b) { a + b; } {
        return strong_typedef(value_ + other.value_);
    }

    template<typename T, typename Tag>
    strong_typedef<T, Tag> & strong_typedef<T, Tag>::operator++() requires requires (T t) { ++t; } {
        ++value_;
        return *this;
    }

    template<typename T, typename Tag>
    strong_typedef<T, Tag> & strong_typedef<T, Tag>::operator--() requires requires (T t) { --t; } {
        --value_;
        return *this;
    }

    // template<typename T, typename Tag>
    // strong_typedef<T, Tag>::operator T() const { return value_; }

    template<typename T, typename Tag>
    strong_typedef<T, Tag> &strong_typedef<T, Tag>::operator=(T i) {
        value_ = i;
        return *this;
    }

    template<typename T, typename Tag>
    std::size_t strong_typedef<T, Tag>::hash::operator()(const strong_typedef &st) const {
        return std::hash<T>{}(st.value_);
    }

}

#endif //ECS_TOOLS_STRONG_TYPEDEF_H
