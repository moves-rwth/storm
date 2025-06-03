cmake_minimum_required (VERSION 3.23)
if (NOT ${CMAKE_VERSION} VERSION_LESS 3.24)
	cmake_policy(VERSION 3.24)
endif()

# Set project name
project (storm
		LANGUAGES CXX C
		VERSION 1.10.0.1)
# Version x.y.z.1 should be read as: This is an updated version of x.y.z that has not been released as x.y.z+1 yet.

# Add the resources/cmake folder to Module Search Path
set(STORM_CMAKE_FIND_MODULES "${PROJECT_SOURCE_DIR}/resources/cmake/find_modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${STORM_CMAKE_FIND_MODULES}" "${PROJECT_SOURCE_DIR}/resources/cmake/macros")
add_custom_target(storm_resources)
if(PROJECT_IS_TOP_LEVEL)
	add_custom_target(resources)
	add_dependencies(resources storm_resources)
endif()


include(ExternalProject)
include(imported)
include(CheckCXXSourceCompiles)
include(CheckCSourceCompiles)
include(ProcessorCount)

set(CMAKE_CXX_STANDARD 20)

#############################################################
##
##	CMake options of Storm
##
#############################################################
## Compilation options.
option(STORM_DEVELOPER "Sets whether the development mode is used." OFF)
option(STORM_ALLWARNINGS "Compile with even more warnings" OFF)
option(STORM_USE_LTO "Sets whether link-time optimizations are enabled." ON)
MARK_AS_ADVANCED(STORM_USE_LTO)
option(STORM_USE_THIN_LTO "Sets whether thin link-time optimizations are enabled (faster compile times than normal LTO)." OFF)
MARK_AS_ADVANCED(STORM_USE_THIN_LTO)
option(STORM_PORTABLE "Sets whether a build needs to be portable." OFF)
MARK_AS_ADVANCED(STORM_PORTABLE)
# Force POPCNT is helpful for portable code targetting platforms with SSE4.2 operation support.
option(STORM_FORCE_POPCNT "Sets whether the popcnt instruction is forced to be used (advanced)." OFF)
MARK_AS_ADVANCED(STORM_FORCE_POPCNT)
option(STORM_COMPILE_WITH_CCACHE "Compile using CCache [if found]" ON)
mark_as_advanced(STORM_COMPILE_WITH_CCACHE)
option(STORM_LOG_DISABLE_DEBUG "Disable log and trace message support" OFF)
option(STORM_COMPILE_WITH_ADDRESS_SANITIZER "Sets whether to compile with AddressSanitizer enabled" OFF)
option(STORM_COMPILE_WITH_ALL_SANITIZERS "Sets whether to compile with all sanitizers enabled" OFF)
option(STORM_COMPILE_WITH_COMPILATION_PROFILING "Compile with output to profile compilation process" OFF)
MARK_AS_ADVANCED(STORM_COMPILE_WITH_COMPILATION_PROFILING)
option(STORM_USE_CLN_EA "Sets whether CLN instead of GMP numbers should be used for exact arithmetic." OFF)
export_option(STORM_USE_CLN_EA)
option(STORM_USE_CLN_RF "Sets whether CLN instead of GMP numbers should be used for rational functions." ON)
export_option(STORM_USE_CLN_RF)

## Configuration setup.
option(STORM_EXCLUDE_TESTS_FROM_ALL "If set, tests will not be compiled by default" OFF )
export_option(STORM_EXCLUDE_TESTS_FROM_ALL)
MARK_AS_ADVANCED(STORM_EXCLUDE_TESTS_FROM_ALL)
option(STORM_LOAD_QVBS "Sets whether the Quantitative Verification Benchmark Set (QVBS) should be downloaded." OFF)
set(STORM_QVBS_ROOT "" CACHE STRING "The root directory of the Quantitative Verification Benchmark Set (QVBS) in case it should not be downloaded (optional).")
MARK_AS_ADVANCED(STORM_QVBS_ROOT)

