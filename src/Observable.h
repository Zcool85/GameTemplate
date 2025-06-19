//
// Created by Zéro Cool on 18/06/2025.
//

#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <algorithm>
#include <concepts>
#include <functional>
#include <print>
#include <type_traits>
#include <vector>

// Concept pour vérifier qu'un type peut être observé avec un événement donné
template<typename Observer, typename Event>
concept ObserverOf = requires(Observer obs, const Event &event)
                     {
                         obs.notify(event);
                     } || requires(Observer obs, const Event &event)
                     {
                         obs(event);
                     } || std::invocable<Observer, const Event &>;

// Concept pour vérifier qu'un type est un événement valide
template<typename T>
concept Event = std::copyable<T> || std::movable<T>;

// Structure pour stocker les observers avec type erasure
template<Event EventType>
class ObserverStorage {
    std::function<void(const EventType &)> callback;

public:
    template<ObserverOf<EventType> Observer>
    explicit ObserverStorage(Observer &&obs) {
        if constexpr (requires(Observer o, const EventType &e) { o.notify(e); }) {
            callback = [obs = std::forward<Observer>(obs)](const EventType &event) mutable {
                obs.notify(event);
            };
        } else if constexpr (requires(Observer o, const EventType &e) { o(e); }) {
            callback = [obs = std::forward<Observer>(obs)](const EventType &event) mutable {
                obs(event);
            };
        } else {
            callback = std::forward<Observer>(obs);
        }
    }

    void operator()(const EventType &event) const {
        callback(event);
    }
};



template<typename T, typename... Ts>
struct index_of;

template<typename T>
struct index_of<T> : std::integral_constant<std::int32_t, -1> {};

// template<typename T, typename... Ts>
// struct index_of<T, type_sequence<Ts...>> : index_of<T, Ts...> { };

template<typename T, typename U, typename... Us>
struct index_of<T, U, Us...> {
    static constexpr std::int32_t value = std::is_same_v<T, U> ? 0 : (index_of<T, Us...>::value != -1) ? index_of<T, Us...>::value + 1 : -1;
};



// Observable principal utilisant la métaprogrammation
template<Event... EventTypes>
class Observable {
    // Tuple pour stocker les listes d'observers pour chaque type d'événement
    std::tuple<std::vector<ObserverStorage<EventTypes>>...> observers;

    // Helper pour obtenir l'index d'un type dans le pack de paramètres
    template<typename T>
    static constexpr std::size_t type_index() {
        static_assert(((std::same_as<T, EventTypes>) || ...), "Event type not supported");
        // std::size_t index = 0;
        // ((std::same_as<T, EventTypes> ? false : (++index, true)) && ...);
        // return index;

        return index_of<T, EventTypes...>::value;
    }

public:
    // Subscribe un observer pour un type d'événement spécifique
    template<Event EventType, ObserverOf<EventType> Observer>
    void subscribe(Observer &&observer)
        requires ((std::same_as<EventType, EventTypes>) || ...) {
        auto &observer_list = std::get<type_index<EventType>()>(observers);
        observer_list.emplace_back(std::forward<Observer>(observer));
    }

    // Notify tous les observers d'un type d'événement
    template<Event EventType>
    void notify(const EventType &event)
        requires ((std::same_as<EventType, EventTypes>) || ...) {
        const auto &observer_list = std::get<type_index<EventType>()>(observers);
        for (const auto &observer: observer_list) {
            observer(event);
        }
    }

    // Notify avec perfect forwarding
    template<Event EventType>
    void notify(EventType &&event)
        requires ((std::same_as<std::decay_t<EventType>, EventTypes>) || ...) {
        using DecayedType = std::decay_t<EventType>;
        const auto &observer_list = std::get<type_index<DecayedType>()>(observers);
        for (const auto &observer: observer_list) {
            observer(event);
        }
    }

    // Méthode pour obtenir le nombre d'observers pour un type d'événement
    template<Event EventType>
    [[nodiscard]] std::size_t observer_count() const
        requires ((std::same_as<EventType, EventTypes>) || ...) {
        return std::get<type_index<EventType>()>(observers).size();
    }
};

// Factory function pour créer facilement un Observable
template<Event... EventTypes>
auto make_observable() {
    return Observable<EventTypes...>{};
}

// Macro pour simplifier la création d'événements
#define DEFINE_EVENT(EventName, ...) \
struct EventName { \
__VA_ARGS__ \
}

#endif //OBSERVABLE_H
