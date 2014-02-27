cmake_minimum_required (VERSION 2.8.6)

# Set project name
project (cudaForStorm CXX C)

# Set the version number
set (STORM_CPP_VERSION_MAJOR 1)
set (STORM_CPP_VERSION_MINOR 0)

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

message(STATUS "CUDA_PATH is ${CUDA_PATH} or $ENV{CUDA_PATH}")

#############################################################
##
##	CMake options of StoRM
##
#############################################################
option(DEBUG "Sets whether the DEBUG mode is used" ON)
option(USE_POPCNT "Sets whether the popcnt instruction is going to be used." ON)
option(LINK_LIBCXXABI "Sets whether libc++abi should be linked." OFF)
option(USE_LIBCXX "Sets whether the standard library is libc++." OFF)
option(ENABLE_GLPK "Sets whether StoRM is built with support for glpk." OFF)
set(GUROBI_ROOT "" CACHE STRING "The root directory of Gurobi (if available).")
set(Z3_ROOT "" CACHE STRING "The root directory of Z3 (if available).")
set(ADDITIONAL_INCLUDE_DIRS "" CACHE STRING "Additional directories added to the include directories.")
set(ADDITIONAL_LINK_DIRS "" CACHE STRING "Additional directories added to the link directories.")
set(STORM_LIB_INSTALL_DIR "${PROJECT_SOURCE_DIR}/../../build/cudaForStorm" CACHE STRING "The Build directory of storm, where the library files should be installed to (if available).")

#############################################################
##
##	Inclusion of required libraries
##
#############################################################

# Add the resources/cmake folder to Module Search Path for FindTBB.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/../cmake/")

find_package(CUDA REQUIRED)
find_package(Doxygen REQUIRED)
find_package(Threads REQUIRED)

# If the DEBUG option was turned on, we will target a debug version and a release version otherwise
if (DEBUG)
    set (CMAKE_BUILD_TYPE "DEBUG")
else()
    set (CMAKE_BUILD_TYPE "RELEASE")
endif()
message(STATUS "StoRM - Building ${CMAKE_BUILD_TYPE} version.")

if ("${GUROBI_ROOT}" STREQUAL "")
    set(ENABLE_GUROBI OFF)
else()
    set(ENABLE_GUROBI ON)
endif()

if ("${Z3_ROOT}" STREQUAL "")
    set(ENABLE_Z3 OFF)
else()
    set(ENABLE_Z3 ON)
	set(Z3_LIB_NAME "z3")
endif()

message(STATUS "StoRM - CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "StoRM - CMAKE_BUILD_TYPE (ENV): $ENV{CMAKE_BUILD_TYPE}")

#############################################################
##
##	CUDA Options
##
#############################################################
SET (CUDA_VERBOSE_BUILD ON CACHE BOOL "nvcc verbose" FORCE)
set(CUDA_ATTACH_VS_BUILD_RULE_TO_CUDA_FILE ON)
set(BUILD_SHARED_LIBS OFF)
set(CUDA_SEPARABLE_COMPILATION ON)
#set(CUDA_NVCC_FLAGS "-arch=sm_30")

# Because the FindCUDA.cmake file has a path related bug, two folders have to be present
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/CMakeFiles/cudaLibrary.dir/Debug")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/CMakeFiles/cudaLibrary.dir/Release")


#############################################################
##
##	Compiler specific settings and definitions
##
#############################################################

# Path to the no-strict-aliasing target
set(CONVERSIONHELPER_TARGET "${PROJECT_SOURCE_DIR}/src/utility/ConversionHelper.cpp")

if(CMAKE_COMPILER_IS_GNUCC)
    message(STATUS "StoRM - Using Compiler Configuration: GCC")
    # Set standard flags for GCC
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")
    # -Werror is atm removed as this gave some problems with existing code
    # May be re-set later
    # (Thomas Heinemann, 2012-12-21)
    
    # Turn on popcnt instruction if desired (yes by default)
    if (USE_POPCNT)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
    endif(USE_POPCNT)
	
	# Set the no-strict-aliasing target for GCC
	set_source_files_properties(${CONVERSIONHELPER_TARGET} PROPERTIES COMPILE_FLAGS " -fno-strict-aliasing ")
