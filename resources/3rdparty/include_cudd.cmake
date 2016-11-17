if (NOT AUTORECONF)
    message(FATAL_ERROR "Cannot find autoreconf, cannot compile cudd3.")
endif()

set(CUDD_LIB_DIR ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0/lib)

ExternalProject_Add(
        cudd3
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/cudd-3.0.0
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0
        PATCH_COMMAND ${AUTORECONF}
        CONFIGURE_COMMAND ${STORM_3RDPARTY_SOURCE_DIR}/cudd-3.0.0/configure  --enable-shared   --enable-obj --prefix=${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0 --libdir=${CUDD_LIB_DIR} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
        BUILD_COMMAND make "CFLAGS=-O2 -w"
        INSTALL_COMMAND make install
        BUILD_IN_SOURCE 0
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
        BUILD_BYPRODUCTS ${CUDD_LIB_DIR}/libcudd${DYNAMIC_EXT} ${CUDD_LIB_DIR}/libcudd${STATIC_EXT}
)

# Do not use system CUDD, StoRM has a modified version
set(CUDD_INCLUDE_DIR ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0/include)
set(CUDD_SHARED_LIBRARY ${CUDD_LIB_DIR}/libcudd${DYNAMIC_EXT})
set(CUDD_STATIC_LIBRARY ${CUDD_LIB_DIR}/libcudd${STATIC_EXT})
set(CUDD_VERSION_STRING 3.0.0)
list(APPEND STORM_LINK_LIBRARIES ${CUDD_SHARED_LIBRARY})
add_dependencies(resources cudd3)

message(STATUS "Storm - Linking with CUDD ${CUDD_VERSION_STRING}.")
include_directories(${CUDD_INCLUDE_DIR})
