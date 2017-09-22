cmake_minimum_required (VERSION 3.2)
cmake_policy(VERSION 3.2)

# Set project name
project (storm CXX C)

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/storm")

# Add the resources/cmake folder to Module Search Path for FindTBB.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/resources/cmake/find_modules" "${PROJECT_SOURCE_DIR}/resources/cmake/macros")

include(ExternalProject)
include(RegisterSourceGroup)
include(imported)


#############################################################
##
##	CMake options of Storm
##
#############################################################
option(STORM_DEVELOPER "Sets whether the development mode is used." OFF)
option(STORM_ALLWARNINGS "Compile with even more warnings" OFF)
option(STORM_USE_LTO "Sets whether link-time optimizations are enabled." ON)
MARK_AS_ADVANCED(STORM_USE_LTO)
option(STORM_PORTABLE "Sets whether a build needs to be portable." OFF)
MARK_AS_ADVANCED(STORM_PORTABLE)
# Force POPCNT is helpful for portable code targetting platforms with SSE4.2 operation support.
option(STORM_FORCE_POPCNT "Sets whether the popcnt instruction is forced to be used (advanced)." OFF)
MARK_AS_ADVANCED(STORM_FORCE_POPCNT)
option(USE_BOOST_STATIC_LIBRARIES "Sets whether the Boost libraries should be linked statically." OFF)
option(STORM_USE_INTELTBB "Sets whether the Intel TBB libraries should be used." OFF)
option(STORM_USE_GUROBI "Sets whether Gurobi should be used." OFF)
set(STORM_CARL_DIR_HINT "" CACHE STRING "A hint where the preferred CArL version can be found. If CArL cannot be found there, it is searched in the OS's default paths.")
option(STORM_FORCE_SHIPPED_CARL "Sets whether the shipped version of carl is to be used no matter whether carl is found or not." OFF)
MARK_AS_ADVANCED(STORM_FORCE_SHIPPED_CARL)
option(USE_SMTRAT "Sets whether SMT-RAT should be included." OFF)
option(USE_HYPRO "Sets whether HyPro should be included." OFF)
option(XML_SUPPORT "Sets whether xml based format parsing should be included." ON)
option(FORCE_COLOR "Force color output" OFF)
mark_as_advanced(FORCE_COLOR)
option(STORM_COMPILE_WITH_CCACHE "Compile using CCache [if found]" ON)
mark_as_advanced(STORM_COMPILE_WITH_CCACHE)
option(STORM_LOG_DISABLE_DEBUG "Disable log and trace message support" OFF)
option(STORM_USE_CLN_EA "Sets whether CLN instead of GMP numbers should be used for exact arithmetic." OFF)
export_option(STORM_USE_CLN_EA)
option(STORM_USE_CLN_RF "Sets whether CLN instead of GMP numbers should be used for rational functions." ON)
export_option(STORM_USE_CLN_RF)
option(BUILD_SHARED_LIBS "Build the Storm library dynamically" OFF)
option(STORM_DEBUG_CUDD "Build CUDD in debug mode." OFF)
MARK_AS_ADVANCED(STORM_DEBUG_CUDD)
set(BOOST_ROOT "" CACHE STRING "A hint to the root directory of Boost (optional).")
set(GUROBI_ROOT "" CACHE STRING "A hint to the root directory of Gurobi (optional).")
set(Z3_ROOT "" CACHE STRING "A hint to the root directory of Z3 (optional).")
set(CUDA_ROOT "" CACHE STRING "The hint to the root directory of CUDA (optional).")
set(MSAT_ROOT "" CACHE STRING "The hint to the root directory of MathSAT (optional).")
set(ADDITIONAL_INCLUDE_DIRS "" CACHE STRING "Additional directories added to the include directories.")
set(ADDITIONAL_LINK_DIRS "" CACHE STRING "Additional directories added to the link directories.")
set(USE_XERCESC ${XML_SUPPORT})
mark_as_advanced(USE_XERCESC)