## Configuring dependencies
set(STORM_CARL_GIT_REPO "https://github.com/moves-rwth/carl-storm.git" CACHE STRING "For fetching carl: which git repo to use")
set(STORM_CARL_GIT_TAG "14.29" CACHE STRING "For fetching carl: which git tag to use")
option(STORM_DISABLE_GMM "Sets whether linking with GMM should be disabled." OFF)
option(STORM_DISABLE_GUROBI "Sets whether linking with Gurobi should be disabled." OFF)
set(GUROBI_ROOT "" CACHE STRING "A hint to the root directory of Gurobi.")
MARK_AS_ADVANCED(GUROBI_ROOT)
option(STORM_DISABLE_MATHSAT "Sets whether linking with MathSat should be disabled." OFF)
set(MATHSAT_ROOT "" CACHE STRING "A hint to the root directory of MathSAT.")
MARK_AS_ADVANCED(MATHSAT_ROOT)
option(STORM_DISABLE_SPOT "Sets whether linking with Spot should be disabled." OFF)
option(STORM_SPOT_FORCE_SHIPPED "Sets whether system versions of Spot should be ignored (if available)." OFF)
set(SPOT_ROOT "" CACHE STRING "A hint to the root directory of Spot.")
MARK_AS_ADVANCED(SPOT_ROOT)
option(STORM_DISABLE_SOPLEX "Sets whether linking with Soplex should be disabled." OFF)
set(SOPLEX_ROOT "" CACHE STRING "A hint to the root directory of Soplex.")
MARK_AS_ADVANCED(SOPLEX_ROOT)
option(STORM_DISABLE_XERCES "Sets whether linking with Xerces should be disabled." OFF)
set(XERCESC_ROOT "" CACHE STRING "A hint to the root directory of Xerces (optional).")
mark_as_advanced(XERCESC_ROOT)
option(STORM_DISABLE_Z3  "Sets whether linking with Spot should be disabled." Z3)
set(Z3_ROOT "" CACHE STRING "A hint to the root directory of Z3.")
mark_as_advanced(Z3_ROOT)

option(STORM_DEBUG_CUDD "Build CUDD in debug mode." OFF)
MARK_AS_ADVANCED(STORM_DEBUG_CUDD)
option(STORM_DEBUG_SYLVAN "Build Sylvan in debug mode." OFF)
MARK_AS_ADVANCED(STORM_DEBUG_SYLVAN)

if (STORM_COMPILE_WITH_ALL_SANITIZERS)
    set(STORM_COMPILE_WITH_ADDRESS_SANITIZER ON)
endif()

# Get an approximation of the number of available processors (used for parallel build of shipped resources)
ProcessorCount(STORM_RESOURCES_BUILD_JOBCOUNT_DEFAULT)
# To be safe, we only take a little more than half of the resources.
# This also correctly deals with the case where ProcessorCount is unable to find the correct number (and thus returns 0)
MATH(EXPR STORM_RESOURCES_BUILD_JOBCOUNT_DEFAULT "${STORM_RESOURCES_BUILD_JOBCOUNT_DEFAULT}/2 + 1")
set(STORM_RESOURCES_BUILD_JOBCOUNT "${STORM_RESOURCES_BUILD_JOBCOUNT_DEFAULT}" CACHE STRING "The number of jobs used when building external resources")
mark_as_advanced(STORM_RESOURCES_BUILD_JOBCOUNT)
if(NOT STORM_RESOURCES_BUILD_JOBCOUNT GREATER 0)
	message(FATAL_ERROR "STORM_RESOURCES_BUILD_JOBCOUNT must be a positive number. Got '${STORM_RESOURCES_BUILD_JOBCOUNT}' instead." )
endif()

# Set some CMAKE Variables as advanced
mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
mark_as_advanced(CMAKE_OSX_SYSROOT)
mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)

# Offer the user the choice of overriding the installation directories
set(STORM_INCLUDE_INSTALL_DIR include/storm CACHE PATH "Installation directory for header files" )
set(STORM_LIB_INSTALL_DIR lib/storm CACHE PATH "Installation directory for libraries")
set(STORM_BIN_INSTALL_DIR bin/storm CACHE PATH "Installation directory for executables")
set(STORM_CMAKE_INSTALL_DIR lib/cmake/storm CACHE PATH  "Installation directory for CMake files")
set(STORM_RESOURCE_INCLUDE_INSTALL_DIR include/storm/resources/ CACHE PATH "Installation directory for dependency header files")
set(STORM_RESOURCE_LIBRARY_INSTALL_DIR lib/storm/resources/ CACHE PATH "Installation directory for depency library files")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# If the STORM_DEVELOPER option was turned on, by default we target a debug version, otherwise a release version.
if (STORM_DEVELOPER)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "DEBUG")
    endif()
    # TODO Do this more targeted
    add_definitions(-DSTORM_DEV)
	set(STORM_DEBUG_CUDD ON)
	set(STORM_DEBUG_SYLVAN ON)
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
else()
    message(STATUS "Storm - Disabled use of ccache.")
