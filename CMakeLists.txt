cmake_minimum_required (VERSION 2.6)

project (MRMC-cpp CXX C)

# The version number.
set (MRMC_CPP_VERSION_MAJOR 1)
set (MRMC_CPP_VERSION_MINOR 0)
 
#Configurations for GCC
if(CMAKE_COMPILER_IS_GNUCC) 
	set(MRMC_GCC_NO_DEBUG_SYMBOLS OFF CACHE BOOL "Whether debug symbols should be included in MRMC with GCC, set ON for better performance")
    #Using C++11        
    set (CMAKE_CXX_FLAGS "-std=c++0x")
    
    #Debug symbols
	if (NOT MRMC_GCC_NO_DEBUG_SYMBOLS)
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
		message(STATUS "Using Debug Symbols for GCC")
	endif()
endif()
 
# configure a header file to pass some of the CMake settings
# to the source code

# Base path for test files
set(MRMC_CPP_TESTS_BASE_PATH ${PROJECT_SOURCE_DIR}/test)

configure_file (
  "${PROJECT_SOURCE_DIR}/MRMCConfig.h.in"
  "${PROJECT_BINARY_DIR}/MRMCConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# Main Sources
file(GLOB_RECURSE MRMCHeaders ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE MRMCSources ${PROJECT_SOURCE_DIR}/src/*.cpp)
# Test Sources
# Note that the tests also need the source files, except the main file!
file(GLOB_RECURSE MRMCTestHeaders ${PROJECT_SOURCE_DIR}/test/*.h)
file(GLOB_RECURSE MRMCTestSources ${PROJECT_SOURCE_DIR}/test/*.cpp ${PROJECT_SOURCE_DIR}/src/*/*.cpp)
# Main Grouping
source_group(Headers FILES ${MRMCHeaders})
source_group(Sources FILES ${MRMCSources})
# Test Grouping
source_group(Headers FILES ${MRMCTestHeaders})
source_group(Sources FILES ${MRMCTestSources})

# Add base folder for better inclusion pathes
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

# temp search path for Pantheios/STLSoft
if (NOT GTEST_INCLUDE_DIR)
	set(GTEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.6.0/include)
endif()

if (NOT STLSOFT_INCLUDE_DIR)
	set(STLSOFT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/resources/3rdparty/stlsoft-1.9.116/include)
endif()

if (NOT PANTHEIOS_INCLUDE_DIR)
	set(PANTHEIOS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/resources/3rdparty/pantheios-1.0.1-beta214/include)
endif()

message(STATUS "GTEST Include Dir is: ${GTEST_INCLUDE_DIR}")
#message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
#list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.6.0)
#list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/resources/3rdparty/pantheios-1.0.1-beta214)
#list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/resources/3rdparty/gteststlsoft-1.9.116)
#message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

# external packages
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED)
find_package(Doxygen REQUIRED)
find_package(STLSoft REQUIRED)

# STLsoft headers
# include_directories("${PROJECT_SRC_DIR}/resources/3rdParty/stlsoft-1.9.114")
# Pantheios Logging library
# include_directories("${PROJECT_SRC_DIR}/resources/3rdParty/pantheios-1.0.1-beta213")

if (STLSOFT_FOUND)
    include_directories(${STLSOFT_INCLUDE_DIR})
endif()
find_package(STLSoft REQUIRED)

#Dynamic runtime for Pantheios is configured for dll-files, hence only usable under Windows...
if (WIN32)
    set(PANTHEIOS_USE_DYNAMIC_RUNTIME ON)
else()
    set(PANTHEIOS_USE_DYNAMIC_RUNTIME OFF)
endif()
find_package(Pantheios REQUIRED COMPONENTS SimpleFrontEnd File)

    
if (PANTHEIOS_FOUND)
    include_directories(${PANTHEIOS_INCLUDE_DIR})
    link_directories(${PANTHEIOS_LIBRARY_DIRS})
else()
    message(STATUS "NO PANTHEIOS!")
endif()
    
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})

	if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
		set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
	endif ()
	link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/resources/3rdparty/eigen/cmake/")

set(EIGEN3_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/resources/3rdparty/eigen)

find_package(Eigen3 REQUIRED)

if (EIGEN3_FOUND)
	include_directories(${EIGEN3_INCLUDE_DIR})
endif()

# must be created AFTER Boost was added because of LINK_DIRECTORIES
add_executable(MRMC-cpp ${MRMCSources} ${MRMCHeaders})
add_executable(MRMC-tests ${MRMCTestSources} ${MRMCTestHeaders})

# Add target link deps for Pantheios
target_link_libraries(MRMC-cpp ${PANTHEIOS_LIBRARIES})
target_link_libraries(MRMC-tests ${PANTHEIOS_LIBRARIES})

# add a target to generate API documentation with Doxygen
if(DOXYGEN_FOUND)

set(CMAKE_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc")
string(REGEX REPLACE ";" " " CMAKE_DOXYGEN_INPUT_LIST "${PROJECT_SOURCE_DIR}/src")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

add_custom_target(doc ALL
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

if (GTEST_FOUND)
	# For make-based builds, defines make target named test.
	# For Visual Studio builds, defines Visual Studio project named RUN_TESTS.
	enable_testing()

	include_directories(${GTEST_INCLUDE_DIRS})
	target_link_libraries(MRMC-tests ${GTEST_LIBRARIES})

	add_test(
		NAME MRMC-tests
		COMMAND MRMC-tests
	)
	if( MSVC ) # VS2012 doesn't support correctly the tuples yet
		add_definitions( /D _VARIADIC_MAX=10 )
	endif()
endif(GTEST_FOUND)


if (THREADS_FOUND)
	include_directories(${THREADS_INCLUDE_DIRS})
	target_link_libraries (MRMC-tests ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries (MRMC-cpp ${CMAKE_THREAD_LIBS_INIT})
endif(THREADS_FOUND)