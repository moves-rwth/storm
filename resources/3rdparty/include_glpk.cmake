set(STORM_HAVE_GLPK OFF)
find_package(GLPK QUIET)

if(GLPK_FOUND)
    set(STORM_HAVE_GLPK ON)
    message (STATUS "Storm - Using system version of glpk.")
    message (STATUS "Storm - Linking with glpk ${GLPK_VERSION_STRING}")

    add_library(glpk SHARED IMPORTED)
    set_target_properties(
            glpk
            PROPERTIES
            IMPORTED_LOCATION ${GLPK_LIBRARIES}
            INTERFACE_INCLUDE_DIRECTORIES ${GLPK_INCLUDE_DIR}
    )
    list(APPEND STORM_DEP_IMP_TARGETS glpk)

else()
    message (WARNING "Storm - Not linking with GLPK.")
endif()


