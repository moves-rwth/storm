if(NOT AUTORECONF)
    message(ERROR "Cannot find autoreconf, cannot compile cudd3")
endif()


ExternalProject_Add(
        cudd3
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/cudd-3.0.0
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0
        UPDATE_COMMAND ${AUTORECONF}
        CONFIGURE_COMMAND ${STORM_3RDPARTY_SOURCE_DIR}/cudd-3.0.0/configure  --enable-shared   --enable-obj --prefix=${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0 --libdir=${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0/lib CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
        BUILD_COMMAND make "CFLAGS=-O2 -w"
        INSTALL_COMMAND make install
        BUILD_IN_SOURCE 0
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
)

# Do not use system CUDD, StoRM has a modified version
set(CUDD_INCLUDE_DIR ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0/include)
set(CUDD_SHARED_LIBRARY ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0/lib/libcudd${DYNAMIC_EXT})
set(CUDD_STATIC_LIBRARY $${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0/cudd-3.0.0/lib/libcudd${STATIC_EXT})
set(CUDD_VERSION_STRING 3.0.0)
list(APPEND STORM_LINK_LIBRARIES ${CUDD_SHARED_LIBRARY})
add_dependencies(resources cudd3)

message(STATUS "StoRM - Linking with CUDD ${CUDD_VERSION_STRING}")
#message("StoRM - CUDD include dir: ${CUDD_INCLUDE_DIR}")
include_directories(${CUDD_INCLUDE_DIR})