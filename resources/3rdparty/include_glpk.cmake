find_package(GLPK QUIET)
if(GLPK_FOUND)
    message (STATUS "StoRM - Using system version of GLPK")
else()
    message (STATUS "StoRM - Using shipped version of GLPK")
    ExternalProject_Add(glpk
        DOWNLOAD_COMMAND ""
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/glpk-4.57
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/glpk-4.57
        CONFIGURE_COMMAND ${STORM_3RDPARTY_SOURCE_DIR}/glpk-4.57/configure --prefix=${STORM_3RDPARTY_SOURCE_DIR}/glpk-4.57  --libdir=${STORM_3RDPARTY_SOURCE_DIR}/glpk-4.57/lib CC=${CMAKE_C_COMPILER}
        BUILD_COMMAND make "CFLAGS=-O2 -w"
        INSTALL_COMMAND make install
        BUILD_IN_SOURCE 0
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
    )
    set(GLPK_LIBRARIES  ${CMAKE_BINARY_DIR}/resources/3rdparty/glpk-4.57/lib/libglpk${DYNAMIC_EXT})
    set(GLPK_INCLUDE_DIR ${CMAKE_BINARY_DIR}/resources/3rdparty/glpk-4.57/include)
    set(GLPK_VERSION_STRING 4.57)
    add_dependencies(resources glpk)
endif()

# Since there is a shipped version, always use GLPK
set(STORM_HAVE_GLPK ON)
message (STATUS "StoRM - Linking with glpk ${GLPK_VERSION_STRING}")
include_directories(${GLPK_INCLUDE_DIR})
list(APPEND STORM_LINK_LIBRARIES ${GLPK_LIBRARIES})
