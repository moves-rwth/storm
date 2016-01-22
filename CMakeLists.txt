cmake_minimum_required (VERSION 2.8.6)
cmake_policy(VERSION 3.2)
# Set project name
project (storm CXX C)

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")


# Add the resources/cmake folder to Module Search Path for FindTBB.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/resources/cmake/")


#############################################################
##
##	CMake options of StoRM
##
#############################################################
option(STORM_DEBUG "Sets whether the DEBUG mode is used" ON)
option(STORM_USE_POPCNT "Sets whether the popcnt instruction is going to be used." ON)
option(USE_BOOST_STATIC_LIBRARIES "Sets whether the Boost libraries should be linked statically." ON)
option(STORM_USE_INTELTBB "Sets whether the Intel TBB libraries should be used." OFF)
option(STORM_USE_COTIRE "Sets whether Cotire should be used (for building precompiled headers)." OFF)
option(LINK_LIBCXXABI "Sets whether libc++abi should be linked." OFF)
option(USE_LIBCXX "Sets whether the standard library is libc++." OFF)
option(USE_CARL "Sets whether carl should be included." ON)
option(FORCE_COLOR "Force color output" OFF)
option(STORM_PYTHON "Builds the API for Python" OFF)
option(STORM_COMPILE_WITH_CCACHE "Compile using CCache" ON)
set(GUROBI_ROOT "" CACHE STRING "A hint to the root directory of Gurobi (optional).")
set(Z3_ROOT "" CACHE STRING "A hint to the root directory of Z3 (optional).")
set(CUDA_ROOT "" CACHE STRING "The root directory of CUDA.")
set(MSAT_ROOT "" CACHE STRING "The root directory of MathSAT (if available).")
set(ADDITIONAL_INCLUDE_DIRS "" CACHE STRING "Additional directories added to the include directories.")
set(ADDITIONAL_LINK_DIRS "" CACHE STRING "Additional directories added to the link directories.")
set(CUSTOM_BOOST_ROOT "" CACHE STRING "A custom path to the Boost root directory.")

# If the DEBUG option was turned on, we will target a debug version and a release version otherwise.
if (STORM_DEBUG)
    set (CMAKE_BUILD_TYPE "DEBUG")
else()
    set (CMAKE_BUILD_TYPE "RELEASE")
endif()
message(STATUS "StoRM - Building ${CMAKE_BUILD_TYPE} version.")
message(STATUS "StoRM - CMAKE_BUILD_TYPE (ENV): $ENV{CMAKE_BUILD_TYPE}")

if(STORM_COMPILE_WITH_CCACHE)
	find_program(CCACHE_FOUND ccache)
	if(CCACHE_FOUND)
		message(STATUS "SToRM - Using ccache")
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
	else()
		message(STATUS "Could not find ccache")
	endif()
endif()

# Base path for test files
set(STORM_CPP_TESTS_BASE_PATH "${PROJECT_SOURCE_DIR}/test")

set(STORMPY_OUTPUT_DIR "${PROJECT_BINARY_DIR}/stormpy")
set(STORMPY_SOURCE_DIR "${PROJECT_SOURCE_DIR}/stormpy")

# Auto-detect operating system.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# Mac OS
	set(OPERATING_SYSTEM "Mac OS")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux
	set(OPERATING_SYSTEM "Linux")
else()
    # Assuming Windows.
    set(OPERATING_SYSTEM "Windows")
ENDIF()
message(STATUS "Detected operating system: ${OPERATING_SYSTEM}")


#############################################################
##
##	Compiler specific settings and definitions
##
#############################################################
# Path to the no-strict-aliasing target
set(CONVERSIONHELPER_TARGET "${PROJECT_SOURCE_DIR}/src/utility/ConversionHelper.cpp")

if(CMAKE_COMPILER_IS_GNUCC)
    set(STORM_COMPILED_BY "GCC")
    # Set standard flags for GCC
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -funroll-loops")
    add_definitions(-DBOOST_RESULT_OF_USE_DECLTYPE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -Wno-deprecated-declarations -Wno-unused-local-typedefs -Wno-unknown-pragmas")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wno-deprecated-declarations")

    # Turn on popcnt instruction if desired (yes by default)
    if (STORM_USE_POPCNT)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
    endif(STORM_USE_POPCNT)

	# Set the no-strict-aliasing target for GCC
	set_source_files_properties(${CONVERSIONHELPER_TARGET} PROPERTIES COMPILE_FLAGS " -fno-strict-aliasing")
