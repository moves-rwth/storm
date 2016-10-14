cmake_minimum_required (VERSION 2.8.6)
cmake_policy(VERSION 3.2)
# Set project name
project (storm CXX C)

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")


# Add the resources/cmake folder to Module Search Path for FindTBB.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/resources/cmake/find_modules" "${PROJECT_SOURCE_DIR}/resources/cmake/macros")

include(ExternalProject)


include(RegisterSourceGroup)
#############################################################
##
##	CMake options of StoRM
##
#############################################################
option(STORM_DEVELOPER "Sets whether the development mode is used" OFF)
option(STORM_USE_POPCNT "Sets whether the popcnt instruction is going to be used." ON)
MARK_AS_ADVANCED(STORM_USE_POPCNT)
option(USE_BOOST_STATIC_LIBRARIES "Sets whether the Boost libraries should be linked statically." ON)
option(STORM_USE_INTELTBB "Sets whether the Intel TBB libraries should be used." OFF)
option(STORM_USE_GUROBI "Sets whether Gurobi should be used." OFF)
option(STORM_USE_COTIRE "Sets whether Cotire should be used (for building precompiled headers)." OFF)
option(LINK_LIBCXXABI "Sets whether libc++abi should be linked." OFF)
option(USE_LIBCXX "Sets whether the standard library is libc++." OFF)
option(USE_CARL "Sets whether carl should be included." ON)
option(USE_XERCES "Sets whether xerces should be used." OFF)
option(FORCE_COLOR "Force color output" OFF)
mark_as_advanced(FORCE_COLOR)
option(STORM_PYTHON "Builds the API for Python" OFF)
option(STORM_COMPILE_WITH_CCACHE "Compile using CCache [if found]" ON)
mark_as_advanced(STORM_COMPILE_WITH_CCACHE)
option(STORM_LOG_DISABLE_DEBUG "Disable log and trace message support" OFF)
set(BOOST_ROOT "" CACHE STRING "A hint to the root directory of Boost (optional).")
set(GUROBI_ROOT "" CACHE STRING "A hint to the root directory of Gurobi (optional).")
set(Z3_ROOT "" CACHE STRING "A hint to the root directory of Z3 (optional).")
set(CUDA_ROOT "" CACHE STRING "The hint to the root directory of CUDA (optional).")
set(MSAT_ROOT "" CACHE STRING "The hint to the root directory of MathSAT (optional).")
set(ADDITIONAL_INCLUDE_DIRS "" CACHE STRING "Additional directories added to the include directories.")
set(ADDITIONAL_LINK_DIRS "" CACHE STRING "Additional directories added to the link directories.")


# Set some CMAKE Variables as advanced
mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
mark_as_advanced(CMAKE_OSX_SYSROOT)
mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)

