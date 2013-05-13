cmake_minimum_required (VERSION 2.8.6)

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
	set (GTEST_LIBRARIES ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY}) # as we dont use FindGTest anymore
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
option(USE_INTELTBB "Sets whether the Intel TBB Extensions should be used." OFF)
option(STORM_USE_COTIRE "Sets whether Cotire should be used (for building precompiled headers)." OFF)
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

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_BUILD_TYPE (ENV): $ENV{CMAKE_BUILD_TYPE}")

if(CMAKE_COMPILER_IS_GNUCC)
    message(STATUS "Using GCC")
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
    message(STATUS "Using MSVC")
	# required for GMM to compile, ugly error directive in their code
	add_definitions(/D_SCL_SECURE_NO_DEPRECATE)
else(CLANG)
    message(STATUS "Using CLANG")
	# As CLANG is not set as a variable, we need to set it in case we have not matched another compiler.
	set (CLANG ON)
    # Set standard flags for clang
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops -O4")
    set (CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wall -Werror -pedantic -Wno-unused-variable -DBOOST_RESULT_OF_USE_TR1 -DBOOST_NO_DECLTYPE")
    
    set (CMAKE_CXX_FLAGS_DEBUG "-g")
    
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
file(GLOB_RECURSE STORM_SOURCES_WITHOUT_MAIN ${PROJECT_SOURCE_DIR}/src/*/*.cpp)
file(GLOB_RECURSE STORM_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm.cpp)
set(STORM_SOURCES "${STORM_SOURCES_WITHOUT_MAIN};${STORM_MAIN_FILE}")
file(GLOB_RECURSE STORM_ADAPTERS_FILES ${PROJECT_SOURCE_DIR}/src/adapters/*.h ${PROJECT_SOURCE_DIR}/src/adapters/*.cpp)
file(GLOB_RECURSE STORM_EXCEPTIONS_FILES ${PROJECT_SOURCE_DIR}/src/exceptions/*.h ${PROJECT_SOURCE_DIR}/src/exceptions/*.cpp)
file(GLOB_RECURSE STORM_FORMULA_FILES ${PROJECT_SOURCE_DIR}/src/formula/*.h ${PROJECT_SOURCE_DIR}/src/formula/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/*.cpp)
file(GLOB_RECURSE STORM_MODELS_FILES ${PROJECT_SOURCE_DIR}/src/models/*.h ${PROJECT_SOURCE_DIR}/src/models/*.cpp)
file(GLOB_RECURSE STORM_PARSER_FILES ${PROJECT_SOURCE_DIR}/src/parser/*.h ${PROJECT_SOURCE_DIR}/src/parser/*.cpp)
file(GLOB_RECURSE STORM_STORAGE_FILES ${PROJECT_SOURCE_DIR}/src/storage/*.h ${PROJECT_SOURCE_DIR}/src/storage/*.cpp)
file(GLOB_RECURSE STORM_UTILITY_FILES ${PROJECT_SOURCE_DIR}/src/utility/*.h ${PROJECT_SOURCE_DIR}/src/utility/*.cpp)
file(GLOB_RECURSE STORM_IR_FILES ${PROJECT_SOURCE_DIR}/src/ir/*.h ${PROJECT_SOURCE_DIR}/src/ir/*.cpp)

# Test Sources
# Note that the tests also need the source files, except for the main file
file(GLOB_RECURSE STORM_FUNCTIONAL_TEST_FILES ${PROJECT_SOURCE_DIR}/test/functional/*.h ${PROJECT_SOURCE_DIR}/test/functional/*.cpp)
file(GLOB_RECURSE STORM_PERFORMANCE_TEST_FILES ${PROJECT_SOURCE_DIR}/test/performance/*.h ${PROJECT_SOURCE_DIR}/test/performance/*.cpp)

# Group the headers and sources
source_group(main FILES ${STORM_MAIN_FILE})
source_group(adapters FILES ${STORM_ADAPTERS_FILES})
source_group(exceptions FILES ${STORM_EXCEPTIONS_FILES})
source_group(formula FILES ${STORM_FORMULA_FILES})
source_group(modelchecker FILES ${STORM_MODELCHECKER_FILES})
source_group(models FILES ${STORM_MODELS_FILES})
source_group(parser FILES ${STORM_PARSER_FILES})
source_group(storage FILES ${STORM_STORAGE_FILES})
source_group(utility FILES ${STORM_UTILITY_FILES})
source_group(ir FILES ${STORM_IR_FILES})
source_group(functional-test FILES ${STORM_FUNCTIONAL_TEST_FILES})
source_group(performance-test FILES ${STORM_PERFORMANCE_TEST_FILES})

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

# Preset pathes for Intel TBB to internal copy in /resources/3rdParty/tbb41_20130314_mergedWindowsLinuxMacOSX
set(TBB_INSTALL_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/tbb41_20130314_merged-win-lin-mac/")
if(MSVC)
	set(ENV{TBB_ARCH_PLATFORM} "intel64/vc11")
elseif(CMAKE_COMPILER_IS_GNUCC)
	set(ENV{TBB_ARCH_PLATFORM} "intel64/gcc4.4")
else(CLANG)
	set(ENV{TBB_ARCH_PLATFORM} "intel64/clang3.2")
endif()

# Add the resources/cmake folder to Module Search Path for FindTBB.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/resources/cmake/")

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
find_package(TBB)

if(Boost_FOUND)
    if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
        set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
    endif ()
	
    message(STATUS "BOOST_INCLUDE_DIRS is ${Boost_INCLUDE_DIRS}")
    message(STATUS "BOOST_LIBRARY_DIRS is ${Boost_LIBRARY_DIRS}")
	
    include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

# Intels Thread Building Blocks
if (TBB_FOUND)
	message(STATUS "Found TBB with Interface Version ${TBB_INTERFACE_VERSION}")
	
	if(USE_INTELTBB)
		add_definitions(-DGMM_USE_TBB)
	endif()
	
	include_directories(${TBB_INCLUDE_DIRS})
	link_directories(${TBB_LIBRARY_DIRS})
endif(TBB_FOUND)

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
add_executable(storm-functional-tests ${STORM_FUNCTIONAL_TEST_FILES} ${STORM_SOURCES_WITHOUT_MAIN} ${STORM_HEADERS})
add_executable(storm-performance-tests ${STORM_PERFORMANCE_TEST_FILES} ${STORM_SOURCES_WITHOUT_MAIN} ${STORM_HEADERS})

# Add target link deps for Boost program options
target_link_libraries(storm ${Boost_LIBRARIES})   
target_link_libraries(storm-functional-tests ${Boost_LIBRARIES})
target_link_libraries(storm-performance-tests ${Boost_LIBRARIES})

if (USE_INTELTBB)
	target_link_libraries(storm tbb tbbmalloc)
	target_link_libraries(storm-functional-tests tbb tbbmalloc)
	target_link_libraries(storm-performance-tests tbb tbbmalloc)
endif(USE_INTELTBB)

if (APPLE)
	set(STORM_USE_COTIRE OFF)
	
    # Set up some Xcode specific settings
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif(APPLE)

# Print Cotire Usage Status
message (STATUS "Using Cotire: ${STORM_USE_COTIRE}")

if (STORM_USE_COTIRE)
	# Include Cotire for PCH Generation
	set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/resources/cmake")
	include(cotire)

	cotire(storm)
	target_link_libraries(storm_unity ${Boost_LIBRARIES})
	#cotire(storm-functional-tests)
	#cotire(storm-performance-tests)
endif()
	
# Link against libc++abi if requested. May be needed to build on Linux systems using clang.
if (LINK_LIBCXXABI)
	message (STATUS "Linking against libc++abi.")
	target_link_libraries(storm "c++abi")
	target_link_libraries(storm-functional-tests "c++abi")
	target_link_libraries(storm-performance-tests "c++abi")
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
	target_link_libraries(storm-functional-tests ${GTEST_LIBRARIES})
	target_link_libraries(storm-performance-tests ${GTEST_LIBRARIES})

	add_test(NAME storm-functional-tests COMMAND storm-functional-tests)
	add_test(NAME storm-performance-tests COMMAND storm-performance-tests)
	if(MSVC) # VS2012 doesn't support correctly the tuples yet
		add_definitions( /D _VARIADIC_MAX=10 )
	endif()
endif(GTEST_INCLUDE_DIR)

if (LOG4CPLUS_INCLUDE_DIR)
    include_directories(${LOG4CPLUS_INCLUDE_DIR})
    target_link_libraries(storm ${LOG4CPLUS_LIBRARIES}) 
	if (STORM_USE_COTIRE)
		target_link_libraries(storm_unity ${LOG4CPLUS_LIBRARIES})
	endif(STORM_USE_COTIRE)
    target_link_libraries(storm-functional-tests ${LOG4CPLUS_LIBRARIES})
    target_link_libraries(storm-performance-tests ${LOG4CPLUS_LIBRARIES})
    # On Linux, we have to link against librt
    if (UNIX AND NOT APPLE)
		target_link_libraries(storm rt)
		if (STORM_USE_COTIRE)
			target_link_libraries(storm_unity rt)
		endif(STORM_USE_COTIRE)
		target_link_libraries(storm-functional-tests rt)
		target_link_libraries(storm-performance-tests rt)
    endif(UNIX AND NOT APPLE)
endif(LOG4CPLUS_INCLUDE_DIR)

if (CUDD_LIBRARY_DIRS)
    target_link_libraries(storm "-lobj -lcudd -lmtr -lst -lutil -lepd")
    target_link_libraries(storm-functional-tests "-lobj -lcudd -lmtr -lst -lutil -lepd")
    target_link_libraries(storm-performance-tests "-lobj -lcudd -lmtr -lst -lutil -lepd")
endif(CUDD_LIBRARY_DIRS)

if (THREADS_FOUND)
	include_directories(${THREADS_INCLUDE_DIRS})
	target_link_libraries(storm ${CMAKE_THREAD_LIBS_INIT})
	if (STORM_USE_COTIRE)
		target_link_libraries(storm_unity ${CMAKE_THREAD_LIBS_INIT})
	endif(STORM_USE_COTIRE)
	target_link_libraries(storm-functional-tests ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(storm-performance-tests ${CMAKE_THREAD_LIBS_INIT})
endif(THREADS_FOUND)

# Configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/storm-config.h.in"
	"${PROJECT_BINARY_DIR}/storm-config.h"
)

add_custom_target(memcheck valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm -v --fix-deadlocks ${PROJECT_SOURCE_DIR}/examples/dtmc/crowds/crowds5_5.tra examples/dtmc/crowds/crowds5_5.lab DEPENDS storm)
add_custom_target(memcheck-functional-tests valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm-functional-tests -v --fix-deadlocks	DEPENDS storm-functional-tests)
add_custom_target(memcheck-performance-tests valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm-performance-tests -v --fix-deadlocks DEPENDS storm-performance-tests)

set (CPPLINT_ARGS --filter=-whitespace/tab,-whitespace/line_length,-legal/copyright,-readability/streams)
add_custom_target(style python cpplint.py ${CPPLINT_ARGS} `find ./src/ -iname "*.h" -or -iname "*.cpp"`)
