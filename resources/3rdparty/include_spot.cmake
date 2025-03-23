set(STORM_HAVE_SPOT OFF)
set(STORM_SHIPPED_SPOT OFF)

if(TRUE) # For easier disabling.
    # try to find Spot on the system
    if (NOT "${SPOT_ROOT}" STREQUAL "")
        message(STATUS "Storm - searching for Spot in ${SPOT_ROOT}")
        find_package(SPOT QUIET PATHS ${SPOT_ROOT} NO_DEFAULT_PATH)
    endif()
    if (NOT SPOT_FOUND)
        find_package(SPOT QUIET)
    endif()

    if (SPOT_FOUND)
        get_filename_component(SPOT_LIB_DIR ${SPOT_LIBRARIES} DIRECTORY)
        find_library(BUDDY_LIBRARY NAMES libbddx bddx PATHS ${SPOT_LIB_DIR} NO_DEFAULT_PATH)
        if(NOT BUDDY_LIBRARY)
            message(FATAL_ERROR "Storm - Did not find BUDDY library that should ship with spot. To work around this, you may disable the system version Spot with '-DSTORM_USE_SPOT_SYSTEM=OFF'.")
        endif()

        add_library(Storm::spot-bddx UNKNOWN IMPORTED)
        set_target_properties(Storm::spot-bddx PROPERTIES
                IMPORTED_LOCATION ${BUDDY_LIBRARY})

        add_library(Storm::spot UNKNOWN IMPORTED)
        set_target_properties(Storm::spot PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${SPOT_INCLUDE_DIR}"
                IMPORTED_LOCATION "${SPOT_LIBRARIES}"
                INTERFACE_LINK_LIBRARIES Storm::spot-bddx
        )

        message(STATUS "Storm - Using system version of Spot ${SPOT_VERSION} (include: ${SPOT_INCLUDE_DIR}, library: ${SPOT_LIBRARIES}).")
        list(APPEND STORM_DEP_IMP_TARGETS Storm::spot)

        set(STORM_HAVE_SPOT ON)
    endif()
endif()

if(NOT SPOT_FOUND AND STORM_USE_SPOT_SHIPPED)
    # Spot does not set library IDs with an rpath but with an absolute path.
    if (APPLE)
        set(SPOT_RPATH_FIX_COMMAND "install_name_tool;-id;@rpath/libspot.dylib;${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libspot${DYNAMIC_EXT}")
    else()
        set(SPOT_RPATH_FIX_COMMAND "")
    endif()

    # download and install shipped Spot as shared libraries.
    # Note that configuring static libraries requires various dependencies which was rather cumbersome.
    # As of '25/3, SJ did not get this to work.
    ExternalProject_Add(spot
        URL https://www.lrde.epita.fr/dload/spot/spot-2.12.tar.gz # When updating, also change version output below
        DOWNLOAD_NO_PROGRESS TRUE
        DOWNLOAD_DIR ${STORM_3RDPARTY_BINARY_DIR}/spot_src
        SOURCE_DIR ${STORM_3RDPARTY_BINARY_DIR}/spot_src
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/spot
        CONFIGURE_COMMAND ${STORM_3RDPARTY_BINARY_DIR}/spot_src/configure --prefix=${STORM_3RDPARTY_BINARY_DIR}/spot --disable-python #--enable-static --disable-shared
        BUILD_COMMAND make -j${STORM_RESOURCES_BUILD_JOBCOUNT}
        INSTALL_COMMAND make install -j${STORM_RESOURCES_BUILD_JOBCOUNT}
            COMMAND ${SPOT_RPATH_FIX_COMMAND}
           # COMMAND ${BDDX_RPATH_FIX_COMMAND}
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
        LOG_OUTPUT_ON_FAILURE ON
        BUILD_BYPRODUCTS ${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libspot.0${DYNAMIC_EXT};${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libbddx.0${DYNAMIC_EXT}
    )
    add_dependencies(storm_resources spot)

    set(SPOT_INCLUDE_DIR "${STORM_3RDPARTY_BINARY_DIR}/spot/include/")
    set(SPOT_LIBRARIES "${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libspot.0${DYNAMIC_EXT};${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libbddx.0${DYNAMIC_EXT}")
    set(SPOT_INSTALL_DIR ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/spot/)
    set(STORM_HAVE_SPOT ON)
    set(STORM_SHIPPED_SPOT ON)

    file(MAKE_DIRECTORY ${SPOT_INCLUDE_DIR}) # Workaround https://gitlab.kitware.com/cmake/cmake/-/issues/15052

    add_library(Storm::spot-bddx SHARED IMPORTED)
    set_target_properties(Storm::spot-bddx PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${STORM_3RDPARTY_BINARY_DIR}/spot/include/"
            IMPORTED_LOCATION ${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libbddx.0${DYNAMIC_EXT})

    add_library(Storm::spot SHARED IMPORTED)
    set_target_properties(Storm::spot PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${STORM_3RDPARTY_BINARY_DIR}/spot/include/"
            IMPORTED_LOCATION ${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libspot.0${DYNAMIC_EXT}
            INTERFACE_LINK_LIBRARIES Storm::spot-bddx
    )

    install(FILES ${SPOT_LIBRARIES} DESTINATION ${STORM_RESOURCE_LIBRARY_INSTALL_DIR})
    install(DIRECTORY ${SPOT_INCLUDE_DIR}/ DESTINATION ${SPOT_INSTALL_DIR}
            FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN ".git" EXCLUDE)

    list(APPEND STORM_DEP_IMP_TARGETS Storm::spot)
    add_dependencies(storm_resources Storm::spot  )

    message(STATUS "Storm - Using shipped version of Spot 2.12 (include: ${SPOT_INCLUDE_DIR}, library ${SPOT_LIBRARIES}).")

endif()

if(NOT STORM_HAVE_SPOT)
    message("Storm - Not using Spot.")
endif()