# Set some CMAKE Variables as advanced
mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
mark_as_advanced(CMAKE_OSX_SYSROOT)
mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)

# Offer the user the choice of overriding the installation directories
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for header files" )
set(LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
#set(SYSCONFIG_INSTALL_DIR etc/carl/  CACHE PATH "Installation for system configuration files)
set(BIN_INSTALL_DIR lib/ CACHE PATH "Installation directory for executables")


# Install dir for cmake files (info for other libraries that include Storm)
set(DEF_INSTALL_CMAKE_DIR "lib/CMake/storm")
set(CMAKE_INSTALL_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH  "Installation directory for CMake files")

message("CMAKE_INSTALL_DIR: ${CMAKE_INSTALL_DIR}")

# If the STORM_DEVELOPER option was turned on, by default we target a debug version, otherwise a release version.
if (STORM_DEVELOPER)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "DEBUG")
    endif()
    add_definitions(-DSTORM_DEV)
else()
    set(STORM_LOG_DISABLE_DEBUG ON)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "RELEASE")
    endif()
endif()

message(STATUS "Storm - Building ${CMAKE_BUILD_TYPE} version.")

if(STORM_COMPILE_WITH_CCACHE)
	find_program(CCACHE_FOUND ccache)
	mark_as_advanced(CCACHE_FOUND)
	if(CCACHE_FOUND)
		message(STATUS "Storm - Using ccache")
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
	else()
		message(STATUS "Storm - Could not find ccache.")
	endif()
endif()

# Directory for test resources.
set(STORM_TEST_RESOURCES_DIR "${PROJECT_SOURCE_DIR}/resources/examples/testfiles")

# Auto-detect operating system.
set(MACOSX 0)
set(LINUX 0)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# Mac OS
	set(OPERATING_SYSTEM "Mac OS")
        set(MACOSX 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux
	set(OPERATING_SYSTEM "Linux")
        set(LINUX 1)
elseif(WIN32)
    # Assuming Windows.
    set(OPERATING_SYSTEM "Windows")
else()
    message(WARNING "We are unsure about your operating system.")
    set(OPERATING_SYSTEM "Linux")
    set(LINUX 1)
ENDIF()
message(STATUS "Storm - Detected operating system ${OPERATING_SYSTEM}.")

set(DYNAMIC_EXT ".so")
set(STATIC_EXT ".a")
set(LIB_PREFIX "lib")
if(MACOSX)
	set(DYNAMIC_EXT ".dylib")
	set(STATIC_EXT ".a")
	set(LIB_PREFIX "lib")
elseif (WIN32)
	set(DYNAMIC_EXT ".dll")
	set(STATIC_EXT ".lib")
	set(LIB_PREFIX "")
endif()
message(STATUS "Assuming extension for shared libraries: ${DYNAMIC_EXT}")
message(STATUS "Assuming extension for static libraries: ${STATIC_EXT}")

if(BUILD_SHARED_LIBS)
    set(LIB_EXT ${DYNAMIC_EXT})
    message(STATUS "Build dynamic libraries.")
else()
    set(LIB_EXT ${STATIC_EXT})
    message(STATUS "Build static libraries.")
endif()

#############################################################
##
##	Compiler detection and initial configuration
##
#############################################################
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# using Clang
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
		message(FATAL_ERROR "Clang version must be at least 3.2.")
	endif()

	set(STORM_COMPILER_CLANG ON)
	set(CLANG ON)
	set(STORM_COMPILER_ID "clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	# using AppleClang
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.3)
		message(FATAL_ERROR "AppleClang version must be at least 7.3.")
	endif()

	set(STORM_COMPILER_APPLECLANG ON)
	set(CLANG ON)
	set(STORM_COMPILER_ID "AppleClang")
	set(CMAKE_MACOSX_RPATH ON)	
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# using GCC
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
		message(FATAL_ERROR "gcc version must be at least 5.0.")
	endif()

	set(STORM_COMPILER_GCC ON)
	set(STORM_COMPILER_ID "gcc")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	message(FATAL_ERROR "Intel compiler is currently not supported.")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	message(FATAL_ERROR "Visual Studio compiler is currently not supported.")
endif()
set(STORM_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION})

