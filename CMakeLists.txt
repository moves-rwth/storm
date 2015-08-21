cmake_minimum_required (VERSION 2.8.6)

# Set project name
project (storm CXX C)

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

# Add the version of Eigen3 in the repository to the include pathes
set(EIGEN3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/eigen")
include_directories(${EIGEN3_INCLUDE_DIR})

# Add the version of GMM in the repository to the include pathes
set(GMMXX_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/gmm-5.0/include")
include_directories(${GMMXX_INCLUDE_DIR})

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
set(GUROBI_ROOT "" CACHE STRING "The root directory of Gurobi (if available).")
set(CUDA_ROOT "" CACHE STRING "The root directory of CUDA.")
set(MSAT_ROOT "" CACHE STRING "The root directory of MathSAT (if available).")
set(ADDITIONAL_INCLUDE_DIRS "" CACHE STRING "Additional directories added to the include directories.")
set(ADDITIONAL_LINK_DIRS "" CACHE STRING "Additional directories added to the link directories.")
set(CUSTOM_BOOST_ROOT "" CACHE STRING "A custom path to the Boost root directory.")
#############################################################
##
##	Inclusion of required libraries
##
#############################################################
# Add the resources/cmake folder to Module Search Path for FindTBB.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/resources/cmake/")

# Boost Option variables
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

# If a custom boost root directory was specified, we set the corresponding hint for the script to find it.
if(CUSTOM_BOOST_ROOT)
	message(STATUS "StoRM - Using Boost from CUSTOM_BOOST_ROOT located at ${CUSTOM_BOOST_ROOT}")
	set(BOOST_ROOT "${CUSTOM_BOOST_ROOT}")
endif(CUSTOM_BOOST_ROOT)

set(TBB_INSTALL_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/tbb42_20140122_merged-win-lin-mac")

find_package(Boost REQUIRED)
find_package(Doxygen REQUIRED)
find_package(Gurobi)
find_package(TBB)
find_package(Threads REQUIRED)
find_package(GMP)
find_package(Z3 QUIET)

# If the DEBUG option was turned on, we will target a debug version and a release version otherwise.
if (STORM_DEBUG)
    set (CMAKE_BUILD_TYPE "DEBUG")
else()
    set (CMAKE_BUILD_TYPE "RELEASE")
endif()
message(STATUS "StoRM - Building ${CMAKE_BUILD_TYPE} version.")

if ("${GUROBI_ROOT}" STREQUAL "" AND NOT GUROBI_FOUND)
    set(ENABLE_GUROBI OFF)
else()
    set(ENABLE_GUROBI ON)
endif()

if ("${CUDA_ROOT}" STREQUAL "")
    set(ENABLE_CUDA OFF)
else()
    set(ENABLE_CUDA ON)
endif()


if ("${MSAT_ROOT}" STREQUAL "")
    set(ENABLE_MSAT OFF)
else()
    set(ENABLE_MSAT ON)
endif()

find_package(carl QUIET)
if(carl_FOUND)
    set(STORM_HAVE_CARL ON)
endif()

find_package(smtrat QUIET)
if(smtrat_FOUND)
    set(STORM_HAVE_SMTRAT ON)
endif()

message(STATUS "StoRM - CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "StoRM - CMAKE_BUILD_TYPE (ENV): $ENV{CMAKE_BUILD_TYPE}")

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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -Wno-deprecated-declarations -Wno-unused-local-typedefs")
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
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops -O4")
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
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=${CLANG_STDLIB} -Wall -pedantic -Wno-newline-eof -Wno-mismatched-tags -ftemplate-depth=1024")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")

    # Turn on popcnt instruction if desired (yes by default)
    if (STORM_USE_POPCNT)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
    endif(STORM_USE_POPCNT)    
	
	# Set the no-strict-aliasing target for Clang
	set_source_files_properties(${CONVERSIONHELPER_TARGET} PROPERTIES COMPILE_FLAGS " -fno-strict-aliasing ")
endif()

message(STATUS "StoRM - Using Compiler Configuration: ${STORM_COMPILED_BY}")

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

# Base path for test files
set(STORM_CPP_TESTS_BASE_PATH "${PROJECT_SOURCE_DIR}/test")
# Gurobi Defines
set(STORM_HAVE_GUROBI ${ENABLE_GUROBI})

# CUDA Defines
if (ENABLE_CUDA)
	set(STORM_CPP_CUDA_DEF "define")
else()
	set(STORM_CPP_CUDA_DEF "undef")
endif()

# glpk defines
set(STORM_HAVE_GLPK 1)

# CUDA Defines
set(STORM_CPP_CUDAFORSTORM_DEF "undef")

# Z3 Defines
set(STORM_HAVE_Z3 ${Z3_FOUND})

# MathSAT Defines
set(STORM_HAVE_MSAT ${ENABLE_MSAT})

# Intel TBB Defines
if (TBB_FOUND AND STORM_USE_INTELTBB)
	set(STORM_CPP_INTELTBB_DEF "define")
else()
	set(STORM_CPP_INTELTBB_DEF "undef")
endif()

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

# Add the binary dir include directory for storm-config.h
include_directories("${PROJECT_BINARY_DIR}/include")

#############################################################
##
##	CUDA Library generation
##
#############################################################

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
endif()

#############################################################
##
##	Source file aggregation and clustering
##
#############################################################
file(GLOB_RECURSE STORM_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE STORM_SOURCES_WITHOUT_MAIN ${PROJECT_SOURCE_DIR}/src/*/*.cpp)
file(GLOB_RECURSE STORM_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm.cpp)
set(STORM_SOURCES "${STORM_SOURCES_WITHOUT_MAIN};${STORM_MAIN_FILE};")
file(GLOB_RECURSE STORM_ADAPTERS_FILES ${PROJECT_SOURCE_DIR}/src/adapters/*.h ${PROJECT_SOURCE_DIR}/src/adapters/*.cpp)
file(GLOB_RECURSE STORM_BUILDER_FILES ${PROJECT_SOURCE_DIR}/src/builder/*.h ${PROJECT_SOURCE_DIR}/src/builder/*.cpp)
file(GLOB_RECURSE STORM_EXCEPTIONS_FILES ${PROJECT_SOURCE_DIR}/src/exceptions/*.h ${PROJECT_SOURCE_DIR}/src/exceptions/*.cpp)
file(GLOB_RECURSE STORM_LOGIC_FILES ${PROJECT_SOURCE_DIR}/src/logic/*.h ${PROJECT_SOURCE_DIR}/src/logic/*.cpp)
file(GLOB STORM_MODELCHECKER_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_PRCTL_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/prctl/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/prctl/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_CSL_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/csl/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/csl/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_REACHABILITY_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/reachability/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/reachability/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_PROPOSITIONAL_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/propositional/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/propositional/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_RESULTS_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/results/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/results/*.cpp)
file(GLOB_RECURSE STORM_COUNTEREXAMPLES_FILES ${PROJECT_SOURCE_DIR}/src/counterexamples/*.h ${PROJECT_SOURCE_DIR}/src/counterexamples/*.cpp)
file(GLOB_RECURSE STORM_PERMISSIVESCHEDULER_FILES ${PROJECT_SOURCE_DIR}/src/permissivesched/*.h ${PROJECT_SOURCE_DIR}/src/permissivesched/*.cpp)
file(GLOB STORM_MODELS_FILES ${PROJECT_SOURCE_DIR}/src/models/*.h ${PROJECT_SOURCE_DIR}/src/models/*.cpp)
file(GLOB_RECURSE STORM_MODELS_SPARSE_FILES ${PROJECT_SOURCE_DIR}/src/models/sparse/*.h ${PROJECT_SOURCE_DIR}/src/models/sparse/*.cpp)
file(GLOB_RECURSE STORM_MODELS_SYMBOLIC_FILES ${PROJECT_SOURCE_DIR}/src/models/symbolic/*.h ${PROJECT_SOURCE_DIR}/src/models/symbolic/*.cpp)
file(GLOB STORM_PARSER_FILES ${PROJECT_SOURCE_DIR}/src/parser/*.h ${PROJECT_SOURCE_DIR}/src/parser/*.cpp)
file(GLOB_RECURSE STORM_PARSER_PRISMPARSER_FILES ${PROJECT_SOURCE_DIR}/src/parser/prismparser/*.h ${PROJECT_SOURCE_DIR}/src/parser/prismparser/*.cpp)
file(GLOB STORM_SETTINGS_FILES ${PROJECT_SOURCE_DIR}/src/settings/*.h ${PROJECT_SOURCE_DIR}/src/settings/*.cpp)
file(GLOB STORM_SETTINGS_MODULES_FILES ${PROJECT_SOURCE_DIR}/src/settings/modules/*.h ${PROJECT_SOURCE_DIR}/src/settings/modules/*.cpp)
file(GLOB_RECURSE STORM_SOLVER_FILES ${PROJECT_SOURCE_DIR}/src/solver/*.h ${PROJECT_SOURCE_DIR}/src/solver/*.cpp)
file(GLOB STORM_STORAGE_FILES ${PROJECT_SOURCE_DIR}/src/storage/*.h ${PROJECT_SOURCE_DIR}/src/storage/*.cpp)
file(GLOB_RECURSE STORM_STORAGE_DD_FILES ${PROJECT_SOURCE_DIR}/src/storage/dd/*.h ${PROJECT_SOURCE_DIR}/src/storage/dd/*.cpp)
file(GLOB_RECURSE STORM_STORAGE_EXPRESSIONS_FILES ${PROJECT_SOURCE_DIR}/src/storage/expressions/*.h ${PROJECT_SOURCE_DIR}/src/storage/expressions/*.cpp)
file(GLOB_RECURSE STORM_STORAGE_PRISM_FILES ${PROJECT_SOURCE_DIR}/src/storage/prism/*.h ${PROJECT_SOURCE_DIR}/src/storage/prism/*.cpp)
file(GLOB_RECURSE STORM_STORAGE_SPARSE_FILES ${PROJECT_SOURCE_DIR}/src/storage/sparse/*.h ${PROJECT_SOURCE_DIR}/src/storage/sparse/*.cpp)
file(GLOB_RECURSE STORM_UTILITY_FILES ${PROJECT_SOURCE_DIR}/src/utility/*.h ${PROJECT_SOURCE_DIR}/src/utility/*.cpp)
# Test Sources
# Note that the tests also need the source files, except for the main file
file(GLOB STORM_FUNCTIONAL_TEST_MAIN_FILE ${STORM_CPP_TESTS_BASE_PATH}/functional/storm-functional-tests.cpp)
file(GLOB_RECURSE STORM_FUNCTIONAL_TEST_FILES ${STORM_CPP_TESTS_BASE_PATH}/functional/*.h ${STORM_CPP_TESTS_BASE_PATH}/functional/*.cpp)
file(GLOB STORM_PERFORMANCE_TEST_MAIN_FILE ${STORM_CPP_TESTS_BASE_PATH}/performance/storm-performance-tests.cpp)
file(GLOB_RECURSE STORM_PERFORMANCE_TEST_FILES ${STORM_CPP_TESTS_BASE_PATH}/performance/*.h ${STORM_CPP_TESTS_BASE_PATH}/performance/*.cpp)

# Additional include files like the storm-config.h
file(GLOB_RECURSE STORM_BUILD_HEADERS ${PROJECT_BINARY_DIR}/include/*.h)

# Group the headers and sources
source_group(main FILES ${STORM_MAIN_FILE})
source_group(adapters FILES ${STORM_ADAPTERS_FILES})
source_group(builder FILES ${STORM_BUILDER_FILES})
source_group(exceptions FILES ${STORM_EXCEPTIONS_FILES})
source_group(logic FILES ${STORM_LOGIC_FILES})
source_group(generated FILES ${STORM_BUILD_HEADERS} ${STORM_BUILD_SOURCES})
source_group(modelchecker FILES ${STORM_MODELCHECKER_FILES})
source_group(modelchecker\\prctl FILES ${STORM_MODELCHECKER_PRCTL_FILES})
source_group(modelchecker\\csl FILES ${STORM_MODELCHECKER_CSL_FILES})
source_group(modelchecker\\reachability FILES ${STORM_MODELCHECKER_REACHABILITY_FILES})
source_group(modelchecker\\propositional FILES ${STORM_MODELCHECKER_PROPOSITIONAL_FILES})
source_group(modelchecker\\results FILES ${STORM_MODELCHECKER_RESULTS_FILES})
source_group(counterexamples FILES ${STORM_COUNTEREXAMPLES_FILES})
source_group(permissiveschedulers FILES ${STORM_PERMISSIVESCHEDULER_FILES})
source_group(models FILES ${STORM_MODELS_FILES})
source_group(models\\sparse FILES ${STORM_MODELS_SPARSE_FILES})
source_group(models\\symbolic FILES ${STORM_MODELS_SYMBOLIC_FILES})
source_group(parser FILES ${STORM_PARSER_FILES})
source_group(parser\\prismparser FILES ${STORM_PARSER_PRISMPARSER_FILES})
source_group(settings FILES ${STORM_SETTINGS_FILES})
source_group(settings\\modules FILES ${STORM_SETTINGS_MODULES_FILES})
source_group(solver FILES ${STORM_SOLVER_FILES})
source_group(storage FILES ${STORM_STORAGE_FILES})
source_group(storage\\dd FILES ${STORM_STORAGE_DD_FILES})
source_group(storage\\expressions FILES ${STORM_STORAGE_EXPRESSIONS_FILES})
source_group(storage\\prism FILES ${STORM_STORAGE_PRISM_FILES})
source_group(storage\\sparse FILES ${STORM_STORAGE_SPARSE_FILES})
source_group(utility FILES ${STORM_UTILITY_FILES})
source_group(functional-test FILES ${STORM_FUNCTIONAL_TEST_FILES})
source_group(performance-test FILES ${STORM_PERFORMANCE_TEST_FILES} ${})

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
	if (NOT GUROBI_FOUND)
		message(FATAL_ERROR "Gurobi was requested, but not found!")
	endif()
	#link_directories("${GUROBI_ROOT}/lib")
endif()
if (ENABLE_MSAT)
    link_directories("${MSAT_ROOT}/lib")
endif()
if(GMP_FOUND)
	link_directories(${GMP_LIBRARY_DIR})
elseif(MPIR_FOUND)
	link_directories(${GMP_MPIR_LIBRARY_DIR} ${GMP_MPIRXX_LIBRARY_DIR})
else(GMP_FOUND)
    if (ENABLE_MSAT)
        message(FATAL_ERROR "GMP is required for MathSAT, but was not found!")
    endif(ENABLE_MSAT)
endif(GMP_FOUND)

if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
	set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
endif ()
link_directories(${Boost_LIBRARY_DIRS})
if (TBB_FOUND AND STORM_USE_INTELTBB)
	link_directories(${TBB_LIBRARY_DIRS})
endif()

###############################################################################
##                                                                            #
##	Executable Creation                                                       #
##                                                                            #
##  All link_directories() calls MUST be made before this point               #
##                                                                            #
###############################################################################
add_library(storm ${STORM_SOURCES_WITHOUT_MAIN} ${STORM_HEADERS})
add_executable(storm-main ${STORM_MAIN_FILE})
target_link_libraries(storm-main storm)
set_target_properties(storm-main PROPERTIES OUTPUT_NAME "storm")
add_executable(storm-functional-tests ${STORM_FUNCTIONAL_TEST_MAIN_FILE} ${STORM_FUNCTIONAL_TEST_FILES})
target_link_libraries(storm-functional-tests storm)
add_executable(storm-performance-tests ${STORM_PERFORMANCE_TEST_MAIN_FILE} ${STORM_PERFORMANCE_TEST_FILES})
target_link_libraries(storm-performance-tests storm)

#############################################################
##
##	Boost
##
#############################################################
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(storm ${Boost_LIBRARIES})   
#message(STATUS "BOOST_INCLUDE_DIRS is ${Boost_INCLUDE_DIRS}")
#message(STATUS "BOOST_LIBRARY_DIRS is ${Boost_LIBRARY_DIRS}")

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
target_link_libraries(storm cudd)

#############################################################
##
##	Gurobi (optional)
##
#############################################################
if (ENABLE_GUROBI)
    message (STATUS "StoRM - Linking with Gurobi (include: ${GUROBI_INCLUDE_DIRS})")
	include_directories(${GUROBI_INCLUDE_DIRS})
    target_link_libraries(storm ${GUROBI_LIBRARY})
endif(ENABLE_GUROBI)

#############################################################
##
##	CUDA (optional)
##
#############################################################
if (ENABLE_CUDA)
    message (STATUS "StoRM - Linking with CUDA")
    target_link_libraries(storm ${STORM_CUDA_LIB_NAME})
    include_directories("${PROJECT_SOURCE_DIR}/cuda/kernels/")
endif(ENABLE_CUDA)

#############################################################
##
##	glpk
##
#############################################################
message (STATUS "StoRM - Linking with glpk")
add_subdirectory("${PROJECT_SOURCE_DIR}/resources/3rdparty/glpk-4.53")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/glpk-4.53/src")
target_link_libraries(storm "glpk")

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
if(STORM_HAVE_Z3)
    message (STATUS "StoRM - Linking with Z3")
    include_directories(${Z3_INCLUDE_DIRS})
    target_link_libraries(storm ${Z3_LIBRARIES})
endif(STORM_HAVE_Z3)


#############################################################
##
##      carl
##
#############################################################

if(STORM_HAVE_CARL)
    message(STATUS "StoRM - Linking with carl.")
    include_directories("${carl_INCLUDE_DIR}")
    target_link_libraries(storm ${carl_LIBRARIES})
endif()

#############################################################
##
##      SMT-RAT
##
#############################################################

if(STORM_HAVE_SMTRAT)
message(STATUS "StoRM - Linking with smtrat.")
    include_directories("${smtrat_INCLUDE_DIR}")
    target_link_libraries(storm ${smtrat_LIBRARIES})
endif()

#############################################################
##
##	MathSAT (optional)
##
#############################################################
if (ENABLE_MSAT)
    message (STATUS "StoRM - Linking with MathSAT")
    include_directories("${MSAT_ROOT}/include")
    target_link_libraries(storm "mathsat")
    if(GMP_FOUND)
        include_directories("${GMP_INCLUDE_DIR}")
        target_link_libraries(storm "gmp")
    elseif(MPIR_FOUND)
        include_directories("${GMP_INCLUDE_DIR}")
        target_link_libraries(storm "mpir" "mpirxx")
    else(GMP_FOUND)
        message(FATAL_ERROR "GMP is required for MathSAT, but was not found!")
    endif(GMP_FOUND)
endif(ENABLE_MSAT)

#############################################################
##
##	Google Test gtest
##
#############################################################
set(gtest_force_shared_crt ON)
add_subdirectory("${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.7.0")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/gtest-1.7.0/include")
enable_testing()
target_link_libraries(storm-functional-tests gtest)
target_link_libraries(storm-performance-tests gtest)
add_test(NAME storm-functional-tests COMMAND storm-functional-tests)
add_test(NAME storm-performance-tests COMMAND storm-performance-tests)

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
target_link_libraries(storm log4cplusS)
target_link_libraries(storm-functional-tests log4cplusS)
target_link_libraries(storm-performance-tests log4cplusS)
if (UNIX AND NOT APPLE)
	target_link_libraries(storm rt)
	if (STORM_USE_COTIRE)
		target_link_libraries(storm_unity rt)
	endif(STORM_USE_COTIRE)
endif(UNIX AND NOT APPLE)

#############################################################
##
##	Intel Threading Building Blocks (optional)
##
#############################################################
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
include_directories(${THREADS_INCLUDE_DIRS})
target_link_libraries(storm ${CMAKE_THREAD_LIBS_INIT})
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
# Add a target to generate API documentation with Doxygen
if(DOXYGEN_FOUND)
    set(CMAKE_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc")
    string(REGEX REPLACE ";" " " CMAKE_DOXYGEN_INPUT_LIST "${PROJECT_SOURCE_DIR}/src")

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

    add_custom_target(doc ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

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

INSTALL(TARGETS storm-main storm-functional-tests storm-performance-tests
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
include(StormCPackConfig.cmake)
