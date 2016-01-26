cmake_minimum_required(VERSION 2.6)
project(sylvan C CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIR})
#message(STATUS "Include directory ${GMP_INCLUDE_DIR}")
include_directories(src)

add_subdirectory(src)

option(SYLVAN_BUILD_TEST "Build test programs" ON)

if(SYLVAN_BUILD_TEST)
    add_subdirectory(test)
endif()

option(SYLVAN_BUILD_EXAMPLES "Build example tools" ON)

if(SYLVAN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
