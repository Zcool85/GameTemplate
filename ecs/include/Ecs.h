//
// Created by Zéro Cool on 18/06/2025.
//

#ifndef ECS_H
#define ECS_H


// Entity
// /   Simple struct "pointing" to the correct component data.
// /   Contains a bitset to keep track of available components
// /   and tags, and a flag to check if the entity is alive.

// Manager
// /   "Context" class. Will contain entities, components,
// /   signatures, metadata.
// /   It will deal with entity/component creation/removal,
// /   with entity iteration and much more.

// Handle
// /   Layer of indirection between the entities and the user.
// /   Handles will be used to keep track and access an entity.
// /   It will be possible to check if the entity is still valid
// /   through an existing handle.


#include "EcsTypes.h"
#include "Settings.h"
#include "Manager.h"
#include "Log.h"


// Inspirations = https://github.com/CppCon/CppCon2015/blob/master/Tutorials/Implementation%20of%20a%20component-based%20entity%20system%20in%20modern%20C%2B%2B/Source%20Code/p3.cpp


// TODO : Try to implement this : https://github.com/CppCon/CppCon2015/blob/master/Tutorials/Implementation%20of%20a%20component-based%20entity%20system%20in%20modern%20C%2B%2B/Source%20Code/p7.cpp


#endif //ECS_H
