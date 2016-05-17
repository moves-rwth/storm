cmake_minimum_required (VERSION 2.8.6)
cmake_policy(VERSION 3.2)
# Set project name
project (storm CXX C)

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")


# Add the resources/cmake folder to Module Search Path for FindTBB.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/resources/cmake/")


include(ExternalProject)
#############################################################
##
##	CMake options of StoRM
##
#############################################################
option(STORM_DEBUG "Sets whether the DEBUG mode is used" ON)
option(STORM_USE_POPCNT "Sets whether the popcnt instruction is going to be used." ON)
option(USE_BOOST_STATIC_LIBRARIES "Sets whether the Boost libraries should be linked statically." ON)
option(STORM_USE_INTELTBB "Sets whether the Intel TBB libraries should be used." OFF)
option(STORM_USE_GUROBI "Sets whether Gurobi should be used." OFF)
option(STORM_USE_COTIRE "Sets whether Cotire should be used (for building precompiled headers)." OFF)
option(LINK_LIBCXXABI "Sets whether libc++abi should be linked." OFF)
option(USE_LIBCXX "Sets whether the standard library is libc++." OFF)
option(USE_CARL "Sets whether carl should be included." ON)
option(USE_XERCES "Sets whether xerces should be used." OFF)
option(FORCE_COLOR "Force color output" OFF)
option(STORM_PYTHON "Builds the API for Python" OFF)
option(STORM_COMPILE_WITH_CCACHE "Compile using CCache" ON)
option(STORM_LOG_DISABLE_DEBUG "Disable log and trace message support" OFF)
set(BOOST_ROOT "" CACHE STRING "A hint to the root directory of Boost (optional).")
set(GUROBI_ROOT "" CACHE STRING "A hint to the root directory of Gurobi (optional).")
set(Z3_ROOT "" CACHE STRING "A hint to the root directory of Z3 (optional).")
set(CUDA_ROOT "" CACHE STRING "The hint to the root directory of CUDA (optional).")
set(MSAT_ROOT "" CACHE STRING "The hint to the root directory of MathSAT (optional).")
set(ADDITIONAL_INCLUDE_DIRS "" CACHE STRING "Additional directories added to the include directories.")
set(ADDITIONAL_LINK_DIRS "" CACHE STRING "Additional directories added to the link directories.")

# If the DEBUG option was turned on, we will target a debug version and a release version otherwise.
if (STORM_DEBUG)
    set (CMAKE_BUILD_TYPE "DEBUG")
else()
    set (CMAKE_BUILD_TYPE "RELEASE")
endif()
message(STATUS "StoRM - Building ${CMAKE_BUILD_TYPE} version.")

if(STORM_COMPILE_WITH_CCACHE)
	find_program(CCACHE_FOUND ccache)
	if(CCACHE_FOUND)
		message(STATUS "StoRM - Using ccache")
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
set(MACOSX 0)
set(LINUX 0)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# Mac OS
	set(OPERATING_SYSTEM "Mac OS")
        set(MACOSX 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux
	set(OPERATING_SYSTEM "Linux")
        set(LINUX 1)
elseif(WIN32)
    # Assuming Windows.
    set(OPERATING_SYSTEM "Windows")
else()
    message(WARNING "We are unsure about your operating system.")
    set(OPERATING_SYSTEM "Linux")
    set(LINUX 1)
ENDIF()
message(STATUS "Operating system: ${OPERATING_SYSTEM}")


set(DYNAMIC_EXT ".so")
set(STATIC_EXT ".a")
if(MACOSX)
	set(DYNAMIC_EXT ".dylib")
	set(STATIC_EXT ".a")
elseif (WIN32)
	set(DYNAMIC_EXT ".dll")
	set(STATIC_EXT ".lib")
endif()
message(STATUS "Assuming extension for shared libraries: ${DYNAMIC_EXT}")
message(STATUS "Assuming extension for static libraries: ${STATIC_EXT}")


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
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=${CLANG_STDLIB} -Wall -pedantic -Wno-newline-eof -Wno-mismatched-tags -Wno-unused-local-typedefs -ftemplate-depth=1024 -Wno-parentheses-equality")

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
#############################################################
##
##	Inclusion of required libraries
##
#############################################################
#############################################################

#############################################################
##
##	Include the targets for non-system resources
##
#############################################################

# In 3rdparty, targets are being defined that can be used
# in the the system does not have a library
add_subdirectory(resources/3rdparty)

#############################################################
##
##	gmm
##
#############################################################

# Add the shipped version of GMM to the include pathes
set(GMMXX_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/gmm-5.0/include")
include_directories(${GMMXX_INCLUDE_DIR})

#############################################################
##
##	gmp
##
#############################################################

# GMP is optional (unless MathSAT is used, see below)
find_package(GMP QUIET)

#############################################################
##
##	Boost
##
#############################################################

# Boost Option variables
set(Boost_USE_STATIC_LIBS        ${USE_BOOST_STATIC_LIBRARIES})
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.56.0 QUIET REQUIRED)

if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
    set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
endif ()
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${Boost_INCLUDE_DIRS})
list(APPEND STORM_LINK_LIBRARIES ${Boost_LIBRARIES})
message(STATUS "StoRM - Using Boost ${Boost_VERSION} (lib ${Boost_LIB_VERSION})")
#message(STATUS "StoRM - BOOST_INCLUDE_DIRS is ${Boost_INCLUDE_DIRS}")
#message(STATUS "StoRM - BOOST_LIBRARY_DIRS is ${Boost_LIBRARY_DIRS}")

