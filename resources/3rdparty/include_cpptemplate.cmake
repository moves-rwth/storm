string(REPLACE " " ";" CMAKE_CXX_FLAGS_AS_LIST ${CMAKE_CXX_FLAGS})
ExternalProject_Add(
        cpptemplate
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/cpptemplate
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/cpptemplate
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ${CMAKE_CXX_COMPILER} -c ${CMAKE_CXX_FLAGS_AS_LIST} -fPIC -I${Boost_INCLUDE_DIRS} -I${STORM_3RDPARTY_SOURCE_DIR}/utf8_v2_3_4/source ${STORM_3RDPARTY_SOURCE_DIR}/cpptemplate/cpptempl.cpp -o ${STORM_3RDPARTY_BINARY_DIR}/cpptemplate/cpptemplate${STATIC_EXT}
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE 0
        LOG_BUILD ON
)

set(CPPTEMPLATE_INCLUDE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/cpptemplate)
set(CPPTEMPLATE_STATIC_LIBRARY ${STORM_3RDPARTY_BINARY_DIR}/cpptemplate/cpptemplate${STATIC_EXT})
list(APPEND STORM_LINK_LIBRARIES ${CPPTEMPLATE_STATIC_LIBRARY})
add_dependencies(resources cpptemplate)

message(STATUS "StoRM - Linking with cpptemplate")
include_directories(${CPPTEMPLATE_INCLUDE_DIR})