elseif(MSVC)
    message(STATUS "StoRM - Using Compiler Configuration: MSVC")
	# required for GMM to compile, ugly error directive in their code
	add_definitions(/D_SCL_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS)
	# required as the PRCTL Parser bloats object files (COFF) beyond their maximum size (see http://msdn.microsoft.com/en-us/library/8578y171(v=vs.110).aspx)
	add_definitions(/bigobj)
	# required by GTest and PrismGrammar::createIntegerVariable
	add_definitions(/D_VARIADIC_MAX=10)
	# Windows.h breaks GMM in gmm_except.h because of its macro definition for min and max
	add_definitions(/DNOMINMAX)
	
	if(ENABLE_Z3)
		set(Z3_LIB_NAME "libz3")
	endif()
	
	# MSVC does not do strict-aliasing, so no option needed
else(CLANG)
    message(STATUS "StoRM - Using Compiler Configuration: Clang (LLVM)")
	# As CLANG is not set as a variable, we need to set it in case we have not matched another compiler.
	set (CLANG ON)
    # Set standard flags for clang
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops -O3")
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
    
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=${CLANG_STDLIB} -Wall -pedantic -Wno-unused-variable -DBOOST_RESULT_OF_USE_TR1 -DBOOST_NO_DECLTYPE -ftemplate-depth=1024")
    
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    
    # Turn on popcnt instruction if desired (yes by default)
    if (USE_POPCNT)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
    endif(USE_POPCNT)    
	
	# Set the no-strict-aliasing target for Clang
	set_source_files_properties(${CONVERSIONHELPER_TARGET} PROPERTIES COMPILE_FLAGS " -fno-strict-aliasing ")
endif()

#############################################################
##
##	CMake-generated Config File for StoRM
##
#############################################################
# Base path for test files
set(STORM_CPP_TESTS_BASE_PATH "${PROJECT_SOURCE_DIR}/test")
# Gurobi Defines
if (ENABLE_GUROBI)
	set(STORM_CPP_GUROBI_DEF "define")
else()
	set(STORM_CPP_GUROBI_DEF "undef")
endif()

# glpk defines
if (ENABLE_GLPK)
	set(STORM_CPP_GLPK_DEF "define")
else()
	set(STORM_CPP_GLPK_DEF "undef")
endif()

# Z3 Defines
if (ENABLE_Z3)
	set(STORM_CPP_Z3_DEF "define")
else()
	set(STORM_CPP_Z3_DEF "undef")
endif()

# Intel TBB Defines
if (TBB_FOUND AND ENABLE_INTELTBB)
	set(STORM_CPP_INTELTBB_DEF "define")
else()
	set(STORM_CPP_INTELTBB_DEF "undef")
endif()

# Configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/../../storm-config.h.in"
	"${PROJECT_BINARY_DIR}/include/storm-config.h"
)
# Add the binary dir include directory for storm-config.h
include_directories("${PROJECT_BINARY_DIR}/include")

# Add the main source directory for includes
include_directories("${PROJECT_SOURCE_DIR}/../../src")

#############################################################
##
##	Source file aggregation and clustering
##
#############################################################
file(GLOB_RECURSE CUDAFORSTORM_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE CUDAFORSTORM_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE CUDAFORSTORM_CUDA_SOURCES "${PROJECT_SOURCE_DIR}/srcCuda/*.cu")
file(GLOB_RECURSE CUDAFORSTORM_CUDA_HEADERS "${PROJECT_SOURCE_DIR}/srcCuda/*.h")