endif()

# Directory for test resources.
set(STORM_TEST_RESOURCES_DIR "${PROJECT_SOURCE_DIR}/resources/examples/testfiles")

# Auto-detect operating system.
set(MACOSX 0)
set(LINUX 0)
set(APPLE_SILICON 0)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# Mac OS
	set(OPERATING_SYSTEM "Mac OS")
        set(MACOSX 1)
	if(${CMAKE_SYSTEM_PROCESSOR} MATCHES arm64)
		set(APPLE_SILICON 1)
	endif()
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

set(STATIC_EXT ".a")
set(LIB_PREFIX "lib")
if(MACOSX)
	set(DYNAMIC_EXT ".dylib")
else()
	set(DYNAMIC_EXT ".so")
endif()
message(VERBOSE "Storm - Assuming extension for shared libraries: ${DYNAMIC_EXT}")
message(VERBOSE "Storm - Assuming extension for static libraries: ${STATIC_EXT}")

#############################################################
##
##	Compiler detection and initial configuration
##
#############################################################
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# using Clang
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
		message(FATAL_ERROR "Clang version must be at least 10.0")
	endif()

	set(STORM_COMPILER_CLANG ON)
	set(CLANG ON)
	set(STORM_COMPILER_ID "clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	# using AppleClang
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14.0.3)
		message(FATAL_ERROR "AppleClang version must be at least 14.0.3 (Xcode version 14.3).")
	elseif (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 15.0.0)
		message(WARNING "Disabling stack checks for AppleClang version < 15.0")
		# Stack checks are known to produce errors with the following Clang versions:
		# 11.0.0: Runtime errors (stack_not_16_byte_aligned_error) when invoking storm in release mode
		# 11.0.3 and  12.0.0: Catching exceptions thrown within PRISM parser does not work (The exception just falls through)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-stack-check")
	endif()

	set(STORM_COMPILER_APPLECLANG ON)
	set(CLANG ON)
	set(STORM_COMPILER_ID "AppleClang")
	set(CMAKE_MACOSX_RPATH ON)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(GCC ON)
	# using GCC
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
		message(FATAL_ERROR "gcc version must be at least 10.0.")
	endif()

	set(STORM_COMPILER_GCC ON)
	set(STORM_COMPILER_ID "gcc")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	message(FATAL_ERROR "Intel compiler is currently not supported.")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	message(FATAL_ERROR "Visual Studio compiler is currently not supported.")
else()
    message(FATAL_ERROR "Unknown compiler '${CMAKE_CXX_COMPILER_ID}' is not supported")
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

#############################################################
##
##	Compiler specific settings
##
#############################################################
if (STORM_COMPILER_CLANG OR STORM_COMPILER_APPLECLANG)
	if (LINUX)
		set(CLANG_STDLIB libstdc++)
    else()
		set(CLANG_STDLIB libc++)
    endif()

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=${CLANG_STDLIB} -ftemplate-depth=1024")

    if(LINUX)
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
    elseif(MACOSX)
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-export_dynamic")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-export_dynamic")
    endif()