# If the STORM_DEVELOPER option was turned on, we will target a debug version.
if (STORM_DEVELOPER)
    message(STATUS "StoRM - Using development mode")
    set(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "Set the build type" FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSTORM_DEV")
endif()
message(STATUS "StoRM - Building ${CMAKE_BUILD_TYPE} version.")

if(STORM_COMPILE_WITH_CCACHE)
	find_program(CCACHE_FOUND ccache)
	mark_as_advanced(CCACHE_FOUND)
	if(CCACHE_FOUND)
		message(STATUS "StoRM - Using ccache")
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
	else()
		message(STATUS "Could not find ccache")
	endif()
endif()

# Base path for test files
set(STORM_CPP_TESTS_BASE_PATH "${PROJECT_SOURCE_DIR}/test")

set(STORMPY_OUTPUT_DIR "${PROJECT_BINARY_DIR}/stormpy")
set(STORMPY_SOURCE_DIR "${PROJECT_SOURCE_DIR}/stormpy")

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
message(STATUS "Operating system: ${OPERATING_SYSTEM}")


set(DYNAMIC_EXT ".so")
set(STATIC_EXT ".a")
if(MACOSX)
	set(DYNAMIC_EXT ".dylib")
	set(STATIC_EXT ".a")
elseif (WIN32)
	set(DYNAMIC_EXT ".dll")
	set(STATIC_EXT ".lib")
endif()
message(STATUS "Assuming extension for shared libraries: ${DYNAMIC_EXT}")
message(STATUS "Assuming extension for static libraries: ${STATIC_EXT}")


#############################################################
##
##	Compiler specific settings and definitions
##
#############################################################
# Path to the no-strict-aliasing target
set(CONVERSIONHELPER_TARGET "${PROJECT_SOURCE_DIR}/src/utility/ConversionHelper.cpp")

if(CMAKE_COMPILER_IS_GNUCC)
    set(STORM_COMPILED_BY "GCC")
    # Set standard flags for GCC
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops --fast-math -fno-finite-math-only")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -funroll-loops")

	# TODO: remove forcing the old version of optional as soon as the related Spirit bug is fixed:
	# https://svn.boost.org/trac/boost/ticket/12349
	# Fix thanks to: https://github.com/freeorion/freeorion/issues/777
    add_definitions(-DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_OPTIONAL_CONFIG_USE_OLD_IMPLEMENTATION_OF_OPTIONAL)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -Wno-deprecated-declarations -Wno-unused-local-typedefs -Wno-unknown-pragmas")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wno-deprecated-declarations")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
		message(FATAL_ERROR "GCC version must be at least 5.0!")
	endif()
    # Turn on popcnt instruction if desired (yes by default)
    if (STORM_USE_POPCNT)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
    endif(STORM_USE_POPCNT)

	# Set the no-strict-aliasing target for GCC
	set_source_files_properties(${CONVERSIONHELPER_TARGET} PROPERTIES COMPILE_FLAGS " -fno-strict-aliasing")
elseif(MSVC)
    set(STORM_COMPILED_BY "MSVC")
	# required for GMM to compile, ugly error directive in their code
	add_definitions(/D_SCL_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS)
	# required as the PRCTL Parser bloats object files (COFF) beyond their maximum size (see http://msdn.microsoft.com/en-us/library/8578y171(v=vs.110).aspx)
	add_definitions(/bigobj)
	# required by GTest and PrismGrammar::createIntegerVariable
	add_definitions(/D_VARIADIC_MAX=10)
	# Windows.h breaks GMM in gmm_except.h because of its macro definition for min and max
	add_definitions(/DNOMINMAX)
	# Boost Defs, required for using boost's transform iterator
	add_definitions(/DBOOST_RESULT_OF_USE_DECLTYPE)

	# since nobody cares at the moment
	add_definitions(/wd4250)

	# MSVC does not do strict-aliasing, so no option needed
else(CLANG)
    set(STORM_COMPILED_BY "Clang (LLVM)")
	# As CLANG is not set as a variable, we need to set it in case we have not matched another compiler.
	set (CLANG ON)
    # Set standard flags for clang
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops -ffast-math -fno-finite-math-only")

	if(UNIX AND NOT APPLE)
		if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
			message(FATAL_ERROR "Clang version must be at least 3.2!")
		endif()
	endif()

	if(UNIX AND APPLE)
		if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.3)
			message(FATAL_ERROR "Clang version must be at least 7.3!")
		endif()
	endif()

    if(UNIX AND NOT APPLE AND NOT USE_LIBCXX)
		set(CLANG_STDLIB libstdc++)
		message(STATUS "StoRM - Linking against libstdc++")
    else()
		set(CLANG_STDLIB libc++)
		message(STATUS "StoRM - Linking against libc++")
		# Disable Cotire
		set(STORM_USE_COTIRE OFF)
		# Set up some Xcode specific settings
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    endif()

	# TODO: remove forcing the old version of optional as soon as the related Spirit bug is fixed:
	# https://svn.boost.org/trac/boost/ticket/12349
	# Fix thanks to: https://github.com/freeorion/freeorion/issues/777
    add_definitions(-DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_OPTIONAL_CONFIG_USE_OLD_IMPLEMENTATION_OF_OPTIONAL)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=${CLANG_STDLIB} -Wall -pedantic -Wno-newline-eof -Wno-mismatched-tags -Wno-unused-local-typedefs -ftemplate-depth=1024 -Wno-parentheses-equality")

    if(FORCE_COLOR)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    endif()

    # Turn on popcnt instruction if desired (yes by default)
    if (STORM_USE_POPCNT)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
    endif(STORM_USE_POPCNT)

	# Set the no-strict-aliasing target for Clang
	set_source_files_properties(${CONVERSIONHELPER_TARGET} PROPERTIES COMPILE_FLAGS " -fno-strict-aliasing ")