# Additional include files like the storm-config.h
file(GLOB_RECURSE STORM_BUILD_HEADERS ${PROJECT_BINARY_DIR}/include/*.h)

# Group the headers and sources
source_group(main FILES ${CUDAFORSTORM_HEADERS} ${CUDAFORSTORM_SOURCES})
source_group(cuda FILES ${CUDAFORSTORM_CUDA_SOURCES} ${CUDAFORSTORM_CUDA_HEADERS})

# Add custom additional include or link directories
if (ADDITIONAL_INCLUDE_DIRS)
	message(STATUS "StoRM - Using additional include directories ${ADDITIONAL_INCLUDE_DIRS}")
	include_directories(${ADDITIONAL_INCLUDE_DIRS})
endif(ADDITIONAL_INCLUDE_DIRS)
if (ADDITIONAL_LINK_DIRS)
	message(STATUS "StoRM - Using additional link directories ${ADDITIONAL_LINK_DIRS}")
	link_directories(${ADDITIONAL_LINK_DIRS})
endif(ADDITIONAL_LINK_DIRS)

#############################################################
##
##	Pre executable-creation link_directories setup
##
#############################################################
if (ENABLE_GUROBI)
	link_directories("${GUROBI_ROOT}/lib")
endif()
if (ENABLE_Z3)
    link_directories("${Z3_ROOT}/bin")
endif()
if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
	set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
endif ()
link_directories(${Boost_LIBRARY_DIRS})
if (TBB_FOUND AND ENABLE_INTELTBB)
	link_directories(${TBB_LIBRARY_DIRS})
endif()

###############################################################################
##                                                                            #
##	Executable Creation                                                       #
##                                                                            #
##  All link_directories() calls MUST be made before this point               #
##                                                                            #
###############################################################################

# Since this will be a library
include (GenerateExportHeader)

add_library(cudaForStorm SHARED ${CUDAFORSTORM_HEADERS} ${CUDAFORSTORM_SOURCES})
GENERATE_EXPORT_HEADER( cudaForStorm
             BASE_NAME cudaForStorm
             EXPORT_MACRO_NAME cudaForStorm_EXPORT
             EXPORT_FILE_NAME include/cudaForStorm_Export.h
             STATIC_DEFINE cudaForStorm_BUILT_AS_STATIC
)

#############################################################
##
##	CUDA
##
#############################################################
#set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} --gpu-architecture sm_30)
cuda_add_library(cudaLibrary
  ${CUDAFORSTORM_CUDA_SOURCES} ${CUDAFORSTORM_CUDA_HEADERS}
  OPTIONS -DSTUFF="" -arch=sm_30
  RELEASE -DNDEBUG
  DEBUG -g -DDEBUG
)
target_link_libraries(cudaLibrary ${CUDA_cusparse_LIBRARY})
ADD_DEPENDENCIES(cudaForStorm cudaLibrary)
target_link_libraries(cudaForStorm cudaLibrary)
message(STATUS "Found CUDA SDK in Version ${CUDA_VERSION_STRING}, sparse lib is ${CUDA_cusparse_LIBRARY}")
include_directories(${CUDA_INCLUDE_DIRS})

#############################################################
##
##	Gurobi (optional)
##
#############################################################
if (ENABLE_GUROBI)
    message (STATUS "StoRM - Linking with Gurobi")
	include_directories("${GUROBI_ROOT}/include")
    target_link_libraries(cudaForStorm "gurobi56")
endif(ENABLE_GUROBI)

#############################################################
##
##	glpk (optional)
##
#############################################################
if (ENABLE_GLPK)
    message (STATUS "StoRM - Linking with glpk")
    target_link_libraries(cudaForStorm "glpk")
endif(ENABLE_GLPK)

#############################################################
##
##	Z3 (optional)
##
#############################################################
if (ENABLE_Z3)
    message (STATUS "StoRM - Linking with Z3")
	include_directories("${Z3_ROOT}/include")
    target_link_libraries(cudaForStorm ${Z3_LIB_NAME})
endif(ENABLE_Z3)

#############################################################
##
##	Threads
##
#############################################################
include_directories(${THREADS_INCLUDE_DIRS})
target_link_libraries(cudaForStorm ${CMAKE_THREAD_LIBS_INIT})

if (MSVC)
	# Add the DebugHelper DLL
	set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} Dbghelp.lib")
	target_link_libraries(cudaForStorm "Dbghelp.lib")   
endif(MSVC)

# Link against libc++abi if requested. May be needed to build on Linux systems using clang.
if (LINK_LIBCXXABI)
	message (STATUS "StoRM - Linking against libc++abi.")
	target_link_libraries(cudaForStorm "c++abi")
endif(LINK_LIBCXXABI)

# Install Directive
install(TARGETS cudaForStorm DESTINATION "${STORM_LIB_INSTALL_DIR}/lib")
install(FILES "${PROJECT_SOURCE_DIR}/src/cudaForStorm.h" "${PROJECT_BINARY_DIR}/cudaForStorm_Export.h" DESTINATION "${STORM_LIB_INSTALL_DIR}/include")