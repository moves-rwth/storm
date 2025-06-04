set(STORM_HAVE_SPOT OFF)
set(STORM_SHIPPED_SPOT OFF)

if(NOT STORM_DISABLE_SPOT)
    # try to find Spot on the system
    if (NOT STORM_SPOT_FORCE_SHIPPED)
        if (NOT "${SPOT_ROOT}" STREQUAL "")
            message(STATUS "Storm - searching for Spot in ${SPOT_ROOT}")
            find_package(Spot QUIET PATHS ${SPOT_ROOT} NO_DEFAULT_PATH)
        endif()
        if (NOT Spot_FOUND)
            find_package(Spot QUIET)
        endif()

        if (Spot_FOUND)
            get_filename_component(Spot_LIB_DIR ${Spot_LIBRARIES} DIRECTORY)
            find_library(BUDDY_LIBRARY NAMES libbddx bddx PATHS ${Spot_LIB_DIR} NO_DEFAULT_PATH)
            if(NOT BUDDY_LIBRARY)
                message(FATAL_ERROR "Storm - Did not find BUDDY library that should ship with Spot. To work around this, you may disable the system version Spot with '-DSTORM_USE_Spot_SYSTEM=OFF'.")
            endif()

            add_library(Storm::Spot-bddx UNKNOWN IMPORTED)
            set_target_properties(Storm::Spot-bddx PROPERTIES
                    IMPORTED_LOCATION ${BUDDY_LIBRARY})

            add_library(Storm::Spot UNKNOWN IMPORTED)
            set_target_properties(Storm::Spot PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${Spot_INCLUDE_DIR}"
                    IMPORTED_LOCATION "${Spot_LIBRARIES}"
                    INTERFACE_LINK_LIBRARIES Storm::Spot-bddx
            )

            message(STATUS "Storm - Using system version of Spot ${Spot_VERSION} (include: ${Spot_INCLUDE_DIR}, library: ${Spot_LIBRARIES}).")
            list(APPEND STORM_DEP_IMP_TARGETS Storm::Spot)

            set(SPOTINFIX "") # not relevant!
            set(STORM_HAVE_SPOT ON)
        endif()
    endif()

    if(NOT STORM_HAVE_SPOT)
        # Spot does not set library IDs with an rpath but with an absolute path.
        if (APPLE)
            # We need to work on these .0 versions as otherwise we cannot fix the RPATHs.
            set(Spot_RPATH_FIX_COMMAND "install_name_tool;-id;@rpath/libspot.dylib;${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libspot${DYNAMIC_EXT}")
            set(BDDX_RPATH_FIX_COMMAND1 "install_name_tool;-change;${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libbddx.0.dylib;@rpath/libbddx.dylib;${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libspot${DYNAMIC_EXT}")
            set(BDDX_RPATH_FIX_COMMAND2 "install_name_tool;-id;@rpath/libbddx.dylib;${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libbddx${DYNAMIC_EXT}")
            set(SPOTINFIX ".0")
        else()
            set(Spot_RPATH_FIX_COMMAND "true") #no op
            set(BDDX_RPATH_FIX_COMMAND1 "true") #no op
            set(BDDX_RPATH_FIX_COMMAND2 "true") #no op
            set(SPOTINFIX "")
        endif()

        # download and install shipped Spot as shared libraries.
        # set Spot version
        set(SPOT_SHIPPED_VERSION 2.13.1)
        # Note that configuring static libraries requires various dependencies which was rather cumbersome.
        # As of '25/3, SJ did not get this to work.
        ExternalProject_Add(Spot
            URL https://www.lre.epita.fr/dload/spot/spot-${SPOT_SHIPPED_VERSION}.tar.gz https://www.lrde.epita.fr/dload/spot/spot-${SPOT_SHIPPED_VERSION}.tar.gz
            DOWNLOAD_NO_PROGRESS TRUE
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE # Prevents reconfiguration, see https://gitlab.kitware.com/cmake/cmake/-/issues/24003
            DOWNLOAD_DIR ${STORM_3RDPARTY_BINARY_DIR}/spot_src
            SOURCE_DIR ${STORM_3RDPARTY_BINARY_DIR}/spot_src
            PREFIX ${STORM_3RDPARTY_BINARY_DIR}/spot
            CONFIGURE_COMMAND ${STORM_3RDPARTY_BINARY_DIR}/spot_src/configure --prefix=${STORM_3RDPARTY_BINARY_DIR}/spot --disable-python #--enable-static --disable-shared
            BUILD_COMMAND make -j${STORM_RESOURCES_BUILD_JOBCOUNT}
            INSTALL_COMMAND make install -j${STORM_RESOURCES_BUILD_JOBCOUNT}
                COMMAND ${Spot_RPATH_FIX_COMMAND}
                COMMAND ${BDDX_RPATH_FIX_COMMAND1}
                COMMAND ${BDDX_RPATH_FIX_COMMAND2}
            LOG_DOWNLOAD ON
            LOG_CONFIGURE ON
            LOG_BUILD ON
            LOG_INSTALL ON
            LOG_OUTPUT_ON_FAILURE ON
            BUILD_BYPRODUCTS ${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libspot${SPOTINFIX}${DYNAMIC_EXT};${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libbddx${SPOTINFIX}${DYNAMIC_EXT}
        )
        add_dependencies(storm_resources Spot)

        set(Spot_INCLUDE_DIR "${STORM_3RDPARTY_BINARY_DIR}/spot/include/")
        set(Spot_LIBRARIES "${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libspot${SPOTINFIX}${DYNAMIC_EXT};${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libbddx${SPOTINFIX}${DYNAMIC_EXT};${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libspot${DYNAMIC_EXT};${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libbddx${DYNAMIC_EXT}")
        set(Spot_INSTALL_DIR ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/spot/)
        set(STORM_HAVE_SPOT ON)
        set(STORM_SHIPPED_SPOT ON)

        file(MAKE_DIRECTORY ${Spot_INCLUDE_DIR}) # Workaround https://gitlab.kitware.com/cmake/cmake/-/issues/15052
        add_library(Storm::Spot-bddx SHARED IMPORTED)
        set_target_properties(Storm::Spot-bddx PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${STORM_3RDPARTY_BINARY_DIR}/spot/include/"
                IMPORTED_LOCATION ${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libbddx${SPOTINFIX}${DYNAMIC_EXT})

        add_library(Storm::Spot SHARED IMPORTED)
        set_target_properties(Storm::Spot PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${STORM_3RDPARTY_BINARY_DIR}/spot/include/"
                IMPORTED_LOCATION ${STORM_3RDPARTY_BINARY_DIR}/spot/lib/libspot${SPOTINFIX}${DYNAMIC_EXT}
                INTERFACE_LINK_LIBRARIES Storm::Spot-bddx
        )

        install(FILES ${Spot_LIBRARIES} DESTINATION ${STORM_RESOURCE_LIBRARY_INSTALL_DIR})
        install(DIRECTORY ${Spot_INCLUDE_DIR}/ DESTINATION ${Spot_INSTALL_DIR}
                FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN ".git" EXCLUDE)

        list(APPEND STORM_DEP_IMP_TARGETS Storm::Spot)
        add_dependencies(storm_resources Storm::Spot )

        message(STATUS "Storm - Using shipped version of Spot ${SPOT_SHIPPED_VERSION} (include: ${Spot_INCLUDE_DIR}, library ${Spot_LIBRARIES}).")

    endif()
endif()
if(NOT STORM_HAVE_SPOT)
    message("Storm - Not linking with Spot. Model checking of LTL formulas (beyond PCTL) will not be supported.")
endif()
