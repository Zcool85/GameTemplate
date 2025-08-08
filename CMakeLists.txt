cmake_minimum_required(VERSION 3.31)

###########################################################################
#
#    Définition du projet
#
###########################################################################

project(GameTemplate
        LANGUAGES CXX
        VERSION 1.0
        DESCRIPTION "Game project template"
)

###########################################################################
#
#    Définition des options de compilation
#
###########################################################################

# C++23 (CLion limit to C++2b)
set(CMAKE_CXX_STANDARD 20)

# Détection du compilateur et de l'architecture
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    #set(NATIVE_OPTIMIZATION_FLAGS "-march=native")

    # Optimisations supplémentaires pour les compilateurs GCC/Clang
    set(PERFORMANCE_FLAGS "-O3 -funroll-loops -ffast-math")

    # On force l'include de cstdint cat c'est l'option par défaut avec CLion
    # et l'on veut le même comportement dans le pipeline CI
    add_compile_options(-include cstdint)

    # Ici on essaye d'avoir les mêmes options de compilation que dans le pipeline CI
    set(STRICT_COMPILE_OPTIONS
            -Werror -Wall -Wextra -Wpedantic -Wshadow
            -Wconversion -Wsign-conversion -Wattributes
            -Wimplicit-int-float-conversion
    )
elseif (MSVC)
    # Équivalent MSVC
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64")
        set(NATIVE_OPTIMIZATION_FLAGS "/arch:AVX2")
    endif ()
    set(PERFORMANCE_FLAGS "/O2 /Ob2")

    add_compile_options("/Wall")
endif ()


# Generate file "${CMAKE_BINARY_DIR}/compile_commands.json"
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

###########################################################################
#
#    Inclusion des modules CMake
#
###########################################################################

add_subdirectory(src)


###########################################################################
#
#    Information de compilation
#
###########################################################################

message(STATUS "Compilateur: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Cible: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Type de build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Standard C++: ${CMAKE_CXX_STANDARD}")
