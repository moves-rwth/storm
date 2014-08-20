cmake_minimum_required (VERSION 2.8.6)

# Set project name
project (cudaForStorm CXX C)

# Set the version number
set (STORM_CPP_VERSION_MAJOR 1)
set (STORM_CPP_VERSION_MINOR 0)

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

message(STATUS "StoRM (CudaPlugin) - CUDA_PATH is ${CUDA_PATH} or $ENV{CUDA_PATH}")

#############################################################
##
##	CMake options of StoRM
##
#############################################################
option(CUDAFORSTORM_DEBUG "Sets whether the DEBUG mode is used" ON)
option(LINK_LIBCXXABI "Sets whether libc++abi should be linked." OFF)
option(USE_LIBCXX "Sets whether the standard library is libc++." OFF)

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

# Set the hint for CUSP
set(CUSP_HINT "${PROJECT_SOURCE_DIR}/../3rdparty/cusplibrary")

find_package(CUDA REQUIRED)
find_package(Cusp REQUIRED)
find_package(Doxygen REQUIRED)
find_package(Thrust REQUIRED)

# If the DEBUG option was turned on, we will target a debug version and a release version otherwise
if (CUDAFORSTORM_DEBUG)
    set (CMAKE_BUILD_TYPE "DEBUG")
else()
    set (CMAKE_BUILD_TYPE "RELEASE")
endif()
message(STATUS "StoRM (CudaPlugin) - Building ${CMAKE_BUILD_TYPE} version.")

message(STATUS "StoRM (CudaPlugin) - CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "StoRM (CudaPlugin) - CMAKE_BUILD_TYPE (ENV): $ENV{CMAKE_BUILD_TYPE}")

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
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/CMakeFiles/cudaForStorm.dir/Debug")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/CMakeFiles/cudaForStorm.dir/Release")


#############################################################
##
##	Compiler specific settings and definitions
##
#############################################################
if(CMAKE_COMPILER_IS_GNUCC)
    message(STATUS "StoRM (CudaPlugin) - Using Compiler Configuration: GCC")
    # Set standard flags for GCC
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")
elseif(MSVC)
    message(STATUS "StoRM (CudaPlugin) - Using Compiler Configuration: MSVC")
	# required for GMM to compile, ugly error directive in their code
	add_definitions(/D_SCL_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS)
	# required as the PRCTL Parser bloats object files (COFF) beyond their maximum size (see http://msdn.microsoft.com/en-us/library/8578y171(v=vs.110).aspx)
	add_definitions(/bigobj)
	# required by GTest and PrismGrammar::createIntegerVariable
	add_definitions(/D_VARIADIC_MAX=10)
	# Windows.h breaks GMM in gmm_except.h because of its macro definition for min and max
	add_definitions(/DNOMINMAX)
else(CLANG)
    message(STATUS "StoRM (CudaPlugin) - Using Compiler Configuration: Clang (LLVM)")
	# As CLANG is not set as a variable, we need to set it in case we have not matched another compiler.
	set (CLANG ON)
    # Set standard flags for clang
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops -O3")
    if(UNIX AND NOT APPLE AND NOT USE_LIBCXX)
		set(CLANG_STDLIB libstdc++)
		message(STATUS "StoRM (CudaPlugin) - Linking against libstdc++")
    else()
		set(CLANG_STDLIB libc++)
		message(STATUS "StoRM (CudaPlugin) - Linking against libc++")
		# Set up some Xcode specific settings
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    endif()
    
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=${CLANG_STDLIB} -Wall -pedantic -Wno-unused-variable -DBOOST_RESULT_OF_USE_TR1 -DBOOST_NO_DECLTYPE -ftemplate-depth=1024")
    
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()

#############################################################
##
##	CMake-generated Config File for StoRM
##
#############################################################

# Test for type alignment
try_run(STORM_CUDA_RUN_RESULT_TYPEALIGNMENT STORM_CUDA_COMPILE_RESULT_TYPEALIGNMENT
	${PROJECT_BINARY_DIR} "${PROJECT_SOURCE_DIR}/CMakeAlignmentCheck.cpp"
	COMPILE_OUTPUT_VARIABLE OUTPUT_TEST_VAR
)
if(NOT STORM_CUDA_COMPILE_RESULT_TYPEALIGNMENT)
	message(FATAL_ERROR "StoRM (CudaPlugin) - Could not test type alignment, there was an Error while compiling the file ${PROJECT_SOURCE_DIR}/CMakeAlignmentCheck.cpp: ${OUTPUT_TEST_VAR}")