if(CCACHE_FOUND)
	set(STORM_COMPILER_ID "${STORM_COMPILER_ID} (ccache)")
endif()

#############################################################
##
##	Compiler independent settings
##
#############################################################
if (STORM_FORCE_POPCNT)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
endif()

# TODO: remove forcing the old version of optional as soon as the related Spirit bug is fixed:
# https://svn.boost.org/trac/boost/ticket/12349
# Fix thanks to: https://github.com/freeorion/freeorion/issues/777
add_definitions(-DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_OPTIONAL_CONFIG_USE_OLD_IMPLEMENTATION_OF_OPTIONAL)

#############################################################
##
##	Compiler specific settings
##
#############################################################
if (STORM_COMPILER_CLANG OR STORM_COMPILER_APPLECLANG)
    if(FORCE_COLOR)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    endif()
    
	if (LINUX)
		set(CLANG_STDLIB libstdc++)
    else()
		set(CLANG_STDLIB libc++)
    endif()
    
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=${CLANG_STDLIB} -ftemplate-depth=1024")
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -fno-finite-math-only")
    
    if(LINUX)
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
    elseif(MACOSX)
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-export_dynamic")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-export_dynamic")
    endif()
elseif (STORM_COMPILER_GCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fprefetch-loop-arrays -ffast-math -fno-finite-math-only")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
endif ()

if (STORM_USE_LTO)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
    
    # Fix for problems that occurred when using LTO on gcc. This should be removed when it
    # is not needed anymore as it makes the the already long link-step potentially longer.
    if (STORM_COMPILER_GCC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto-partition=none")
    endif()
    
    message(STATUS "Storm - Enabling link-time optimizations.")
else()
    message(STATUS "Storm - Disabling link-time optimizations.")
endif()

# In release mode, we turn on even more optimizations if we do not have to provide a portable binary.
if (NOT STORM_PORTABLE)
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
endif ()

if (STORM_DEVELOPER)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
	if (STORM_ALLWARNINGS)
		if (CLANG)
		    # Enable strictly every warning and then disable selected ones.
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-old-style-cast")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reserved-id-macro")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-newline-eof")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-documentation")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch-enum")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-covered-switch-default")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-padded")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-float-equal")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedef")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-variable-declarations")
		endif ()
	else ()
	    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
	endif ()
else()
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer")
endif()

#############################################################
##
##	RPATH settings
##
#############################################################

# don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already (but only when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#############################################################
##
##	Generator specific settings
##
#############################################################
if ("${CMAKE_GENERATOR}" STREQUAL "Xcode")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()

