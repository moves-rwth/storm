find_package(GLPK QUIET)
if(GLPK_FOUND)
    message (STATUS "Storm - Using system version of glpk.")
else()
    message (STATUS "Storm - Using shipped version of glpk.")
    set(GLPK_LIB_DIR ${STORM_3RDPARTY_BINARY_DIR}/glpk-4.57/lib)
    ExternalProject_Add(glpk_ext
        DOWNLOAD_COMMAND ""
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/glpk-4.57
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/glpk-4.57
        CONFIGURE_COMMAND ${STORM_3RDPARTY_SOURCE_DIR}/glpk-4.57/configure --prefix=${STORM_3RDPARTY_BINARY_DIR}/glpk-4.57  --libdir=${GLPK_LIB_DIR} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
        BUILD_COMMAND make "CFLAGS=-O2 -w"
        INSTALL_COMMAND make install
        BUILD_IN_SOURCE 0
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
        BUILD_BYPRODUCTS ${GLPK_LIB_DIR}/libglpk${DYNAMIC_EXT} ${GLPK_LIB_DIR}/libglpk${STATIC_EXT}
    )
    set(GLPK_LIBRARIES  ${GLPK_LIB_DIR}/libglpk${DYNAMIC_EXT})
    set(GLPK_INCLUDE_DIR ${STORM_3RDPARTY_BINARY_DIR}/glpk-4.57/include)
    set(GLPK_VERSION_STRING 4.57)
    add_dependencies(resources glpk_ext)
endif()

# Since there is a shipped version, always use GLPK
set(STORM_HAVE_GLPK ON)
message (STATUS "Storm - Linking with glpk ${GLPK_VERSION_STRING}")

add_imported_library(glpk SHARED ${GLPK_LIBRARIES} ${GLPK_INCLUDE_DIR})
list(APPEND STORM_DEP_TARGETS glpk_SHARED)