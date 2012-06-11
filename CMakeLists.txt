cmake_minimum_required (VERSION 2.6)

project (MRMC-cpp)

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

add_executable(MRMC-cpp src/mrmc-cpp.cpp)