if (STORM_USE_SOPLEX)
    find_package(soplex)

    IF(SOPLEX_FOUND)
        get_target_property(soplexLOC libsoplex-pic LOCATION)
        get_target_property(soplexINCLUDE libsoplex-pic INTERFACE_INCLUDE_DIRECTORIES)
        MESSAGE(STATUS "Storm - Linking with SoPlex: (libary: ${soplexLOC}; include: ${soplexINCLUDE})")
    ELSE(SOPLEX_FOUND)
        MESSAGE(WARNING "Storm not linking with SoPlex!")
    ENDIF(SOPLEX_FOUND)
    set(STORM_HAVE_SOPLEX ${SOPLEX_FOUND})
    list(APPEND STORM_DEP_TARGETS libsoplex-pic)
ENDIF(STORM_USE_SOPLEX)

