
add_subdirectory(ecs)
add_subdirectory(logs)
add_subdirectory(game)


###########################################################################
#
#    Récupération des éléments tiers
#
###########################################################################

#### argparse

# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)


###########################################################################
#
#    Binaire du jeu
#
###########################################################################

add_executable(${PROJECT_NAME} main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE ${STRICT_COMPILE_OPTIONS})
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        argparse
        SharedPCH
        GameLib::GameLib
        ecs::LogsLib
        ecs::EcsLib
)

###########################################################################
#
#    Définition des étapes de copie des fichiers de configuration, des ressources et des données
#
###########################################################################

# Copy config directory to output directory
add_custom_target(copy-config-files ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/config
        ${CMAKE_CURRENT_BINARY_DIR}/config
)
# Add copy-file target to this project dependencies
add_dependencies(${PROJECT_NAME} copy-config-files)

# Copy assets directory to output directory
add_custom_target(copy-assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
)
# Add copy-file target to this project dependencies
add_dependencies(${PROJECT_NAME} copy-assets)