#############################################################
##
##	ExprTk
##
#############################################################

# Use the shipped version of ExprTK
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

if(Z3_FOUND)
    message (STATUS "StoRM - Linking with Z3")
    include_directories(${Z3_INCLUDE_DIRS})
    list(APPEND STORM_LINK_LIBRARIES ${Z3_LIBRARIES})
endif(Z3_FOUND)

#############################################################
##
##	glpk
##
#############################################################

find_package(GLPK QUIET)
if(GLPK_FOUND)
	message (STATUS "StoRM - Using system version of GLPK")
else()
	message (STATUS "StoRM - Using shipped version of GLPK")
	set(GLPK_LIBRARIES  ${CMAKE_BINARY_DIR}/resources/3rdparty/glpk-4.57/lib/libglpk${DYNAMIC_EXT})
	set(GLPK_INCLUDE_DIR ${CMAKE_BINARY_DIR}/resources/3rdparty/glpk-4.57/include)
	set(GLPK_VERSION_STRING 4.57)
	add_dependencies(resources glpk)
endif()

# Since there is a shipped version, always use GLPK
set(STORM_HAVE_GLPK ON)
message (STATUS "StoRM - Linking with glpk ${GLPK_VERSION_STRING}")
include_directories(${GLPK_INCLUDE_DIR})
list(APPEND STORM_LINK_LIBRARIES ${GLPK_LIBRARIES})

#############################################################
##
##	Gurobi (optional)
##
#############################################################

if (STORM_USE_GUROBI)
	find_package(Gurobi QUIET REQUIRED)
	set(STORM_HAVE_GUROBI ${GUROBI_FOUND})
	if (GUROBI_FOUND)
		message (STATUS "StoRM - Linking with Gurobi")
		include_directories(${GUROBI_INCLUDE_DIRS})
		list(APPEND STORM_LINK_LIBRARIES ${GUROBI_LIBRARY})
		#link_directories("${GUROBI_ROOT}/lib")
	else()
		#message(FATAL_ERROR "StoRM - Gurobi was requested, but not found!")
	endif()
else()
	set(STORM_HAVE_GUROBI OFF)
endif()

#############################################################
##
##	CUDD
##
#############################################################

# Do not use system CUDD, StoRM has a modified version
set(CUDD_INCLUDE_DIR ${CMAKE_BINARY_DIR}/resources/3rdparty/cudd-3.0.0/include)
set(CUDD_SHARED_LIBRARY ${CMAKE_BINARY_DIR}/resources/3rdparty/cudd-3.0.0/lib/libcudd${DYNAMIC_EXT})
set(CUDD_STATIC_LIBRARY ${CMAKE_BINARY_DIR}/resources/3rdparty/cudd-3.0.0/lib/libcudd${STATIC_EXT})
set(CUDD_VERSION_STRING 3.0.0)
list(APPEND STORM_LINK_LIBRARIES ${CUDD_SHARED_LIBRARY})
add_dependencies(resources cudd3)

message(STATUS "StoRM - Linking with CUDD ${CUDD_VERSION_STRING}")
#message("StoRM - CUDD include dir: ${CUDD_INCLUDE_DIR}")
include_directories(${CUDD_INCLUDE_DIR})

#############################################################
##
##      CLN
##
#############################################################

find_package(CLN QUIET)

if(CLN_FOUND)
    set(STORM_HAVE_CLN ON)
    message(STATUS "StoRM - Linking with CLN ${CLN_VERSION_STRING}")
    include_directories("${CLN_INCLUDE_DIR}")
    list(APPEND STORM_LINK_LIBRARIES ${CLN_LIBRARIES})
else()
    set(STORM_HAVE_CLN OFF)
    if(NOT GMP_FOUND)
        message(FATAL_ERROR "StoRM - Neither CLN nor GMP found")
    endif()
endif()

#############################################################
##
##      carl
##
#############################################################

set(STORM_HAVE_CARL OFF)
if(USE_CARL)
	find_package(carl QUIET REQUIRED)
	if(carl_FOUND)
		set(STORM_HAVE_CARL ON)
		message(STATUS "StoRM - Linking with carl ${carl_VERSION_STRING}")
		include_directories("${carl_INCLUDE_DIR}")
		list(APPEND STORM_LINK_LIBRARIES ${carl_LIBRARIES})
	else()
		message(FATAL_ERROR "StoRM - CARL was requested but not found")
	endif()
