ExternalProject_Add(
        cpptemplate
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/cpptemplate
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/cpptemplate
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ${CMAKE_CXX_COMPILER} -std=c++14 -stdlib=libc++ -O3 -I${Boost_INCLUDE_DIRS} -I${STORM_3RDPARTY_SOURCE_DIR}/utf8_v2_3_4/source -shared ${STORM_3RDPARTY_SOURCE_DIR}/cpptemplate/cpptempl.cpp -o ${STORM_3RDPARTY_BINARY_DIR}/cpptemplate/cpptemplate${DYNAMIC_EXT}
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE 0
        LOG_BUILD ON
)

set(CPPTEMPLATE_INCLUDE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/cpptemplate)
set(CPPTEMPLATE_SHARED_LIBRARY ${STORM_3RDPARTY_BINARY_DIR}/cpptemplate/cpptemplate${DYNAMIC_EXT})
list(APPEND STORM_LINK_LIBRARIES ${CPPTEMPLATE_SHARED_LIBRARY})
add_dependencies(resources cpptemplate)

message(STATUS "StoRM - Linking with cpptemplate")
include_directories(${CPPTEMPLATE_INCLUDE_DIR})