elseif (STORM_COMPILER_GCC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fprefetch-loop-arrays")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

	# Do not warn about problems internally to GCC 12
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-overflow")
endif ()

if (STORM_USE_THIN_LTO)
    if (STORM_COMPILER_CLANG OR STORM_COMPILER_APPLECLANG)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto=thin")
        message(STATUS "Storm - Enabling link-time optimizations using ThinLTO.")
    else()
        message(FATAL_ERROR "Storm - ThinLTO only supported for Clang. Use regular LTO instead.")
    endif()
elseif (STORM_USE_LTO)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")

    # Fix for problems that occurred when using LTO on gcc. This should be removed when it
    # is not needed anymore as it makes the already long link-step potentially longer.
    if (STORM_COMPILER_GCC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto-partition=none")
    endif()

    message(STATUS "Storm - Enabling link-time optimizations.")
else()
    message(STATUS "Storm - Disabling link-time optimizations.")
endif()

if (STORM_COMPILE_WITH_ADDRESS_SANITIZER)
    message(STATUS "Storm - Enabling AddressSanitizer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

# In release mode, we turn on even more optimizations if we do not have to provide a portable binary.
if (NOT STORM_PORTABLE AND (NOT APPLE_SILICON OR (STORM_COMPILER_CLANG AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15.0)))
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
endif()

if (STORM_COMPILE_WITH_COMPILATION_PROFILING)
	if (CLANG)
		# Allow profiling of compilation times,
		# outputs json with chromium-tracing information along every object file (open in chrome://tracing/).
		# overhead is very limited
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-trace")
	endif()
endif()

if (STORM_DEVELOPER)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
	if (STORM_ALLWARNINGS)
		# With gcc, we already set (almost) all warnings.
		# With Clang, we can even go further using Weverything, which is not always helpful, which is why these options are guarded by  STORM_ALL_WARNINGS
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
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undefined-func-template")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-double-promotion")

			# Reenable soon
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-non-virtual-dtor")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-conditional-uninitialized")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-float-conversion")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unreachable-code-break")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ctad-maybe-unsupported")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-template")

			# Requires adapter
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-comma")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-used-but-marked-unused")

			# Requires adapter for gmock
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undef")


			# Conflicts with warnings in gcc
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unreachable-code-return")

            # ?? unclear semantics
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ctad-maybe-unsupported")

			# Potentially useful, but just too many right now
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-exception-parameter")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-zero-as-null-pointer-constant")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow-field-in-constructor")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-destructor-override")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-destructor-override")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-dynamic-exception-spec")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-extra-semi")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shorten-64-to-32")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-conversion")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-override")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-copy-with-dtor")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow-field")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-documentation-unknown-command")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-noreturn")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes")
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
##	Set precompiled headers
##
#############################################################

# Selection is based on (relatively extensive, but ad-hoc) compilation profiling in late 2023.
# As changes mean recompilations of the complete code base, we only use std libraries and boost libraries here for now.
SET(STORM_PRECOMPILED_HEADERS "<any>" "<map>" "<vector>" "<unordered_set>" "<forward_list>"
		"<optional>" "<ostream>" "<istream>" "<list>" "<set>" "<fstream>" "<string>" "<boost/optional.hpp>" "<boost/variant.hpp>"
		"<boost/container/flat_set.hpp>" "<boost/dynamic_bitset.hpp>" "<boost/range/irange.hpp>")

#############################################################
##
##	Compiler tests during config
##
#############################################################

# Test compiler by compiling small program.
CHECK_C_SOURCE_COMPILES("
	#include <stdio.h>
	int main() {
		const char* text = \"A Storm is coming.\";
		return 0;
	}"
	COMPILER_C_WORKS
)
CHECK_CXX_SOURCE_COMPILES("
	#include <string>
	int main() {
		const std::string text = \"A Storm is coming.\";
		return 0;
	}"
	COMPILER_CXX_WORKS
)
if ((NOT COMPILER_CXX_WORKS) OR (NOT COMPILER_C_WORKS))
	if (MACOSX)
		message(FATAL_ERROR "The C/C++ compiler is not configured correctly.\nTry running 'xcode-select --install'.")
	else()
		message(FATAL_ERROR "The C/C++ compiler is not configured correctly.")
	endif()
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
# TODO move these to the right targets rather than setting them globally.
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib/storm;${CMAKE_INSTALL_PREFIX}/lib/storm/resources")

# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#############################################################
##
##	Generator specific settings
##
#############################################################
# Display information about build configuration.
message(STATUS "Storm - Using compiler configuration ${STORM_COMPILER_ID} ${STORM_COMPILER_VERSION}.")
message(STATUS "Storm - Build type: ${CMAKE_BUILD_TYPE}")

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

if(STORM_HAVE_GLPK OR STORM_HAVE_GUROBI OR STORM_HAVE_SOPLEX OR STORM_HAVE_Z3_OPTIMIZE)
	set(STORM_HAVE_LP_SOLVER ON)
	message(STATUS "Storm - Compiled with an LP solver.")
else()
	set(STORM_HAVE_LP_SOLVER OFF)
	message(WARNING "Storm - Compiled without an LP solver. This may yield errors at run time.")
endif()

if(PROJECT_IS_TOP_LEVEL)
	# Include Doxygen
	include(resources/doxygen/CMakeLists.txt)
