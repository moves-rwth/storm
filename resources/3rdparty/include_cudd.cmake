
####
#### Find autotools for cudd update step
set(CUDD_AUTOTOOLS_LOCATIONS "")
foreach (TOOL_VAR AUTORECONF ACLOCAL AUTOMAKE AUTOCONF AUTOHEADER)
	string(TOLOWER ${TOOL_VAR} PROG_NAME)
	find_program(${TOOL_VAR} ${PROG_NAME})
	if (NOT ${TOOL_VAR})
		message(FATAL_ERROR "Cannot find ${PROG_NAME}, cannot compile cudd3.")
	endif()
    mark_as_advanced(${TOOL_VAR})
	string(APPEND CUDD_AUTOTOOLS_LOCATIONS "${TOOL_VAR}=${${TOOL_VAR}};")
endforeach()

set(CUDD_LIB_DIR ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0/lib)

# create CUDD compilation flags
if (NOT STORM_DEBUG_CUDD)
	set(STORM_CUDD_FLAGS "-O3")
else()
	message(WARNING "Building CUDD in DEBUG mode.")
	set(STORM_CUDD_FLAGS "-O0 -g")
endif()
set(STORM_CUDD_FLAGS "CFLAGS=${STORM_CUDD_FLAGS} -w -DPIC -DHAVE_IEEE_754 -fno-common -ffast-math -fno-finite-math-only")
if (NOT STORM_PORTABLE AND (NOT APPLE_SILICON OR (STORM_COMPILER_CLANG AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15.0)))
	set(STORM_CUDD_FLAGS "${STORM_CUDD_FLAGS} -march=native")
endif()

# Set sysroot to circumvent problems in macOS "Mojave" (or higher) where the header files are no longer in /usr/include
set(CUDD_INCLUDE_FLAGS "")
if (CMAKE_OSX_SYSROOT)
    set(CUDD_INCLUDE_FLAGS "CPPFLAGS=--sysroot=${CMAKE_OSX_SYSROOT}")
endif()

set(CUDD_CXX_COMPILER "${CMAKE_CXX_COMPILER}")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12.0.0.12000032)
		if (CMAKE_HOST_SYSTEM_VERSION VERSION_GREATER_EQUAL 20.1.0)
			message(WARNING "There are some known issues compiling CUDD on some setups. We implemented a workaround that mostly works, but if you still have problems compiling CUDD, especially if you do not use the default compiler of your system, please contact the Storm developers.")
			# The issue is known to occur using the Command Line Tools for XCode 12.2. Apparently, it is fixed in the beta for XCode 12.3. 
			set(CUDD_CXX_COMPILER "c++")
		endif()
	endif()
endif()


ExternalProject_Add(
        cudd3
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/cudd-3.0.0
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0
        PATCH_COMMAND ${CMAKE_COMMAND} -E env ${CUDD_AUTOTOOLS_LOCATIONS} ${AUTORECONF}
        CONFIGURE_COMMAND ${STORM_3RDPARTY_SOURCE_DIR}/cudd-3.0.0/configure --enable-shared --enable-obj --with-pic=yes --prefix=${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0 --libdir=${CUDD_LIB_DIR} CC=${CMAKE_C_COMPILER} CXX=${CUDD_CXX_COMPILER} ${CUDD_INCLUDE_FLAGS}
        BUILD_COMMAND make ${STORM_CUDD_FLAGS} ${CUDD_AUTOTOOLS_LOCATIONS}
        INSTALL_COMMAND make install -j${STORM_RESOURCES_BUILD_JOBCOUNT} ${CUDD_AUTOTOOLS_LOCATIONS}
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

add_imported_library(cudd SHARED ${CUDD_SHARED_LIBRARY} ${CUDD_INCLUDE_DIR})
add_imported_library(cudd STATIC ${CUDD_STATIC_LIBRARY} ${CUDD_INCLUDE_DIR})

add_dependencies(resources cudd3)

if(BUILD_SHARED_LIBS)
	list(APPEND STORM_DEP_TARGETS cudd_SHARED)
else()
    list(APPEND STORM_DEP_TARGETS cudd_STATIC)
endif()

message(STATUS "Storm - Linking with CUDD ${CUDD_VERSION_STRING}.")
