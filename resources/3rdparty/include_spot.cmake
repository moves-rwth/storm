set(MANUAL_SPOT_DIR ${SPOT_ROOT})
ExternalProject_Add(spot
        PREFIX ${SPOT_ROOT}/spot
        SOURCE_DIR ${SPOT_ROOT}/spot_src
        CONFIGURE_COMMAND ${SPOT_ROOT}/spot_src/configure --prefix=${MANUAL_SPOT_DIR}/spot --disable-python
        BUILD_COMMAND make -j${STORM_RESOURCES_BUILD_JOBCOUNT}
        INSTALL_COMMAND make install
        BUILD_BYPRODUCTS ${SPOT_ROOT}/spot/lib/libspot${DYNAMIC_EXT}
        )

set(SPOT_INCLUDE_DIR "${MANUAL_SPOT_DIR}/spot/include/")
set(SPOT_DIR "${MANUAL_SPOT_DIR}/spot/")
set(SPOT_LIBRARIES ${MANUAL_SPOT_DIR}/spot/lib/libspot${DYNAMIC_EXT})
set(STORM_HAVE_SPOT ON)
add_dependencies(resources spot)

if (STORM_HAVE_SPOT)
    include_directories("${SPOT_INCLUDE_DIR}")
    list(APPEND STORM_LINK_LIBRARIES ${SPOT_LIBRARIES})
endif()