endif()


if(CCACHE_FOUND)
	set(STORM_COMPILED_BY  "${STORM_COMPILED_BY} (ccache)")
endif()

message(STATUS "StoRM - Using Compiler Configuration: ${STORM_COMPILED_BY}")

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
##	Cotire
##
#############################################################
message (STATUS "StoRM - Using Cotire: ${STORM_USE_COTIRE}")

if (STORM_USE_COTIRE)
	# Include Cotire for PCH Generation
	set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/resources/cmake")
	include(cotire)

	cotire(storm)
	target_link_libraries(storm_unity ${Boost_LIBRARIES})
	#cotire(storm-functional-tests)
	#cotire(storm-performance-tests)
endif()

#############################################################
##
##	libc++abi
##
#############################################################
# Link against libc++abi if requested. May be needed to build on Linux systems using clang.
if (LINK_LIBCXXABI)
	message (STATUS "StoRM - Linking against libc++abi.")
	target_link_libraries(storm "c++abi")
endif(LINK_LIBCXXABI)


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
##	CMake-generated Config File for StoRM
##
#############################################################

#
# Make a version file containing the current version from git.
#
include(GetGitRevisionDescription)
git_describe_checkout(STORM_GIT_VERSION_STRING)
message(STATUS "STORM_GIT_VERSION_STRING: ${STORM_GIT_VERSION_STRING}")
# Parse the git Tag into variables
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" STORM_CPP_VERSION_MAJOR "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" STORM_CPP_VERSION_MINOR "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" STORM_CPP_VERSION_PATCH "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-([0-9]+)\\-.*" "\\1" STORM_CPP_VERSION_COMMITS_AHEAD "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-([a-z0-9]+).*" "\\1" STORM_CPP_VERSION_HASH "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-[a-z0-9]+\\-(.*)" "\\1" STORM_CPP_VERSION_APPENDIX "${STORM_GIT_VERSION_STRING}")
if ("${STORM_CPP_VERSION_APPENDIX}" MATCHES "^.*dirty.*$")
	set(STORM_CPP_VERSION_DIRTY 1)
else()
	set(STORM_CPP_VERSION_DIRTY 0)
endif()
message(STATUS "StoRM - Version information: ${STORM_CPP_VERSION_MAJOR}.${STORM_CPP_VERSION_MINOR}.${STORM_CPP_VERSION_PATCH} (${STORM_CPP_VERSION_COMMITS_AHEAD} commits ahead of Tag) build from ${STORM_CPP_VERSION_HASH} (Dirty: ${STORM_CPP_VERSION_DIRTY})")

# Configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/storm-config.h.in"
	"${PROJECT_BINARY_DIR}/include/storm-config.h"
)

# Configure a header file to pass the storm version to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/storm-version.cpp.in"
	"${PROJECT_SOURCE_DIR}/src/utility/storm-version.cpp"
)

set(STORM_GENERATED_SOURCES "${PROJECT_BINARY_DIR}/src/utility/storm-version.cpp")

# Add the binary dir include directory for storm-config.h
include_directories("${PROJECT_BINARY_DIR}/include")

add_subdirectory(src)
add_subdirectory(test)



#############################################################
##
##	memcheck targets
##
#############################################################
add_custom_target(memcheck valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm -v --fix-deadlocks ${PROJECT_SOURCE_DIR}/examples/dtmc/crowds/crowds5_5.tra examples/dtmc/crowds/crowds5_5.lab DEPENDS storm)
add_custom_target(memcheck-functional-tests valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm-functional-tests -v --fix-deadlocks	DEPENDS storm-functional-tests)
add_custom_target(memcheck-performance-tests valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm-performance-tests -v --fix-deadlocks DEPENDS storm-performance-tests)

set(CPPLINT_ARGS --filter=-whitespace/tab,-whitespace/line_length,-legal/copyright,-readability/streams)
add_custom_target(style python resources/3rdparty/cpplint/cpplint.py ${CPPLINT_ARGS} `find ./src/ -iname "*.h" -or -iname "*.cpp" `)

include(StormCPackConfig.cmake)
