cmake_minimum_required (VERSION 2.6)

project (MRMC-cpp CXX C)

# The version number.
set (MRMC_CPP_VERSION_MAJOR 1)
set (MRMC_CPP_VERSION_MINOR 0)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/MRMCConfig.h.in"
  "${PROJECT_BINARY_DIR}/MRMCConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# Main Sources
FILE(GLOB_RECURSE MRMCHeaders ${PROJECT_SOURCE_DIR}/src/*.h)
FILE(GLOB_RECURSE MRMCSources ${PROJECT_SOURCE_DIR}/src/*.cpp)
# Test Sources
FILE(GLOB_RECURSE MRMCTestHeaders ${PROJECT_SOURCE_DIR}/test/*.h)
FILE(GLOB_RECURSE MRMCTestSources ${PROJECT_SOURCE_DIR}/test/*.cpp)
# Main Grouping
SOURCE_GROUP(Headers FILES ${MRMCHeaders})
SOURCE_GROUP(Sources FILES ${MRMCSources})
# Test Grouping
SOURCE_GROUP(Headers FILES ${MRMCTestHeaders})
SOURCE_GROUP(Sources FILES ${MRMCTestSources})

# Add base folder for better inclusion pathes
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

# temp search path for Pantheios/STLSoft
message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

# external packages
FIND_PACKAGE(GTest REQUIRED)
find_package(Threads REQUIRED)
SET(Boost_USE_STATIC_LIBS        ON)
SET(Boost_USE_MULTITHREADED      ON)
SET(Boost_USE_STATIC_RUNTIME    OFF)
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(Doxygen REQUIRED)
FIND_PACKAGE(STLSoft REQUIRED)

set(PANTHEIOS_USE_DYNAMIC_RUNTIME ON)
FIND_PACKAGE(Pantheios REQUIRED COMPONENTS SimpleFrontEnd File)

# STLsoft headers
# include_directories("${PROJECT_SRC_DIR}/resources/3rdParty/stlsoft-1.9.114")
# Pantheios Logging library
# include_directories("${PROJECT_SRC_DIR}/resources/3rdParty/pantheios-1.0.1-beta213")

if (STLSOFT_FOUND)
	message(STATUS "STLSoft found")
	INCLUDE_DIRECTORIES(${STLSOFT_INCLUDE_DIR})
endif()

IF (PANTHEIOS_FOUND)
	message(STATUS "Pantheios found")
	INCLUDE_DIRECTORIES(${PANTHEIOS_INCLUDE_DIR})
	LINK_DIRECTORIES(${PANTHEIOS_LIBRARY_DIRS})
else()
	message(STATUS "NO PANTHEIOS!")
ENDIF()
	
if(Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})

if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
	set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
	message(STATUS "Boost_LIBRARY_DIRS was empty, setting to ${Boost_LIBRARY_DIRS}")
endif ()

link_directories(${Boost_LIBRARY_DIRS})

endif(Boost_FOUND)

# must be created AFTER Boost was added because of LINK_DIRECTORIES
ADD_EXECUTABLE(MRMC-cpp ${MRMCSources} ${MRMCHeaders})
ADD_EXECUTABLE(MRMC-tests ${MRMCTestSources} ${MRMCTestHeaders})

# Add target link deps for Pantheios
target_link_libraries(MRMC-cpp ${PANTHEIOS_LIBRARIES})
target_link_libraries(MRMC-tests ${PANTHEIOS_LIBRARIES})

# add a target to generate API documentation with Doxygen
if(DOXYGEN_FOUND)

SET(CMAKE_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc")
STRING(REGEX REPLACE ";" " " CMAKE_DOXYGEN_INPUT_LIST "${PROJECT_SOURCE_DIR}/src")

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
endif(GTEST_FOUND)