elseif(MSVC)
    set(STORM_COMPILED_BY "MSVC")
	# required for GMM to compile, ugly error directive in their code
	add_definitions(/D_SCL_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS)
	# required as the PRCTL Parser bloats object files (COFF) beyond their maximum size (see http://msdn.microsoft.com/en-us/library/8578y171(v=vs.110).aspx)
	add_definitions(/bigobj)
	# required by GTest and PrismGrammar::createIntegerVariable
	add_definitions(/D_VARIADIC_MAX=10)
	# Windows.h breaks GMM in gmm_except.h because of its macro definition for min and max
	add_definitions(/DNOMINMAX)
	# Boost Defs, required for using boost's transform iterator
	add_definitions(/DBOOST_RESULT_OF_USE_DECLTYPE)

	# since nobody cares at the moment
	add_definitions(/wd4250)

	# MSVC does not do strict-aliasing, so no option needed
else(CLANG)
    set(STORM_COMPILED_BY "Clang (LLVM)")
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

    add_definitions(-DBOOST_RESULT_OF_USE_DECLTYPE)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=${CLANG_STDLIB} -Wall -pedantic -Wno-newline-eof -Wno-mismatched-tags -Wno-unused-local-typedefs -ftemplate-depth=1024")

    if(FORCE_COLOR)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    endif()

    # Turn on popcnt instruction if desired (yes by default)
    if (STORM_USE_POPCNT)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
    endif(STORM_USE_POPCNT)

	# Set the no-strict-aliasing target for Clang
	set_source_files_properties(${CONVERSIONHELPER_TARGET} PROPERTIES COMPILE_FLAGS " -fno-strict-aliasing ")
endif()

if(CCACHE_FOUND)
	set(STORM_COMPILED_BY  "${STORM_COMPILED_BY} (ccache)")
endif()

message(STATUS "StoRM - Using Compiler Configuration: ${STORM_COMPILED_BY}")

#############################################################
##
##	Inclusion of required libraries
##
#############################################################


