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

#add_executable(MRMC-cpp src/mrmc-cpp.cpp)

FILE(GLOB_RECURSE MRMCSources ${PROJECT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE MRMCHeaders ${PROJECT_SOURCE_DIR}/src/*.h)

SOURCE_GROUP(Headers FILES ${MRMCHeaders})
SOURCE_GROUP(Sources FILES ${MRMCSources})

ADD_EXECUTABLE(MRMC-cpp ${MRMCSources} ${MRMCHeaders})

# Add base folder for better inclusion pathes
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

# external packages
FIND_PACKAGE(GTest REQUIRED)
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(Doxygen REQUIRED)

if(Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})
endif(Boost_FOUND)

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

add_executable(MRMC-tests ${PROJECT_SOURCE_DIR}/test/mrmc-tests.cpp)

include_directories(${GTEST_INCLUDE_DIRS})
target_link_libraries(MRMC-tests ${GTEST_LIBRARIES})

add_test(
    NAME MRMC-tests
    COMMAND MRMC-tests
)
endif(GTEST_FOUND)
