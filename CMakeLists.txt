cmake_minimum_required (VERSION 2.6)

# Set project name
project (storm CXX C)

# Set the version number
set (STORM_CPP_VERSION_MAJOR 1)
set (STORM_CPP_VERSION_MINOR 0)

# Set all GTest references to the version in the repository and show it as output
set (GTEST_INCLUDE_DIR resources/3rdparty/gtest-1.6.0/include)
if(MSVC)
	set (STORM_LIB_SUFFIX lib)
	set (GTEST_LIBRARY_DEBUG ${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.6.0/build/Debug/gtest.${STORM_LIB_SUFFIX})
	set (GTEST_MAIN_LIBRARY_DEBUG ${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.6.0/build/Debug/gtest_main.${STORM_LIB_SUFFIX})
	set (GTEST_LIBRARY ${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.6.0/build/Release/gtest.${STORM_LIB_SUFFIX})
	set (GTEST_MAIN_LIBRARY ${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.6.0/build/Release/gtest_main.${STORM_LIB_SUFFIX})
	set (GTEST_LIBRARIES optimized ${GTEST_LIBRARY} debug ${GTEST_LIBRARY_DEBUG})
else()
	set (STORM_LIB_SUFFIX a)
	set (GTEST_LIBRARY ${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.6.0/libgtest.${STORM_LIB_SUFFIX})
	set (GTEST_MAIN_LIBRARY ${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.6.0/libgtest_main.${STORM_LIB_SUFFIX})
	set (GTEST_LIBRARIES ${GTEST_LIBRARY}) # as we dont use FindGTest anymore
endif()
message(STATUS "GTEST_INCLUDE_DIR is ${GTEST_INCLUDE_DIR}")
message(STATUS "GTEST_LIBRARY is ${GTEST_LIBRARY}")
message(STATUS "GTEST_MAIN_LIBRARY is ${GTEST_MAIN_LIBRARY}")

# Set all log4cplus references the version in the repository
set (LOG4CPLUS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/resources/3rdparty/log4cplus-1.1.0/include)
if (MSVC)
	set (LOG4CPLUS_LIBRARIES optimized ${PROJECT_SOURCE_DIR}/resources/3rdparty/log4cplus-1.1.0/msvc10/x64/bin.Release/log4cplusS.${STORM_LIB_SUFFIX} debug ${PROJECT_SOURCE_DIR}/resources/3rdparty/log4cplus-1.1.0/msvc10/x64/bin.Debug/log4cplusSD.${STORM_LIB_SUFFIX})
else()
	set (LOG4CPLUS_LIBRARIES ${PROJECT_SOURCE_DIR}/resources/3rdparty/log4cplus-1.1.0/src/liblog4cplus.${STORM_LIB_SUFFIX})
endif()
message(STATUS "LOG4CPLUS_INCLUDE_DIR is ${LOG4CPLUS_INCLUDE_DIR}")
message(STATUS "LOG4CPLUS_LIBRARY is ${LOG4CPLUS_LIBRARIES}")

# Set all Eigen references the version in the repository
set(EIGEN3_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/resources/3rdparty/eigen)
message(STATUS "EIGEN3_INCLUDE_DIR is ${EIGEN3_INCLUDE_DIR}")

# Set all Eigen references the version in the repository
set(GMMXX_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/resources/3rdparty/gmm-4.2/include)
message(STATUS "GMMXX_INCLUDE_DIR is ${GMMXX_INCLUDE_DIR}")

# Set all CUDD references to the version in the repository
set(CUDD_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/include ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/obj ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/cudd ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/mtr ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/epd)
set(CUDD_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/cudd ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/mtr ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/util ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/st ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/epd ${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/obj)
message(STATUS "CUDD_INCLUDE_DIRS is ${CUDD_INCLUDE_DIRS}")
message(STATUS "CUDD_LIBRARY_DIRS is ${CUDD_LIBRARY_DIRS}")

# Now define all available custom options
option(DEBUG "Sets whether the DEBUG mode is used" ON)
option(USE_POPCNT "Sets whether the popcnt instruction is going to be used." ON)
option(USE_BOOST_STATIC_LIBRARIES "Sets whether the Boost libraries should be linked statically." ON)
option(LINK_LIBCXXABI "Sets whether libc++abi should be linked" OFF) 
set(ADDITIONAL_INCLUDE_DIRS "" CACHE STRING "Additional directories added to the include directories.")
set(ADDITIONAL_LINK_DIRS "" CACHE STRING "Additional directories added to the link directories.")
set(CUSTOM_BOOST_ROOT "" CACHE STRING "A custom path to the Boost root directory.")

# If the DEBUG option was turned on, we will target a debug version and a release version otherwise
if (DEBUG)
    set (CMAKE_BUILD_TYPE "DEBUG")
    message(STATUS "Building DEBUG version.")
else()
    set (CMAKE_BUILD_TYPE "RELEASE")
    message(STATUS "Building RELEASE version.")
endif()

if(CMAKE_COMPILER_IS_GNUCC)
    # Set standard flags for GCC
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops")
    set (CMAKE_CXX_FLAGS "-std=c++0x -Wall -pedantic")
    # -Werror is atm removed as this gave some problems with existing code
    # May be re-set later
    # (Thomas Heinemann, 2012-12-21)
    
    # Turn on popcnt instruction if desired (yes by default)
    if (USE_POPCNT)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
    endif(USE_POPCNT)
elseif(MSVC)
	# required for GMM to compile, ugly error directive in their code
	add_definitions(/D_SCL_SECURE_NO_DEPRECATE)
else(CLANG)
	# As CLANG is not set as a variable, we need to set it in case we have not matched another compiler.
	set (CLANG ON)
    # Set standard flags for clang
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops -O4")
    set (CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wall -Werror -pedantic -Wno-unused-variable -DBOOST_RESULT_OF_USE_TR1 -DBOOST_NO_DECLTYPE")
    
    # Turn on popcnt instruction if desired (yes by default)
    if (USE_POPCNT)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
    endif(USE_POPCNT)    
endif()

# Add the binary folder to the search path for include files so that we will find storm-config.h
include_directories("${PROJECT_BINARY_DIR}")

# Base path for test files
set(STORM_CPP_TESTS_BASE_PATH ${PROJECT_SOURCE_DIR}/test)
message(STATUS "STORM_CPP_TESTS_BASE_PATH is ${STORM_CPP_TESTS_BASE_PATH}")

# Main Sources
file(GLOB_RECURSE STORM_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE STORM_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Test Sources
# Note that the tests also need the source files, except for the main file
file(GLOB_RECURSE STORM_TEST_HEADERS ${PROJECT_SOURCE_DIR}/test/*.h)
file(GLOB_RECURSE STORM_TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cpp ${PROJECT_SOURCE_DIR}/src/*/*.cpp)

# Main Grouping
source_group(Headers FILES ${STORM_HEADERS})
source_group(Sources FILES ${STORM_SOURCES})

# Test Grouping
source_group(Headers FILES ${STORM_TEST_HEADERS})
source_group(Sources FILES ${STORM_TEST_SOURCES})

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

# Set required external packages
find_package(Threads REQUIRED)
find_package(Doxygen REQUIRED)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

# If a custom boost root directory was specified, we set the corresponding hint for the script to find it.
if(CUSTOM_BOOST_ROOT)
	set(BOOST_ROOT "${CUSTOM_BOOST_ROOT}")
endif(CUSTOM_BOOST_ROOT)
find_package(Boost REQUIRED COMPONENTS program_options)

if(Boost_FOUND)
    if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
        set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
    endif ()
	
    message(STATUS "BOOST_INCLUDE_DIRS is ${Boost_INCLUDE_DIRS}")
    message(STATUS "BOOST_LIBRARY_DIRS is ${Boost_LIBRARY_DIRS}")
	
    include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

# Add Eigen to the included directories
include_directories(${EIGEN3_INCLUDE_DIR})

# Add GMMXX to the included directories
include_directories(${GMMXX_INCLUDE_DIR})

# Add custom additional include or link directories
if (ADDITIONAL_INCLUDE_DIRS)
	message(STATUS "Using additional include directories ${ADDITIONAL_INCLUDE_DIRS}")
	include_directories(${ADDITIONAL_INCLUDE_DIRS})
endif(ADDITIONAL_INCLUDE_DIRS)
if (ADDITIONAL_LINK_DIRS)
	message(STATUS "Using additional link directories ${ADDITIONAL_LINK_DIRS}")
	link_directories(${ADDITIONAL_LINK_DIRS})
endif(ADDITIONAL_LINK_DIRS)

if (CUDD_INCLUDE_DIRS)
    include_directories(${CUDD_INCLUDE_DIRS})
endif(CUDD_INCLUDE_DIRS)
if (CUDD_LIBRARY_DIRS)
    link_directories(${CUDD_LIBRARY_DIRS})
endif(CUDD_LIBRARY_DIRS)

# Add the executables
# Must be created *after* Boost was added because of LINK_DIRECTORIES
add_executable(storm ${STORM_SOURCES} ${STORM_HEADERS})
add_executable(storm-tests ${STORM_TEST_SOURCES} ${STORM_TEST_HEADERS})

# Add target link deps for Boost program options
target_link_libraries(storm ${Boost_LIBRARIES})   
target_link_libraries(storm-tests ${Boost_LIBRARIES})

# Link against libc++abi if requested. May be needed to build on Linux systems using clang.
if (LINK_LIBCXXABI)
	message (STATUS "Linking against libc++abi.")
	target_link_libraries(storm "c++abi")
	target_link_libraries(storm-tests "c++abi")
endif(LINK_LIBCXXABI)

# Add a target to generate API documentation with Doxygen
if(DOXYGEN_FOUND)
    set(CMAKE_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc")
    string(REGEX REPLACE ";" " " CMAKE_DOXYGEN_INPUT_LIST "${PROJECT_SOURCE_DIR}/src")

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

if (GTEST_INCLUDE_DIR)
	# For make-based builds, defines make target named test.
	# For Visual Studio builds, defines Visual Studio project named RUN_TESTS.
	enable_testing()

	include_directories(${GTEST_INCLUDE_DIR})
	target_link_libraries(storm-tests ${GTEST_LIBRARIES})

	add_test(NAME storm-tests COMMAND storm-tests)
	if(MSVC) # VS2012 doesn't support correctly the tuples yet
		add_definitions( /D _VARIADIC_MAX=10 )
	endif()
endif(GTEST_INCLUDE_DIR)

if (LOG4CPLUS_INCLUDE_DIR)
    include_directories(${LOG4CPLUS_INCLUDE_DIR})
    target_link_libraries(storm ${LOG4CPLUS_LIBRARIES}) 
    target_link_libraries(storm-tests ${LOG4CPLUS_LIBRARIES})
    # On Linux, we have to link against librt
    if (UNIX AND NOT APPLE)
	   target_link_libraries(storm rt)
	   target_link_libraries(storm-tests rt)
    endif(UNIX AND NOT APPLE)
endif(LOG4CPLUS_INCLUDE_DIR)

if (CUDD_LIBRARY_DIRS)
    target_link_libraries(storm "-lobj -lcudd -lmtr -lst -lutil -lepd")
    target_link_libraries(storm-tests "-lobj -lcudd -lmtr -lst -lutil -lepd")
endif(CUDD_LIBRARY_DIRS)

if (THREADS_FOUND)
	include_directories(${THREADS_INCLUDE_DIRS})
    target_link_libraries (storm ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries (storm-tests ${CMAKE_THREAD_LIBS_INIT})
endif(THREADS_FOUND)

# Configure a header file to pass some of the CMake settings to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/storm-config.h.in"
  "${PROJECT_BINARY_DIR}/storm-config.h"
)

add_custom_target(memcheck valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm -v --fix-deadlocks ${PROJECT_SOURCE_DIR}/examples/dtmc/crowds/crowds5_5.tra examples/dtmc/crowds/crowds5_5.lab
						DEPENDS storm)
add_custom_target(memcheck-tests valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm-tests -v --fix-deadlocks 
						DEPENDS storm-tests)
						
set (CPPLINT_ARGS --filter=-whitespace/tab,-whitespace/line_length,-legal/copyright,-readability/streams)
add_custom_target(style python cpplint.py ${CPPLINT_ARGS} `find ./src/ -iname "*.h" -or -iname "*.cpp"`)