# Add the version of Eigen3 in the repository to the include pathes
set(EIGEN3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/eigen")
include_directories(${EIGEN3_INCLUDE_DIR})

# Add the version of GMM in the repository to the include pathes
set(GMMXX_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/gmm-5.0/include")
include_directories(${GMMXX_INCLUDE_DIR})

find_package(GMP)


#############################################################
##
##	Boost
##
#############################################################

# Boost Option variables
set(Boost_USE_STATIC_LIBS        ${USE_BOOST_STATIC_LIBRARIES})
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

# If a custom boost root directory was specified, we set the corresponding hint for the script to find it.
if(CUSTOM_BOOST_ROOT)
	message(STATUS "StoRM - Using Boost from CUSTOM_BOOST_ROOT located at ${CUSTOM_BOOST_ROOT}")
	set(BOOST_ROOT "${CUSTOM_BOOST_ROOT}")
endif(CUSTOM_BOOST_ROOT)

if(STORM_PYTHON)
find_package(Boost REQUIRED COMPONENTS python3)
else()
find_package(Boost REQUIRED)
endif()

if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
	set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
endif ()
link_directories(${Boost_LIBRARY_DIRS})


include_directories(${Boost_INCLUDE_DIRS})
list(APPEND STORM_LINK_LIBRARIES ${Boost_LIBRARIES})
#message(STATUS "BOOST_INCLUDE_DIRS is ${Boost_INCLUDE_DIRS}")
#message(STATUS "BOOST_LIBRARY_DIRS is ${Boost_LIBRARY_DIRS}")



#############################################################
##
##	ExprTk
##
#############################################################
message (STATUS "StoRM - Including ExprTk")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/exprtk")

#############################################################
##
##	Z3 (optional)
##
#############################################################

find_package(Z3 QUIET)

# Z3 Defines
set(STORM_HAVE_Z3 ${Z3_FOUND})

if(STORM_HAVE_Z3)
    message (STATUS "StoRM - Linking with Z3")
    include_directories(${Z3_INCLUDE_DIRS})
    list(APPEND STORM_LINK_LIBRARIES ${Z3_LIBRARIES})
endif(STORM_HAVE_Z3)




#############################################################
##
##	glpk
##
#############################################################

set(STORM_HAVE_GLPK 1)
message (STATUS "StoRM - Linking with glpk")
add_subdirectory("${PROJECT_SOURCE_DIR}/resources/3rdparty/glpk-4.53")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/glpk-4.53/src")
list(APPEND STORM_LINK_LIBRARIES "glpk")



#############################################################
##
##	Gurobi (optional)
##
#############################################################

find_package(Gurobi)
if ("${GUROBI_ROOT}" STREQUAL "" AND NOT GUROBI_FOUND)
    set(ENABLE_GUROBI OFF)
else()
    set(ENABLE_GUROBI ON)
endif()

# Gurobi Defines
set(STORM_HAVE_GUROBI ${ENABLE_GUROBI})

if (ENABLE_GUROBI)
	if (NOT GUROBI_FOUND)
		message(FATAL_ERROR "Gurobi was requested, but not found!")
        endif()
        message (STATUS "StoRM - Linking with Gurobi (include: ${GUROBI_INCLUDE_DIRS})")
        include_directories(${GUROBI_INCLUDE_DIRS})
        list(APPEND STORM_LINK_LIBRARIES ${GUROBI_LIBRARY})
	#link_directories("${GUROBI_ROOT}/lib")
endif()

#############################################################
##
##	CUDA Library generation
##
#############################################################


if ("${CUDA_ROOT}" STREQUAL "")
    set(ENABLE_CUDA OFF)
else()
    set(ENABLE_CUDA ON)
endif()

# CUDA Defines
if (ENABLE_CUDA)
	set(STORM_CPP_CUDA_DEF "define")
else()
	set(STORM_CPP_CUDA_DEF "undef")
endif()


# CUDA Defines
set(STORM_CPP_CUDAFORSTORM_DEF "undef")


if(ENABLE_CUDA)

	# Test for type alignment
	try_run(STORM_CUDA_RUN_RESULT_TYPEALIGNMENT STORM_CUDA_COMPILE_RESULT_TYPEALIGNMENT
		${PROJECT_BINARY_DIR} "${PROJECT_SOURCE_DIR}/cuda/CMakeAlignmentCheck.cpp"
		COMPILE_OUTPUT_VARIABLE OUTPUT_TEST_VAR
	)
	if(NOT STORM_CUDA_COMPILE_RESULT_TYPEALIGNMENT)
		message(FATAL_ERROR "StoRM (CudaPlugin) - Could not test type alignment, there was an Error while compiling the file ${PROJECT_SOURCE_DIR}/cuda/CMakeAlignmentCheck.cpp: ${OUTPUT_TEST_VAR}")
	elseif(STORM_CUDA_RUN_RESULT_TYPEALIGNMENT EQUAL 0)
		message(STATUS "StoRM (CudaPlugin) - Result of Type Alignment Check: OK.")
	else()
		message(FATAL_ERROR "StoRM (CudaPlugin) - Result of Type Alignment Check: FAILED (Code ${STORM_CUDA_RUN_RESULT_TYPEALIGNMENT})")
	endif()

	# Test for Float 64bit Alignment
	try_run(STORM_CUDA_RUN_RESULT_FLOATALIGNMENT STORM_CUDA_COMPILE_RESULT_FLOATALIGNMENT
		${PROJECT_BINARY_DIR} "${PROJECT_SOURCE_DIR}/cuda/CMakeFloatAlignmentCheck.cpp"
		COMPILE_OUTPUT_VARIABLE OUTPUT_TEST_VAR
	)
	if(NOT STORM_CUDA_COMPILE_RESULT_FLOATALIGNMENT)
		message(FATAL_ERROR "StoRM (CudaPlugin) - Could not test float type alignment, there was an Error while compiling the file ${PROJECT_SOURCE_DIR}/cuda/CMakeFloatAlignmentCheck.cpp: ${OUTPUT_TEST_VAR}")
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
		"${PROJECT_SOURCE_DIR}/cuda/storm-cudaplugin-config.h.in"
		"${PROJECT_BINARY_DIR}/include/storm-cudaplugin-config.h"
	)

	#create library
	find_package(CUDA REQUIRED)
	set(CUSP_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/resources/3rdparty/cusplibrary")
	find_package(Cusp REQUIRED)
	find_package(Thrust REQUIRED)

	set(STORM_CUDA_LIB_NAME "storm-cuda")

	file(GLOB_RECURSE STORM_CUDA_KERNEL_FILES ${PROJECT_SOURCE_DIR}/cuda/kernels/*.cu)
	file(GLOB_RECURSE STORM_CUDA_HEADER_FILES ${PROJECT_SOURCE_DIR}/cuda/kernels/*.h)

	source_group(kernels FILES ${STORM_CUDA_KERNEL_FILES} ${STORM_CUDA_HEADER_FILES})
	include_directories(${PROJECT_SOURCE_DIR}/cuda/kernels/)

	#set(CUDA_PROPAGATE_HOST_FLAGS OFF)
	set(CUDA_NVCC_FLAGS "-arch=sm_30")

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

	include_directories(${CUDA_INCLUDE_DIRS})
	include_directories(${ADDITIONAL_INCLUDE_DIRS})

	cuda_add_library(${STORM_CUDA_LIB_NAME}
	    ${STORM_CUDA_KERNEL_FILES} ${STORM_CUDA_HEADER_FILES}
	)

        message (STATUS "StoRM - Linking with CUDA")
        list(APPEND STORM_LINK_LIBRARIES ${STORM_CUDA_LIB_NAME})
        include_directories("${PROJECT_SOURCE_DIR}/cuda/kernels/")
endif()


if(GMP_FOUND)
	link_directories(${GMP_LIBRARY_DIR})
elseif(MPIR_FOUND)
	link_directories(${GMP_MPIR_LIBRARY_DIR} ${GMP_MPIRXX_LIBRARY_DIR})
endif(GMP_FOUND)



#############################################################
##
##	CUDD
##
#############################################################
add_subdirectory("${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/src/cudd")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/src/epd")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/src/mtr")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/src/nanotrav")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/src/obj")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/src/st")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/cudd-2.5.0/src/util")
list(APPEND STORM_LINK_LIBRARIES cudd)





#############################################################
##
##      carl
##
#############################################################

if(USE_CARL)
    find_package(carl QUIET)
    if(carl_FOUND)
     set(STORM_HAVE_CARL ON)
    endif()

    #find_package(smtrat QUIET)
    if(smtrat_FOUND)
        set(STORM_HAVE_SMTRAT ON)
    endif()
endif()

if(STORM_HAVE_CARL)
    message(STATUS "StoRM - Linking with carl.")
	message("${carl_INCLUDE_DIR}")
    include_directories("${carl_INCLUDE_DIR}")
    list(APPEND STORM_LINK_LIBRARIES ${carl_LIBRARIES})
endif()

#############################################################
##
##      SMT-RAT
##
#############################################################

if(STORM_HAVE_SMTRAT)
message(STATUS "StoRM - Linking with smtrat.")
    include_directories("${smtrat_INCLUDE_DIR}")
    list(APPEND STORM_LINK_LIBRARIES ${smtrat_LIBRARIES})
endif()

#############################################################
##
##	MathSAT (optional)
##
#############################################################



if ("${MSAT_ROOT}" STREQUAL "")
    set(ENABLE_MSAT OFF)
else()
    set(ENABLE_MSAT ON)
endif()

# MathSAT Defines
set(STORM_HAVE_MSAT ${ENABLE_MSAT})
if (ENABLE_MSAT)
    link_directories("${MSAT_ROOT}/lib")
    message (STATUS "StoRM - Linking with MathSAT")
    include_directories("${MSAT_ROOT}/include")
    list(APPEND STORM_LINK_LIBRARIES "mathsat")
    if(GMP_FOUND)
        include_directories("${GMP_INCLUDE_DIR}")
        list(APPEND STORM_LINK_LIBRARIES "gmp")
    elseif(MPIR_FOUND)
        include_directories("${GMP_INCLUDE_DIR}")
        list(APPEND STORM_LINK_LIBRARIES "mpir" "mpirxx")
    else(GMP_FOUND)
        message(FATAL_ERROR "GMP is required for MathSAT, but was not found!")
    endif(GMP_FOUND)
endif(ENABLE_MSAT)

#############################################################
##
##	Xerces
##
#############################################################

find_package(Xerces)
if(NOT XERCES_FOUND)
	message("Use shipped version of xerces")
	set(XERCES_ROOT ${CMAKE_BINARY_DIR}/resources/3rdparty/xercesc-3.1.2)
	set(XERCESC_INCLUDE ${XERCES_ROOT}/include)
	set(XERCES_LIBRARY_PATH ${XERCES_ROOT}/lib)
	set(XERCESC_LIBRARIES ${XERCES_LIBRARY_PATH}/libxerces-c.a)
endif()

set(STORM_HAVE_XERCES TRUE)
if(STORM_HAVE_XERCES)
	include_directories(${XERCESC_INCLUDE})
	list(APPEND STORM_LINK_LIBRARIES ${XERCESC_LIBRARIES})
endif()

#############################################################
##
##	Sylvan
##
#############################################################
include(ExternalProject)
set(STORM_SYLVAN_ROOT "${PROJECT_SOURCE_DIR}/resources/3rdparty/sylvan")
ExternalProject_Add(
    sylvan
    DOWNLOAD_COMMAND ""
    PREFIX "sylvan"
    SOURCE_DIR "${STORM_SYLVAN_ROOT}"
    CMAKE_ARGS -DSYLVAN_BUILD_TEST=Off -DSYLVAN_BUILD_EXAMPLES=Off -DCMAKE_BUILD_TYPE=Release
    BINARY_DIR "${PROJECT_BINARY_DIR}/sylvan"
    INSTALL_COMMAND ""
    INSTALL_DIR "${PROJECT_BINARY_DIR}/sylvan"
)
ExternalProject_Get_Property(sylvan binary_dir)
set(Sylvan_INCLUDE_DIR "${STORM_SYLVAN_ROOT}/src")
message(STATUS "Linking with shipped version of sylvan (in directory ${STORM_SYLVAN_ROOT}).")
include_directories("${Sylvan_INCLUDE_DIR}")
list(APPEND STORM_LINK_LIBRARIES "${binary_dir}/src/libsylvan.a")
if(${OPERATING_SYSTEM} MATCHES "Linux")
	find_package(Hwloc QUIET)
	if(NOT Hwloc_FOUND)
		message(SEND_ERROR "HWLOC is required but was not found.")
	else()
		list(APPEND STORM_LINK_LIBRARIES ${Hwloc_LIBRARIES})	
	endif()
endif()

#############################################################
##
##	Google Test gtest
##
#############################################################

set(gtest_force_shared_crt ON)
add_subdirectory("${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.7.0")

#############################################################
##
##	Log4CPlus
##
#############################################################
set(BUILD_SHARED_LIBS OFF CACHE BOOL "If TRUE, log4cplus is built as a shared library, otherwise as a static library")
set(LOG4CPLUS_BUILD_LOGGINGSERVER OFF)
set(LOG4CPLUS_BUILD_TESTING OFF)
set(LOG4CPLUS_USE_UNICODE OFF)
set(LOG4CPLUS_DEFINE_INSTALL_TARGET OFF)
add_subdirectory("${PROJECT_SOURCE_DIR}/resources/3rdparty/log4cplus-1.1.3-rc1")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/log4cplus-1.1.3-rc1/include")
include_directories("${PROJECT_BINARY_DIR}/resources/3rdparty/log4cplus-1.1.3-rc1/include") # This adds the defines.hxx file

list(APPEND STORM_LINK_LIBRARIES log4cplusS)
if (UNIX AND NOT APPLE)
    list(APPEND STORM_LINK_LIBRARIES rt)
endif(UNIX AND NOT APPLE)

#############################################################
##
##	Intel Threading Building Blocks (optional)
##
#############################################################

set(TBB_INSTALL_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/tbb42_20140122_merged-win-lin-mac")


find_package(TBB)
if (TBB_FOUND AND STORM_USE_INTELTBB)
	link_directories(${TBB_LIBRARY_DIRS})
	set(STORM_CPP_INTELTBB_DEF "define")
else()
	set(STORM_CPP_INTELTBB_DEF "undef")
endif()


if (TBB_FOUND)
	message(STATUS "StoRM - Found Intel TBB with interface version ${TBB_INTERFACE_VERSION}.")
	if (STORM_USE_INTELTBB)
		message(STATUS "StoRM - Linking with Intel TBB in ${TBB_LIBRARY_DIRS}.")
		include_directories(${TBB_INCLUDE_DIRS})
		target_link_libraries(storm tbb tbbmalloc)
	endif(STORM_USE_INTELTBB)
endif(TBB_FOUND)

#############################################################
##
##	Threads
##
#############################################################

find_package(Threads REQUIRED)
include_directories(${THREADS_INCLUDE_DIRS})
list(APPEND STORM_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
if (STORM_USE_COTIRE)
	target_link_libraries(storm_unity ${CMAKE_THREAD_LIBS_INIT})
endif(STORM_USE_COTIRE)

if (MSVC)
	# Add the DebugHelper DLL
	set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} Dbghelp.lib")
	target_link_libraries(storm "Dbghelp.lib")
endif(MSVC)


#############################################################
##
##	Cotire
##
#############################################################
message (STATUS "StoRM - Using Cotire: ${STORM_USE_COTIRE}")

if (STORM_USE_COTIRE)
	# Include Cotire for PCH Generation
	set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/resources/cmake")
	include(cotire)

	cotire(storm)
	target_link_libraries(storm_unity ${Boost_LIBRARIES})
	#cotire(storm-functional-tests)
	#cotire(storm-performance-tests)
endif()

#############################################################
##
##	libc++abi
##
#############################################################
# Link against libc++abi if requested. May be needed to build on Linux systems using clang.
if (LINK_LIBCXXABI)
	message (STATUS "StoRM - Linking against libc++abi.")
	target_link_libraries(storm "c++abi")
endif(LINK_LIBCXXABI)


#############################################################
##
##	Doxygen
##
#############################################################

find_package(Doxygen REQUIRED)
# Add a target to generate API documentation with Doxygen
if(DOXYGEN_FOUND)
    set(CMAKE_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc")
    string(REGEX REPLACE ";" " " CMAKE_DOXYGEN_INPUT_LIST "${PROJECT_SOURCE_DIR}/src")

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

    add_custom_target(doc ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)
#############################################################
##
##	Python
##
#############################################################


if(STORM_PYTHON)
    set(Python_ADDITIONAL_VERSIONS 3.4)
    FIND_PACKAGE(PythonInterp REQUIRED)
    FIND_PACKAGE(PythonLibs 3.4 REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIR})
endif()

#############################################################
##
##	CMake-generated Config File for StoRM
##
#############################################################

#
# Make a version file containing the current version from git.
#
include(GetGitRevisionDescription)
git_describe_checkout(STORM_GIT_VERSION_STRING)
message(STATUS "STORM_GIT_VERSION_STRING: ${STORM_GIT_VERSION_STRING}")
# Parse the git Tag into variables
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" STORM_CPP_VERSION_MAJOR "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" STORM_CPP_VERSION_MINOR "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" STORM_CPP_VERSION_PATCH "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-([0-9]+)\\-.*" "\\1" STORM_CPP_VERSION_COMMITS_AHEAD "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-([a-z0-9]+).*" "\\1" STORM_CPP_VERSION_HASH "${STORM_GIT_VERSION_STRING}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+\\-[0-9]+\\-[a-z0-9]+\\-(.*)" "\\1" STORM_CPP_VERSION_APPENDIX "${STORM_GIT_VERSION_STRING}")
if ("${STORM_CPP_VERSION_APPENDIX}" MATCHES "^.*dirty.*$")
	set(STORM_CPP_VERSION_DIRTY 1)
else()
	set(STORM_CPP_VERSION_DIRTY 0)
endif()
message(STATUS "StoRM - Version information: ${STORM_CPP_VERSION_MAJOR}.${STORM_CPP_VERSION_MINOR}.${STORM_CPP_VERSION_PATCH} (${STORM_CPP_VERSION_COMMITS_AHEAD} commits ahead of Tag) build from ${STORM_CPP_VERSION_HASH} (Dirty: ${STORM_CPP_VERSION_DIRTY})")

# Configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/storm-config.h.in"
	"${PROJECT_BINARY_DIR}/include/storm-config.h"
)

# Configure a header file to pass the storm version to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/storm-version.cpp.in"
	"${PROJECT_BINARY_DIR}/src/utility/storm-version.cpp"
)

set(STORM_GENERATED_SOURCES "${PROJECT_BINARY_DIR}/src/utility/storm-version.cpp")

# Add the binary dir include directory for storm-config.h
include_directories("${PROJECT_BINARY_DIR}/include")

add_subdirectory(resources/3rdparty)
add_subdirectory(src)
add_subdirectory(test)



#############################################################
##
##	memcheck targets
##
#############################################################
add_custom_target(memcheck valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm -v --fix-deadlocks ${PROJECT_SOURCE_DIR}/examples/dtmc/crowds/crowds5_5.tra examples/dtmc/crowds/crowds5_5.lab DEPENDS storm)
add_custom_target(memcheck-functional-tests valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm-functional-tests -v --fix-deadlocks	DEPENDS storm-functional-tests)
add_custom_target(memcheck-performance-tests valgrind --leak-check=full --show-reachable=yes ${PROJECT_BINARY_DIR}/storm-performance-tests -v --fix-deadlocks DEPENDS storm-performance-tests)

set(CPPLINT_ARGS --filter=-whitespace/tab,-whitespace/line_length,-legal/copyright,-readability/streams)
add_custom_target(style python cpplint.py ${CPPLINT_ARGS} `find ./src/ -iname "*.h" -or -iname "*.cpp" `)


include(StormCPackConfig.cmake)
