add_custom_target(resources)
add_custom_target(test-resources)

set(STORM_3RDPARTY_SOURCE_DIR ${PROJECT_SOURCE_DIR}/resources/3rdparty)
set(STORM_3RDPARTY_BINARY_DIR ${PROJECT_BINARY_DIR}/resources/3rdparty)

####
#### Find autoreconf for cudd update step
find_program(AUTORECONF autoreconf)
mark_as_advanced(AUTORECONF)

#############################################################
##
##	l3pp
##
#############################################################



ExternalProject_Add(
        l3pp
        GIT_REPOSITORY https://github.com/hbruintjes/l3pp.git
        GIT_TAG master
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/l3pp
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_INSTALL ON
)
ExternalProject_Get_Property(l3pp source_dir)
set(l3pp_INCLUDE "${source_dir}/")
include_directories(${l3pp_INCLUDE})
add_dependencies(resources l3pp)

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
##	Eigen
##
#############################################################

# Add the shipped version of Eigen to the include pathes
set(EIGEN_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/resources/3rdparty/eigen-3.3-beta1")
include_directories(${EIGEN_INCLUDE_DIR})

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
##	ModernJSON
##
#############################################################

#use the shipped version of modernjson
message (STATUS "StoRM - Including ModernJSON")
include_directories("${PROJECT_SOURCE_DIR}/resources/3rdparty/modernjson/src/")

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

include(${STORM_3RDPARTY_SOURCE_DIR}/include_glpk.cmake)

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
include(${STORM_3RDPARTY_SOURCE_DIR}/include_cudd.cmake)

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
    find_package(carl QUIET)
    if(carl_FOUND)
        set(STORM_HAVE_CARL ON)
        message(STATUS "StoRM - Use system version of carl")
        message(STATUS "StoRM - Linking with carl ${carl_VERSION_STRING}")
        include_directories("${carl_INCLUDE_DIR}")
        list(APPEND STORM_LINK_LIBRARIES ${carl_LIBRARIES})
    else()
        message(STATUS "StoRM - Using shipped version of carl")
        #
        ExternalProject_Add(
                carl
                GIT_REPOSITORY https://github.com/smtrat/carl
                GIT_TAG master
                INSTALL_DIR ${STORM_3RDPARTY_BINARY_DIR}/carl
                SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/carl
                CMAKE_ARGS -DCXX=${CMAKE_CXX_COMPILER} -DEXPORT_TO_CMAKE=0 -DUSE_CLN_NUMBERS=1 -DCMAKE_SKIP_INSTALL_ALL_DEPENDENCY=1 -DCMAKE_INSTALL_PREFIX:PATH=${STORM_3RDPARTY_BINARY_DIR}/carl
                BUILD_IN_SOURCE 0
                BUILD_COMMAND make lib_carl
                INSTALL_COMMAND make install
                LOG_UPDATE ON
                LOG_CONFIGURE ON
                LOG_BUILD ON
                LOG_INSTALL ON
        )

        include_directories(${STORM_3RDPARTY_BINARY_DIR}/carl/include)
        list(APPEND STORM_LINK_LIBRARIES ${STORM_3RDPARTY_BINARY_DIR}/carl/lib/libcarl${DYNAMIC_EXT})
        set(STORM_HAVE_CARL ON)
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

include(${STORM_3RDPARTY_SOURCE_DIR}/include_xerces.cmake)

#############################################################
##
##	Sylvan
##
#############################################################

ExternalProject_Add(
        sylvan
        DOWNLOAD_COMMAND ""
        PREFIX "sylvan"
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/sylvan
        CMAKE_ARGS -DSYLVAN_BUILD_TEST=Off -DSYLVAN_BUILD_EXAMPLES=Off -DCMAKE_BUILD_TYPE=Release
        BINARY_DIR "${PROJECT_BINARY_DIR}/sylvan"
        BUILD_IN_SOURCE 0
        INSTALL_COMMAND ""
        INSTALL_DIR "${STORM_3RDPARTY_BINARY_DIR}/sylvan"
        LOG_CONFIGURE ON
        LOG_BUILD ON
)
ExternalProject_Get_Property(sylvan source_dir)
ExternalProject_Get_Property(sylvan binary_dir)
set(Sylvan_INCLUDE_DIR "${source_dir}/src")
set(Sylvan_LIBRARY "${binary_dir}/src/libsylvan.a")



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
ExternalProject_Add(
        googletest
        #For downloads (may be useful later!)
        #SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
        #TIMEOUT 10
        DOWNLOAD_COMMAND ""
        SOURCE_DIR "${STORM_3RDPARTY_SOURCE_DIR}/gtest-1.7.0"
        # Force the same output paths for debug and release builds so that
        # we know in which place the binaries end up when using the Xcode generator
        CMAKE_ARGS  -Dgtest_force_shared_crt=ON -DCXX=${CMAKE_CXX_COMPILER} -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=${STORM_3RDPARTY_BINARY_DIR}/gtest-1.7.0 -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=${STORM_3RDPARTY_BINARY_DIR}/gtest-1.7.0
        # Disable install step
        INSTALL_COMMAND ""
        BINARY_DIR "${STORM_3RDPARTY_BINARY_DIR}/gtest-1.7.0"
        INSTALL_DIR "${STORM_3RDPARTY_BINARY_DIR}/gtest-1.7.0"
        # Wrap download, configure and build steps in a script to log output
        LOG_CONFIGURE ON
        LOG_BUILD ON)

# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR ${source_dir}/include)
# Specify MainTest's link libraries
ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARIES ${binary_dir}/libgtest.a ${binary_dir}/libgtest_main.a)

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