cmake_minimum_required(VERSION 2.6)
project(sylvan C CXX)
enable_testing()

set(CMAKE_C_FLAGS "-O3 -Wextra -Wall -fno-strict-aliasing -std=gnu11 -fPIC")
set(CMAKE_CXX_FLAGS "-O3 -Wextra -Wall -fno-strict-aliasing -Wno-deprecated-register -std=gnu++11 -fPIC")

option(USE_CARL "Sets whether carl should be included." ON)
option(WITH_COVERAGE "Add generation of test coverage" OFF)
if(WITH_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -coverage")

    find_program(GCOV_PATH gcov)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    add_custom_target(coverage
        # Cleanup lcov
        ${LCOV_PATH} --directory . --zerocounters
        # Run tests
        COMMAND make test
        # Capture counters
        COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} --directory . --capture --output-file coverage.info
        COMMAND ${LCOV_PATH} --remove coverage.info 'test/*' '/usr/*' 'examples/*' 'src/sylvan_mtbdd*' 'src/lace*' 'src/sylvan_ldd*' 'src/avl.h' 'src/sha2.c' --output-file coverage.info.cleaned
        COMMAND ${GENHTML_PATH} -o coverage coverage.info.cleaned
        COMMAND ${CMAKE_COMMAND} -E remove coverage.info coverage.info.cleaned
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

if(USE_CARL)
	find_package(carl QUIET REQUIRED)
	if(carl_FOUND)
               add_definitions(-DSYLVAN_HAVE_CARL)
		include_directories("${carl_INCLUDE_DIR}")
		list(APPEND STORM_LINK_LIBRARIES ${carl_LIBRARIES})
		message(STATUS "Sylvan - using CARL.")
	else()
		message(FATAL_ERROR "Sylvan - CARL was requested but not found")
	endif()
else()
	message(STATUS "Sylvan - not using CARL.")
endif()


set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIR})
include_directories("${PROJECT_SOURCE_DIR}/../../../")
include_directories("${PROJECT_BINARY_DIR}/../include")
include_directories(src)

include_directories(src)

add_subdirectory(src)

option(SYLVAN_BUILD_TEST "Build test programs" ON)

if(SYLVAN_BUILD_TEST)
    add_subdirectory(test)
endif()

option(SYLVAN_BUILD_EXAMPLES "Build example tools" OFF)

if(SYLVAN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