endif()

#############################################################
##
##      SMT-RAT
##
#############################################################

# No find routine yet
#find_package(smtrat QUIET)
# Not yet supported
set(smtrat_FOUND OFF)
set(STORM_HAVE_SMTRAT OFF)
if(smtrat_FOUND)
	set(STORM_HAVE_SMTRAT ON)
	message(STATUS "StoRM - Linking with smtrat.")
    include_directories("${smtrat_INCLUDE_DIR}")
    list(APPEND STORM_LINK_LIBRARIES ${smtrat_LIBRARIES})
endif()

#############################################################
##
##      GiNaC
##
#############################################################

find_package(GiNaC QUIET)

if(GINAC_FOUND)
    set(STORM_HAVE_GINAC ON)
    message(STATUS "StoRM - Linking with GiNaC ${GINAC_VERSION_STRING}")
    # Right now only link with GiNaC for carl
    #include_directories("${GINAC_INCLUDE_DIR}")
    list(APPEND STORM_LINK_LIBRARIES ${GINAC_LIBRARIES})
else()
    set(STORM_HAVE_GINAC OFF)
    #TODO: Check if CARL actually requires the use of GiNaC
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
    message (STATUS "StoRM - Linking with MathSAT")
    link_directories("${MSAT_ROOT}/lib")
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

if(USE_XERCES)
	find_package(Xerces QUIET REQUIRED)
	if(XERCES_FOUND)
		message(STATUS "StoRM - Use system version of xerces")
    else()
		message(STATUS "StoRM - Use shipped version of xerces")
		set(XERCES_ROOT ${CMAKE_BINARY_DIR}/resources/3rdparty/xercesc-3.1.2)
		set(XERCESC_INCLUDE ${XERCES_ROOT}/include)
		set(XERCES_LIBRARY_PATH ${XERCES_ROOT}/lib)
		set(XERCESC_LIBRARIES ${XERCES_LIBRARY_PATH}/libxerces-c.a)

		add_dependencies(resources xercesc)
    endif()

	message (STATUS "StoRM - Linking with xercesc")
    set(STORM_HAVE_XERCES ON)
	include_directories(${XERCESC_INCLUDE})
	list(APPEND STORM_LINK_LIBRARIES ${XERCESC_LIBRARIES})
endif(USE_XERCES)

#############################################################
##
##	Sylvan
##
#############################################################

message(STATUS "StoRM - Using shipped version of sylvan")
message(STATUS "StoRM - Linking with sylvan")
include_directories("${Sylvan_INCLUDE_DIR}")
list(APPEND STORM_LINK_LIBRARIES ${Sylvan_LIBRARY})
add_dependencies(resources sylvan)

if(${OPERATING_SYSTEM} MATCHES "Linux")
	find_package(Hwloc QUIET REQUIRED)
	if(Hwloc_FOUND)
		message(STATUS "StoRM - Linking with hwloc ${Hwloc_VERSION}")
		list(APPEND STORM_LINK_LIBRARIES ${Hwloc_LIBRARIES})
	else()
		message(FATAL_ERROR "HWLOC is required but was not found.")
	endif()
endif()

#############################################################
##
##	Google Test gtest
##
#############################################################

add_dependencies(test-resources googletest)
list(APPEND STORM_TEST_LINK_LIBRARIES ${GTEST_LIBRARIES})

#############################################################
##
##	Intel Threading Building Blocks (optional)
##
#############################################################

set(STORM_HAVE_INTELTBB OFF)
if (STORM_USE_INTELTBB)
	# Point to shipped TBB directory
	set(TBB_INSTALL_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/tbb42_20140122_merged-win-lin-mac")
	find_package(TBB QUIET REQUIRED)

	if (TBB_FOUND)
		message(STATUS "StoRM - Found Intel TBB with interface version ${TBB_INTERFACE_VERSION}.")
		message(STATUS "StoRM - Linking with Intel TBB in ${TBB_LIBRARY_DIRS}.")
		set(STORM_HAVE_INTELTBB ON)
		link_directories(${TBB_LIBRARY_DIRS})
		include_directories(${TBB_INCLUDE_DIRS})
		list(APPEND STORM_LINK_LIBRARIES tbb tbbmalloc)	
	else(TBB_FOUND)
		message(FATAL_ERROR "StoRM - TBB was requested, but not found!")
	endif(TBB_FOUND)
endif(STORM_USE_INTELTBB)

#############################################################
##
##	Threads
##
#############################################################

find_package(Threads QUIET REQUIRED)
if (NOT Threads_FOUND)
	message(FATAL_ERROR "StoRM - Threads was requested, but not found!")
endif()
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
	"${PROJECT_SOURCE_DIR}/src/utility/storm-version.cpp"
)

set(STORM_GENERATED_SOURCES "${PROJECT_BINARY_DIR}/src/utility/storm-version.cpp")

# Add the binary dir include directory for storm-config.h
include_directories("${PROJECT_BINARY_DIR}/include")

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