elseif(STORM_CUDA_RUN_RESULT_TYPEALIGNMENT EQUAL 0)
	message(STATUS "StoRM (CudaPlugin) - Result of Type Alignment Check: OK.")
else()
	message(FATAL_ERROR "StoRM (CudaPlugin) - Result of Type Alignment Check: FAILED (Code ${STORM_CUDA_RUN_RESULT_TYPEALIGNMENT})")
endif()

# Test for Float 64bit Alignment
try_run(STORM_CUDA_RUN_RESULT_FLOATALIGNMENT STORM_CUDA_COMPILE_RESULT_FLOATALIGNMENT
	${PROJECT_BINARY_DIR} "${PROJECT_SOURCE_DIR}/CMakeFloatAlignmentCheck.cpp"
	COMPILE_OUTPUT_VARIABLE OUTPUT_TEST_VAR
)
if(NOT STORM_CUDA_COMPILE_RESULT_FLOATALIGNMENT)
	message(FATAL_ERROR "StoRM (CudaPlugin) - Could not test float type alignment, there was an Error while compiling the file ${PROJECT_SOURCE_DIR}/CMakeFloatAlignmentCheck.cpp: ${OUTPUT_TEST_VAR}")
elseif(STORM_CUDA_RUN_RESULT_FLOATALIGNMENT EQUAL 2)
	message(STATUS "StoRM (CudaPlugin) - Result of Float Type Alignment Check: 64bit alignment active.")
	set(STORM_CUDAPLUGIN_FLOAT_64BIT_ALIGN_DEF "define")
elseif(STORM_CUDA_RUN_RESULT_FLOATALIGNMENT EQUAL 3)
	message(STATUS "StoRM (CudaPlugin) - Result of Float Type Alignment Check: 64bit alignment disabled.")
	set(STORM_CUDAPLUGIN_FLOAT_64BIT_ALIGN_DEF "undef")
else()
	message(FATAL_ERROR "StoRM (CudaPlugin) - Result of Float Type Alignment Check: FAILED (Code ${STORM_CUDA_RUN_RESULT_FLOATALIGNMENT})")
endif()


#
# Make a version file containing the current version from git.
#
include(GetGitRevisionDescription)
git_describe_checkout(STORM_GIT_VERSION_STRING)
# Parse the git Tag into variables
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" STORM_CUDAPLUGIN_VERSION_MAJOR "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" STORM_CUDAPLUGIN_VERSION_MINOR "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" STORM_CUDAPLUGIN_VERSION_PATCH "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-([0-9]+)\\-.*" "\\1" STORM_CUDAPLUGIN_VERSION_COMMITS_AHEAD "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-([a-z0-9]+).*" "\\1" STORM_CUDAPLUGIN_VERSION_HASH "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-[a-z0-9]+\\-(.*)" "\\1" STORM_CUDAPLUGIN_VERSION_APPENDIX "${STORM_GIT_VERSION_STRING}")
if ("${STORM_CUDAPLUGIN_VERSION_APPENDIX}" MATCHES "^.*dirty.*$")
	set(STORM_CUDAPLUGIN_VERSION_DIRTY 1)
else()
	set(STORM_CUDAPLUGIN_VERSION_DIRTY 0)
endif()
message(STATUS "StoRM (CudaPlugin) - Version information: ${STORM_CUDAPLUGIN_VERSION_MAJOR}.${STORM_CUDAPLUGIN_VERSION_MINOR}.${STORM_CUDAPLUGIN_VERSION_PATCH} (${STORM_CUDAPLUGIN_VERSION_COMMITS_AHEAD} commits ahead of Tag) build from ${STORM_CUDAPLUGIN_VERSION_HASH} (Dirty: ${STORM_CUDAPLUGIN_VERSION_DIRTY})")


# Configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/storm-cudaplugin-config.h.in"
	"${PROJECT_BINARY_DIR}/include/storm-cudaplugin-config.h"
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
file(GLOB_RECURSE CUDAFORSTORM_BUILD_HEADERS ${PROJECT_BINARY_DIR}/include/*.h)

# Group the headers and sources
source_group(main FILES ${CUDAFORSTORM_HEADERS} ${CUDAFORSTORM_SOURCES})
source_group(cuda FILES ${CUDAFORSTORM_CUDA_SOURCES} ${CUDAFORSTORM_CUDA_HEADERS})

# Add custom additional include or link directories
if (ADDITIONAL_INCLUDE_DIRS)
	message(STATUS "StoRM (CudaPlugin) - Using additional include directories ${ADDITIONAL_INCLUDE_DIRS}")
	include_directories(${ADDITIONAL_INCLUDE_DIRS})
endif(ADDITIONAL_INCLUDE_DIRS)
if (ADDITIONAL_LINK_DIRS)
	message(STATUS "StoRM (CudaPlugin) - Using additional link directories ${ADDITIONAL_LINK_DIRS}")
	link_directories(${ADDITIONAL_LINK_DIRS})
endif(ADDITIONAL_LINK_DIRS)

#############################################################
##
##	Pre executable-creation link_directories setup
##
#############################################################



#############################################################
##
##	CUDA
##
#############################################################
#set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} --gpu-architecture sm_30)
#target_link_libraries(cudaLibrary ${CUDA_cusparse_LIBRARY})
#ADD_DEPENDENCIES(cudaForStorm cudaLibrary)
#target_link_libraries(cudaForStorm cudaLibrary)
message(STATUS "StoRM (CudaPlugin) - Found CUDA SDK in Version ${CUDA_VERSION_STRING}, sparse lib is ${CUDA_cusparse_LIBRARY}")
include_directories(${CUDA_INCLUDE_DIRS})

#############################################################
##
##	CUSP
##
#############################################################
if(CUSP_FOUND)
	include_directories(${CUSP_INCLUDE_DIR})
	cuda_include_directories(${CUSP_INCLUDE_DIR})
	message(STATUS "StoRM (CudaPlugin) - Found CUSP Version ${CUSP_VERSION} in location ${CUSP_INCLUDE_DIR}")
else()
	message(FATAL_ERROR "StoRM (CudaPlugin) - Could not find CUSP!")
endif()

#############################################################
##
##	Thrust
##
#############################################################
if(THRUST_FOUND)
	include_directories(${THRUST_INCLUDE_DIR})
	cuda_include_directories(${THRUST_INCLUDE_DIR})
	message(STATUS "StoRM (CudaPlugin) - Found Thrust Version ${THRUST_VERSION} in location ${THRUST_INCLUDE_DIR}")
else()
	message(FATAL_ERROR "StoRM (CudaPlugin) - Could not find Thrust! Check your CUDA installation.")
endif()

###############################################################################
##                                                                            #
##	Executable Creation                                                       #
##                                                                            #
##  All link_directories() calls AND include_directories() calls              #
##  MUST be made before this point                                            #
##                                                                            #
###############################################################################
include (GenerateExportHeader)

cuda_add_library(cudaForStorm SHARED
  ${CUDAFORSTORM_CUDA_SOURCES} ${CUDAFORSTORM_CUDA_HEADERS}
  OPTIONS -DSTUFF="" -arch=sm_30
  RELEASE -DNDEBUG
  DEBUG -g -DDEBUG
)
GENERATE_EXPORT_HEADER( cudaForStorm
             BASE_NAME cudaForStorm
             EXPORT_MACRO_NAME cudaForStorm_EXPORT
             EXPORT_FILE_NAME include/cudaForStorm_Export.h
             STATIC_DEFINE cudaForStorm_BUILT_AS_STATIC
)

if (MSVC)
	# Add the DebugHelper DLL
	set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} Dbghelp.lib")
	target_link_libraries(cudaForStorm "Dbghelp.lib")   
endif(MSVC)

# Link against libc++abi if requested. May be needed to build on Linux systems using clang.
if (LINK_LIBCXXABI)
	message (STATUS "StoRM (CudaPlugin) - Linking against libc++abi.")
	target_link_libraries(cudaForStorm "c++abi")
endif(LINK_LIBCXXABI)

# Install Directive
install(TARGETS cudaForStorm DESTINATION "${STORM_LIB_INSTALL_DIR}/lib")
install(FILES "${PROJECT_SOURCE_DIR}/srcCuda/cudaForStorm.h" "${PROJECT_BINARY_DIR}/include/cudaForStorm_Export.h" DESTINATION "${STORM_LIB_INSTALL_DIR}/include")