# Display information about build configuration.
message(STATUS "Storm - Using compiler configuration ${STORM_COMPILER_ID} ${STORM_COMPILER_VERSION}.")
if (STORM_DEVELOPER)
	message(STATUS "Storm - CXX Flags: ${CMAKE_CXX_FLAGS}")
	message(STATUS "Storm - CXX Debug Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
	message(STATUS "Storm - CXX Release Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
	message(STATUS "Storm - Build type: ${CMAKE_BUILD_TYPE}")
endif()

#############################################################
#############################################################
##
##	Inclusion of required libraries
##
#############################################################
#############################################################

#############################################################
##
##	Include the targets for non-system resources
##
#############################################################

# In 3rdparty, targets are being defined that can be used
# in the the system does not have a library
include(resources/3rdparty/CMakeLists.txt)

#############################################################
##
##	Doxygen
##
#############################################################

find_package(Doxygen)
# Add a target to generate API documentation with Doxygen
if(DOXYGEN_FOUND)
    set(CMAKE_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc")
    string(REGEX REPLACE ";" " " CMAKE_DOXYGEN_INPUT_LIST "${PROJECT_SOURCE_DIR}/src")

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources/doxygen/Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

    add_custom_target(doc ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

#############################################################
##
##	CMake-generated Config File for Storm
##
#############################################################

# try to obtain the current version from git.
include(GetGitRevisionDescription)
get_git_head_revision(STORM_VERSION_REFSPEC STORM_VERSION_GIT_HASH)
git_describe_checkout(STORM_GIT_VERSION_STRING)

# parse the git tag into variables
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" STORM_VERSION_MAJOR "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" STORM_VERSION_MINOR "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" STORM_VERSION_PATCH "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-([0-9]+)\\-.*" "\\1" STORM_VERSION_COMMITS_AHEAD "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-([a-z0-9]+).*" "\\1" STORM_VERSION_TAG_HASH "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-[a-z0-9]+\\-(.*)" "\\1" STORM_VERSION_APPENDIX "${STORM_GIT_VERSION_STRING}")
	
# now check whether the git version lookup failed
if (STORM_VERSION_MAJOR MATCHES "NOTFOUND")
	set(STORM_VERSION_MAJOR 1)
	set(STORM_VERSION_MINOR 0)
	set(STORM_VERSION_PATCH 2)
	set(STORM_VERSION_GIT_HASH "")
	set(STORM_VERSION_COMMITS_AHEAD 0)
	set(STORM_VERSION_DIRTY boost::none)

	message(WARNING "Storm - git version information not available, statically assuming version ${STORM_VERSION_MAJOR}.${STORM_VERSION_MINOR}.${STORM_VERSION_PATCH}.")
else()
	if ("${STORM_VERSION_APPENDIX}" MATCHES "^.*dirty.*$")
		set(STORM_VERSION_DIRTY "true")
	else()
		set(STORM_VERSION_DIRTY "false")
	endif()
endif()

set(STORM_VERSION_DEV "false")
set(STORM_VERSION_DEV_STRING "")
if(STORM_VERSION_COMMITS_AHEAD)
	MATH(EXPR STORM_VERSION_DEV_PATCH "${STORM_VERSION_PATCH}+1")
	set(STORM_VERSION_DEV "true")
	set(STORM_VERSION_DEV_STRING " (dev)")
else()
	set(STORM_VERSION_DEV_PATCH ${STORM_VERSION_PATCH})
endif()

set(STORM_VERSION "${STORM_VERSION_MAJOR}.${STORM_VERSION_MINOR}.${STORM_VERSION_DEV_PATCH}")
set(STORM_VERSION_STRING "${STORM_VERSION}${STORM_VERSION_DEV_STRING}")

message(STATUS "Storm - version is ${STORM_VERSION_STRING} (version ${STORM_VERSION_MAJOR}.${STORM_VERSION_MINOR}.${STORM_VERSION_PATCH} + ${STORM_VERSION_COMMITS_AHEAD} commits), building from git: ${STORM_VERSION_GIT_HASH} (dirty: ${STORM_VERSION_DIRTY}).")


# Configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/storm-config.h.in"
	"${PROJECT_BINARY_DIR}/include/storm-config.h"
)

# Configure a source file to pass the Storm version to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/storm-version.cpp.in"
	"${PROJECT_SOURCE_DIR}/src/storm/utility/storm-version.cpp"
)

set(STORM_GENERATED_SOURCES "${PROJECT_BINARY_DIR}/src/storm/utility/storm-version.cpp")

# Add the binary dir include directory for storm-config.h
include_directories("${PROJECT_BINARY_DIR}/include")

include(CTest)
# Compiles all tests
add_custom_target(tests)
# Compiles and runs all tests
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
set(CMAKE_CTEST_COMMAND_VERBOSE ${CMAKE_CTEST_COMMAND} -V)
add_custom_target(check-verbose COMMAND ${CMAKE_CTEST_COMMAND_VERBOSE})
add_dependencies(check tests)
add_dependencies(check-verbose tests)

set(STORM_TARGETS "")
add_subdirectory(src)

include(export)

include(StormCPackConfig.cmake)
