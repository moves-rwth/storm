if(NOT STORM_DISABLE_SPOT)
    find_package(soplex)

    IF(TARGET libsoplex-pic)
        set(STORM_HAVE_SOPLEX ON)
        get_target_property(soplexLOC libsoplex-pic LOCATION)
        get_target_property(soplexINCLUDE libsoplex-pic INTERFACE_INCLUDE_DIRECTORIES)
        MESSAGE(STATUS "Storm - Linking with SoPlex version ${soplex_VERSION}: (libary: ${soplexLOC}; include: ${soplexINCLUDE})")
        list(APPEND STORM_DEP_TARGETS libsoplex-pic)
    ELSE()
        set(STORM_HAVE_SOPLEX OFF)
        MESSAGE(STATUS "Storm not linking with SoPlex!")
    ENDIF()
else()
    set(STORM_HAVE_SOPLEX OFF)
    MESSAGE(STATUS "Storm not linking with SoPlex!")
endif()



