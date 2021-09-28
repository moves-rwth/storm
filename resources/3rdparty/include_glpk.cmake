find_package(GLPK QUIET)
if(GLPK_FOUND)
    message (STATUS "Storm - Using system version of glpk.")
else()
    message (STATUS "Storm - Using shipped version of glpk.")
    set(GLPK_LIB_DIR ${STORM_3RDPARTY_BINARY_DIR}/glpk-4.65/lib)
	
	# Set sysroot to circumvent problems in macOS "Mojave" (or higher) where the header files are no longer in /usr/include
	set(GLPK_INCLUDE_FLAGS "")
	if (CMAKE_OSX_SYSROOT)
	    set(GLPK_INCLUDE_FLAGS "CPPFLAGS=--sysroot=${CMAKE_OSX_SYSROOT}")
	endif()
	
    ExternalProject_Add(glpk_ext
        DOWNLOAD_COMMAND ""
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/glpk-4.65
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/glpk-4.65
        CONFIGURE_COMMAND ${STORM_3RDPARTY_SOURCE_DIR}/glpk-4.65/configure --prefix=${STORM_3RDPARTY_BINARY_DIR}/glpk-4.65  --libdir=${GLPK_LIB_DIR} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} ${GLPK_INCLUDE_FLAGS}
        BUILD_COMMAND make "CFLAGS=-O3 -w"
        INSTALL_COMMAND make install -j${STORM_RESOURCES_BUILD_JOBCOUNT}
        BUILD_IN_SOURCE 0
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
        BUILD_BYPRODUCTS ${GLPK_LIB_DIR}/libglpk${DYNAMIC_EXT} ${GLPK_LIB_DIR}/libglpk${STATIC_EXT}
    )
    set(GLPK_LIBRARIES  ${GLPK_LIB_DIR}/libglpk${DYNAMIC_EXT})
    set(GLPK_INCLUDE_DIR ${STORM_3RDPARTY_BINARY_DIR}/glpk-4.65/include)
    set(GLPK_VERSION_STRING 4.65)
    add_dependencies(resources glpk_ext)
endif()

# Since there is a shipped version, always use GLPK
set(STORM_HAVE_GLPK ON)
message (STATUS "Storm - Linking with glpk ${GLPK_VERSION_STRING}")

add_imported_library(glpk SHARED ${GLPK_LIBRARIES} ${GLPK_INCLUDE_DIR})
list(APPEND STORM_DEP_TARGETS glpk_SHARED)