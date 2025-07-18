FIND_PATH(CONFIG++_INCLUDE_DIR libconfig.h++ /usr/include /usr/local/include)

FIND_LIBRARY(CONFIG++_LIBRARY NAMES config++ PATH /usr/lib /usr/local/lib)

IF (CONFIG++_INCLUDE_DIR AND CONFIG++_LIBRARY)
    SET(CONFIG++_FOUND TRUE)
ENDIF (CONFIG++_INCLUDE_DIR AND CONFIG++_LIBRARY)

IF (CONFIG++_FOUND)
    IF (NOT CONFIG++_FIND_QUIETLY)
        MESSAGE(STATUS "Found Config++: ${CONFIG++_LIBRARY}")

        add_library(config++ SHARED IMPORTED)
        set_target_properties(config++ PROPERTIES
                IMPORTED_IMPLIB "${CONFIG++_LIBRARY}"
        )

    ENDIF (NOT CONFIG++_FIND_QUIETLY)
ELSE (CONFIG++_FOUND)
    IF (Config++_FIND_REQUIRED)
        IF (NOT CONFIG++_INCLUDE_DIR)
            MESSAGE(FATAL_ERROR "Could not find LibConfig++ header file!")
        ENDIF (NOT CONFIG++_INCLUDE_DIR)

        IF (NOT CONFIG++_LIBRARY)
            MESSAGE(FATAL_ERROR "Could not find LibConfig++ library file!")
        ENDIF (NOT CONFIG++_LIBRARY)
    ENDIF (Config++_FIND_REQUIRED)
ENDIF (CONFIG++_FOUND)