endif()
#############################################################
##
##	CMake-generated Config File for Storm
##
#############################################################
if (PROJECT_IS_TOP_LEVEL)
	# try to obtain the current version from git.
	include(GetGitRevisionDescription)
	get_git_head_revision(STORM_VERSION_REFSPEC STORM_VERSION_GIT_HASH)
	git_describe(STORM_GIT_VERSION_STRING)

	# Trying to figure out if there git has changed.
	string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(.*)$" STORM_VERSION_MATCH "${STORM_GIT_VERSION_STRING}")
	# No longer used, but help understanding the regex.
	#set(STORM_VERSION_MAJOR "${CMAKE_MATCH_1}")
	#set(STORM_VERSION_MINOR "${CMAKE_MATCH_2}")
	#set(STORM_VERSION_PATCH "${CMAKE_MATCH_3}")
	set(STORM_GIT_VERSION_REST "${CMAKE_MATCH_4}")
	# parse rest of the form (-label)-commitsahead-hash(-appendix)
	string(REGEX MATCH "^(\\-([a-z][a-z0-9\\.]+))?\\-([0-9]+)\\-([a-z0-9]+)(\\-.*)?$" STORM_VERSION_REST_MATCH "${STORM_GIT_VERSION_REST}")
	# No longer used, but structure helps understanding the regex.
	#set(STORM_VERSION_LABEL "${CMAKE_MATCH_2}") # might be empty
	#set(STORM_VERSION_COMMITS_AHEAD "${CMAKE_MATCH_3}")
	#set(STORM_VERSION_TAG_HASH "${CMAKE_MATCH_4}") # is not used
	set(STORM_VERSION_APPENDIX "${CMAKE_MATCH_5}") # might be empty
endif()


# check whether the git version lookup failed
if (STORM_GIT_VERSION_STRING MATCHES "NOTFOUND")
	set(STORM_VERSION_DIRTY DirtyState::Unknown)
else()
	if ("${STORM_VERSION_APPENDIX}" MATCHES "^.*dirty.*$")
		set(STORM_VERSION_DIRTY "DirtyState::Dirty")
	else()
		set(STORM_VERSION_DIRTY "DirtyState::Clean")
	endif()
endif()

# set final Storm version

set(STORM_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(STORM_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(STORM_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(STORM_VERSION "${STORM_VERSION_MAJOR}.${STORM_VERSION_MINOR}.${STORM_VERSION_PATCH}")
if(PROJECT_VERSION_TWEAK GREATER 0)
	set(STORM_VERSION_DEV 1)
else()
	set(STORM_VERSION_DEV 0)
endif()

set(STORM_VERSION_STRING "${STORM_VERSION}")
if(STORM_VERSION_DEV GREATER 0)
	set(STORM_VERSION_STRING "${STORM_VERSION_STRING} (dev)")
endif()

message(STATUS "Storm - Version is ${STORM_VERSION_STRING}.")


# Configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/src/storm-config.h.in"
	"${PROJECT_BINARY_DIR}/include/storm-config.h"
)

if(PROJECT_IS_TOP_LEVEL)
	include(CTest)
	# Compiles all tests
	add_custom_target(tests)
	# Compiles and runs all tests
	add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)
	set(CMAKE_CTEST_COMMAND_VERBOSE ${CMAKE_CTEST_COMMAND} -V)
	add_custom_target(check-verbose COMMAND ${CMAKE_CTEST_COMMAND_VERBOSE})
	add_dependencies(check tests)
	add_dependencies(check-verbose tests)
endif()

if(PROJECT_IS_TOP_LEVEL)
	# Apply code formatting
	add_custom_target(format COMMAND ${PROJECT_SOURCE_DIR}/resources/scripts/auto-format.sh)
endif()

set(STORM_TARGETS "")
add_subdirectory(src)

export_option(STORM_HAVE_GMM)
export_option(STORM_HAVE_GUROBI)
export_option(STORM_HAVE_SPOT)
export_option(STORM_HAVE_SOPLEX)
export_option(STORM_HAVE_XERCES)
export_option(STORM_HAVE_Z3)

include(export)

install(DIRECTORY ${STORM_CMAKE_FIND_MODULES} DESTINATION ${STORM_CMAKE_INSTALL_DIR})
install(FILES ${PROJECT_BINARY_DIR}/stormConfig.cmake.install DESTINATION ${STORM_CMAKE_INSTALL_DIR} RENAME stormConfig.cmake)
install(FILES ${PROJECT_BINARY_DIR}/stormConfigVersion.cmake DESTINATION ${STORM_CMAKE_INSTALL_DIR})
install(EXPORT storm_Targets FILE stormTargets.cmake DESTINATION ${STORM_CMAKE_INSTALL_DIR})

include(StormCPackConfig.cmake)
