//
// Created by Zéro Cool on 12/07/2025.
//

#ifndef CINPUT_H
#define CINPUT_H

struct CInput {
    int up{};
    int down{};
    int left{};
    int right{};
    int shoot{};
    // TODO : PEut-on se passer de cette variable ???
    sf::Vector2i shoot_position{};
};

#endif //CINPUT_H
