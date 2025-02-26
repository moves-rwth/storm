
if(PROJECT_TOP_LEVEL)
    add_custom_target(resources)
    add_dependencies(resources storm_resources)
endif()
add_custom_target(test-resources)

set(STORM_3RDPARTY_SOURCE_DIR ${PROJECT_SOURCE_DIR}/resources/3rdparty)
set(STORM_3RDPARTY_BINARY_DIR ${PROJECT_BINARY_DIR}/resources/3rdparty)
set(STORM_3RDPARTY_INCLUDE_DIR ${PROJECT_BINARY_DIR}/include/resources/3rdparty)

message(STATUS "Storm - Building external resources with ${STORM_RESOURCES_BUILD_JOBCOUNT} job(s) in parallel.")

#############################################################
##
##	l3pp
##
#############################################################

# Do not take a branch, needs internet connection.
message (STATUS "Storm - Including l3pp logging.")
ExternalProject_Add(
        l3pp_ext
        GIT_REPOSITORY https://github.com/hbruintjes/l3pp.git
        GIT_TAG e4f8d7fe6c328849aff34d2dfd6fd592c14070d5
        SOURCE_DIR ${STORM_3RDPARTY_BINARY_DIR}/l3pp
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_INSTALL ON
        LOG_DOWNLOAD ON
)
add_library(l3pp INTERFACE) # Not imported, we are in control of the sources.
add_dependencies(l3pp l3pp_ext)
target_include_directories(l3pp INTERFACE
        $<BUILD_INTERFACE:${STORM_3RDPARTY_BINARY_DIR}/l3pp>
        $<INSTALL_INTERFACE:${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/l3pp>
)
install(TARGETS l3pp EXPORT storm_Targets)
install(DIRECTORY ${STORM_3RDPARTY_BINARY_DIR}/l3pp/ DESTINATION ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/l3pp FILES_MATCHING PATTERN "*.h" PATTERN ".git" EXCLUDE)
list(APPEND STORM_DEP_TARGETS l3pp)

#############################################################
##
##	gmm
##
#############################################################

message (STATUS "Storm - Including gmm 5.2.")
add_library(gmm INTERFACE) # Not imported, we are in control of the sources.
target_include_directories(gmm INTERFACE
        $<BUILD_INTERFACE:${STORM_3RDPARTY_SOURCE_DIR}/gmm-5.2/include>
        $<INSTALL_INTERFACE:${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/gmm>
)
install(TARGETS gmm EXPORT storm_Targets)
install(DIRECTORY ${STORM_3RDPARTY_SOURCE_DIR}/gmm-5.2/include/
        DESTINATION ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/gmm
        FILES_MATCHING PATTERN "*.h" PATTERN ".git" EXCLUDE)
list(APPEND STORM_DEP_TARGETS gmm)

#############################################################
##
##	Eigen
##
#############################################################

# Checkout Eigen
# We're checking out Eigen using the (currently not officially released) version 3.4.1
# See here https://gitlab.com/libeigen/eigen/-/commit/bae907b8f6078b1df290729eef946360315bd312
# The current official release is 3.4.0 which results in issues, see https://github.com/moves-rwth/storm/issues/162
message (STATUS "Storm - Including Eigen commit bae907b8f6078b1df290729eef946360315bd312 (unofficial 3.4.1 release).")
ExternalProject_Add(
        eigen_src
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG bae907b8f6078b1df290729eef946360315bd312
        SOURCE_DIR ${STORM_3RDPARTY_BINARY_DIR}/StormEigen
        # First check whether patch was already applied (--reverse --check), otherwise apply patch
        PATCH_COMMAND git apply ${STORM_3RDPARTY_SOURCE_DIR}/patches/eigen341alpha.patch --reverse --check || git apply ${STORM_3RDPARTY_SOURCE_DIR}/patches/eigen341alpha.patch
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_INSTALL ON
)
add_library(eigen3storm INTERFACE) # Not imported, we are in control of the sources.
add_dependencies(eigen3storm eigen_src)
# note that we include without StormEigen
target_include_directories(eigen3storm INTERFACE
        $<BUILD_INTERFACE:${STORM_3RDPARTY_BINARY_DIR}>
        $<INSTALL_INTERFACE:${STORM_RESOURCE_INCLUDE_INSTALL_DIR}>
)
install(TARGETS eigen3storm EXPORT storm_Targets)
install(DIRECTORY ${STORM_3RDPARTY_BINARY_DIR}/StormEigen/
        DESTINATION ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/StormEigen
        FILES_MATCHING PATTERN "*.h" PATTERN ".git" EXCLUDE)
list(APPEND STORM_DEP_TARGETS eigen3storm)

#############################################################
##
##	Boost
##
#############################################################

# Boost Option variables
set(Boost_NO_WARN_NEW_VERSIONS ON)
find_package(Boost 1.65.1 QUIET REQUIRED)
if (${Boost_VERSION} VERSION_GREATER_EQUAL "1.81.0")
    message(STATUS "Storm - Using workaround for Boost >= 1.81")
    add_compile_options(-DBOOST_PHOENIX_STL_TUPLE_H_) # or add compile_definitions?
    # TODO Only do this for a specific target?
endif()

message(STATUS "Storm - Using boost ${Boost_VERSION} (library version ${Boost_LIB_VERSION}).")
list(APPEND STORM_DEP_IMP_TARGETS Boost::boost)

#############################################################
##
##	ExprTk
##
#############################################################

# Use the shipped version of ExprTK
message (STATUS "Storm - Including ExprTk.")
set(exprTk_SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/exprtk)
set(exprTk_INSTALL_DIR ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/exprtk)
add_library(exprTk INTERFACE) # Not imported, we are in control of the sources.
target_include_directories(exprTk INTERFACE
        $<BUILD_INTERFACE:${exprTk_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${exprTk_INSTALL_DIR}>
)
install(TARGETS exprTk EXPORT storm_Targets)
install(DIRECTORY ${exprTk_SOURCE_DIR}
        DESTINATION ${exprTk_INSTALL_DIR}
        FILES_MATCHING PATTERN "*.hpp" PATTERN ".git" EXCLUDE)
list(APPEND STORM_DEP_TARGETS exprTk)

#############################################################
##
##	Parallel Hashmap
##
#############################################################

# Use the shipped version of Parallel Hashmap
message (STATUS "Storm - Including Parallel Hashmap.")
set(phmap_SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/parallel_hashmap)
set(phmap_INSTALL_DIR ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/parallel_hashmap)
add_library(phmap INTERFACE) # Not imported, we are in control of the sources.
target_include_directories(phmap INTERFACE
        $<BUILD_INTERFACE:${phmap_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${phmap_INSTALL_DIR}>)
install(TARGETS phmap EXPORT storm_Targets)
install(DIRECTORY ${phmap_SOURCE_DIR}
        DESTINATION ${phmap_INSTALL_DIR}
        FILES_MATCHING PATTERN "*.h" PATTERN ".git" EXCLUDE)
list(APPEND STORM_DEP_TARGETS phmap)

#############################################################
##
##	cpphoafparser
##
#############################################################

# Use the shipped version of cpphoafparser
message (STATUS "Storm - Including cpphoafparser 0.99.2")
set(cppHOAparser_SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/cpphoafparser-0.99.2/include)
set(cppHOAparser_INSTALL_DIR ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/cpphoafparser-0.99.2/)
add_library(cppHOAparser INTERFACE) # Not imported, we are in control of the sources.
target_include_directories(cppHOAparser INTERFACE
        $<BUILD_INTERFACE:${cppHOAparser_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${cppHOAparser_INSTALL_DIR}>)
install(TARGETS cppHOAparser EXPORT storm_Targets)
install(DIRECTORY ${cppHOAparser_SOURCE_DIR}
        DESTINATION ${cppHOAparser_INSTALL_DIR}
        FILES_MATCHING PATTERN "*.h" PATTERN ".git" EXCLUDE)
list(APPEND STORM_DEP_TARGETS cppHOAparser)

#############################################################
##
##	ModernJSON
##
#############################################################

#use the shipped version of modernjson
message (STATUS "Storm - Including ModernJSON.")
set(ModernJSON_SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/modernjson/include/)
set(ModernJSON_INSTALL_DIR ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/modernjson/)
add_library(ModernJSON INTERFACE) # Not imported, we are in control of the sources.
target_include_directories(ModernJSON INTERFACE
        $<BUILD_INTERFACE:${ModernJSON_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${ModernJSON_INSTALL_DIR}>)
install(TARGETS ModernJSON EXPORT storm_Targets)
install(DIRECTORY ${ModernJSON_SOURCE_DIR}
        DESTINATION ${ModernJSON_INSTALL_DIR}
        FILES_MATCHING PATTERN "*.hpp" PATTERN ".git" EXCLUDE)
list(APPEND STORM_DEP_TARGETS ModernJSON)

#############################################################
##
##	Z3 (optional)
##
#############################################################

find_package(Z3 QUIET 4.8.7 NO_MODULE)

if(TARGET z3::libz3)
    set(STORM_HAVE_Z3 ON)

    # TODO remove STORM_HAVE_Z3_OPTIMIZE checks
    # TODO remove STORM_Z3_API_USES_STANDARD_INTEGERS checks
    set(STORM_Z3_API_USES_STANDARD_INTEGERS ON)
    set(STORM_HAVE_Z3_OPTIMIZE ON)

    set(Z3_VERSION "${Z3_VERSION_MAJOR}.${Z3_VERSION_MINOR}.${Z3_VERSION_PATCH}")

    if ("${Z3_VERSION}" VERSION_EQUAL "4.10.1")
        message (WARNING "Storm cannot be linked against Z3 version 4.10.1, see issue 252.")
        message(WARNING "Storm - Could not link with z3. Building of Prism/JANI models will not be supported.")
        set(Z3_FOUND FALSE)
    else()
        # Check whether the version of z3 supports optimization
        message (STATUS "Storm - Linking with Z3 (version ${Z3_VERSION}). (Z3 version supports optimization)")
        list(APPEND STORM_DEP_IMP_TARGETS z3::libz3)
    endif()
else()
    message (WARNING "Storm - Z3 not found. Building of Prism/JANI models will not be supported.")
endif()


#############################################################
##
##	glpk
##  TODO currently requires pre-installed and requires custom findpackage
#############################################################

include(${STORM_3RDPARTY_SOURCE_DIR}/include_glpk.cmake)

#############################################################
##
##	Gurobi (optional)
##  TODO
#############################################################

if (STORM_USE_GUROBI)
    find_package(GUROBI QUIET REQUIRED)
    set(STORM_HAVE_GUROBI ${GUROBI_FOUND})
    if (GUROBI_FOUND)
		if (EXISTS ${GUROBI_LIBRARY})
            message (STATUS "Storm - Linking with Gurobi (${GUROBI_CXX_LIBRARY}).")
            add_imported_library(GUROBI SHARED ${GUROBI_LIBRARY} ${GUROBI_INCLUDE_DIRS})
            list(APPEND STORM_DEP_IMP_TARGETS GUROBI_SHARED)
        else()
            # The FindGurobi.cmake script needs to be updated every now and then as the library file contains the version number...
            message(FATAL_ERROR "Gurobi Library ${GUROBI_LIBRARY} not found. If your Gurobi Version is higher then 9.0.0, please contact the Storm developers.")
        endif()
    else()
    	# The FindGurobi.cmake script needs to be updated every now and then as the library file contains the version number ...
        message(FATAL_ERROR "Gurobi Library requested but was not found. Make sure that GUROBI_ROOT points to the correct directory (containing include/ and lib/ subdirectories). If your Gurobi Version is higher then 9.0.0, please contact the Storm developers.")
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
##      carl (including cln and ginac)
##
#############################################################
# TODO Boost might need to have been configured above. It would be good to check that.

# Set compile flags for dependencies
# TODO probably broken.
if(STORM_USE_CLN_EA OR STORM_USE_CLN_RF)
    set(SHIPPED_CARL_USE_CLN_NUMBERS ON)
    set(SHIPPED_CARL_USE_GINAC ON)
else()
    set(SHIPPED_CARL_USE_CLN_NUMBERS OFF)
    set(SHIPPED_CARL_USE_GINAC OFF)
endif()

set(STORM_HAVE_CARL OFF)
set(CARL_MINVERSION "14.28")
set(CARL_C14VERSION "14")

include(FetchContent)
FETCHCONTENT_DECLARE(
        carl
        GIT_REPOSITORY ${STORM_CARL_GIT_REPO}
        GIT_TAG ${STORM_CARL_GIT_TAG}
)
SET(EXCLUDE_TESTS_FROM_ALL ON)  #carl without tests
SET(THREAD_SAFE ON) #carl thread safe code
SET(BOOST_INCLUDEDIR ${Boost_INCLUDE_DIRS}) #TODO not sure whether we still need this. It would be good to feed the boost::headers target into carl though
SET(BOOST_LIBRARYDIR ${Boost_LIBRARY_DIRS}) #TODO not sure whether we still need this
SET(USE_CLN_NUMBERS ${SHIPPED_CARL_USE_CLN_NUMBERS})
SET(CARL_LIB_INSTALL_DIR "lib/storm")
SET(CARL_INCLUDE_INSTALL_DIR "include/storm") # maybe use resources folder /carl instead for consistency?
SET(CARL_EXPORT_TO_CMAKE OFF)
SET(CARL_CMAKE_INSTALL_DIR "lib/cmake/storm")
FETCHCONTENT_MAKEAVAILABLE(carl)

#include(${carl_BINARY_DIR}/carlConfig.cmake)
#
# TODO I would like to read these variables
set(STORM_HAVE_CLN ON )#${CARL_USE_CLN_NUMBERS})
set(STORM_HAVE_GINAC ON ) #${CARL_USE_GINAC})
message(STATUS "storm -- using carl version ${carl_VERSION} (cln: ${STORM_HAVE_CLN}, ginac: ${STORM_HAVE_GINAC})")
#
add_dependencies(storm_resources lib_carl)
# TODO this old flag is no longer really relevant as CARL must always be present.
set(STORM_HAVE_CARL ON)
# This flag is currently required to pick the right carl adapter.
# TODO It can be removed as the old versions that did not support forward declarations are no longer supported either way.
set(STORM_CARL_SUPPORTS_FWD_DECL ON)
list(APPEND STORM_FETCHED_TARGETS lib_carl)

# Some checks to see whether we correctly configured carl.
if(STORM_USE_CLN_RF AND NOT STORM_HAVE_CLN)
    message(FATAL_ERROR "Cannot use CLN numbers if carl is build without.")
endif()
if(STORM_USE_CLN_RF AND NOT STORM_HAVE_GINAC)
    message(FATAL_ERROR "Cannot use CLN numbers if carl is build without ginac.")
endif()

#############################################################
##
##	MathSAT (optional)
##  TODO: To be updated
##
#############################################################
if (STORM_USE_MATHSAT)
    find_package(MATHSAT QUIET)
    if(MATHSAT_FOUND)
        set(STORM_HAVE_MATHSAT ON)
        if(${OPERATING_SYSTEM} MATCHES "Linux")
            add_imported_library(mathsat SHARED ${MATHSAT_LIBRARIES} ${MATHSAT_INCLUDE_DIRS})
            list(APPEND STORM_DEP_TARGETS mathsat_SHARED)
            message(STATUS "Storm - Linking with MathSAT: (library: ${mathsat_SHARED}; include: ${MATHSAT_INCLUDE_DIRS})")
        else()
            # on macOS, the .dylib file has some hard coded path (Version 5.5.4) and we therefore link statically
            add_imported_library(mathsat STATIC ${MATHSAT_LIB} ${MATHSAT_INCLUDE_DIRS})
            list(APPEND STORM_DEP_TARGETS mathsat_STATIC)
            message(STATUS "Storm - Linking with MathSAT: (library: ${mathsat_STATIC}; include: ${MATHSAT_INCLUDE_DIRS})")
        endif()
    else()
        set(STORM_HAVE_MATHSAT OFF)
        message(FATAL_ERROR "Storm - MathSAT library requested but was not found. Make sure that MATHSAT_ROOT points to the correct directory.")
    endif()
else()
    set(STORM_HAVE_MATHSAT OFF)
endif()

#############################################################
##
##	QVBS (Quantitative verification benchmark set)
##
##  TODO: Consider installing these files as well?
#############################################################

set(STORM_HAVE_QVBS OFF)
if (STORM_LOAD_QVBS)
    ExternalProject_Add(
            download_qvbs
            GIT_REPOSITORY https://github.com/ahartmanns/qcomp.git
            GIT_SHALLOW 1
            PREFIX ${PROJECT_BINARY_DIR}/qvbs
            UPDATE_COMMAND ""
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
            LOG_INSTALL ON
    )
    add_dependencies(storm_resources download_qvbs)
    set(STORM_HAVE_QVBS ON)
    ExternalProject_Get_Property(download_qvbs source_dir)
    set(STORM_QVBS_ROOT "${source_dir}/benchmarks")
elseif (NOT "${STORM_QVBS_ROOT}" STREQUAL "")
    if( EXISTS "${STORM_QVBS_ROOT}/index.json")
        set(STORM_HAVE_QVBS ON)
    else()
        message(FATAL_ERROR "No file 'index.json' in provided QVBS root directory ${STORM_QVBS_ROOT}")
    endif()
endif()
if (STORM_HAVE_QVBS)
    message (STATUS "Storm - Enabled inputs from QVBS located at ${STORM_QVBS_ROOT}")
endif()


#############################################################
##
##	Xerces
##
#############################################################

include(${STORM_3RDPARTY_SOURCE_DIR}/include_xerces.cmake)

#############################################################
##
##	SoPlex
##
#############################################################

include(${STORM_3RDPARTY_SOURCE_DIR}/include_soplex.cmake)

#############################################################
##
##	Spot
##  TODO
#############################################################

include(${STORM_3RDPARTY_SOURCE_DIR}/include_spot.cmake)

#############################################################
##
##	Sylvan
##
#############################################################

FETCHCONTENT_DECLARE(
        sylvanfetch
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/sylvan
)
SET(SYLVAN_GMP ON)
SET(USE_CARL ON)
SET(SYLVAN_INSTALL_BINDIR ${STORM_RESOURCE_BIN_INSTALL_DIR}/sylvan)
SET(SYLVAN_INSTALL_LIBDIR ${STORM_RESOURCE_LIBRARY_INSTALL_DIR}/sylvan)
SET(SYLVAN_INSTALL_INCLUDEDIR ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/sylvan)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON) # Likely only necessary on linux
SET(STORM_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/") # TODO this is ugly.
SET(STORM_BINARY_INCLUDE_DIR "${PROJECT_BINARY_DIR}/include/") #TODO this is ugly.
FETCHCONTENT_MAKEAVAILABLE(sylvanfetch)

# TODO We previously had -DSYLVAN_GMP=ON. Why?

list(APPEND STORM_FETCHED_TARGETS sylvan)

#############################################################
##
##	Google Test gtest
##  TODO (not urgent, tests are not installed either way)
#############################################################
set(GOOGLETEST_LIB_DIR ${STORM_3RDPARTY_BINARY_DIR}/googletest)
ExternalProject_Add(
        googletest
        #For downloads (may be useful later!)
        #SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
        #TIMEOUT 10
        DOWNLOAD_COMMAND ""
        SOURCE_DIR "${STORM_3RDPARTY_SOURCE_DIR}/googletest"
        # Force the same output paths for debug and release builds so that
        # we know in which place the binaries end up when using the Xcode generator
        CMAKE_ARGS  -Dgtest_force_shared_crt=ON -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=${GOOGLETEST_LIB_DIR} -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=${GOOGLETEST_LIB_DIR}
        # Disable install step
        INSTALL_COMMAND ""
        BINARY_DIR ${GOOGLETEST_LIB_DIR}
        INSTALL_DIR ${GOOGLETEST_LIB_DIR}
        # Wrap download, configure and build steps in a script to log output
        LOG_CONFIGURE ON
        LOG_BUILD ON
        BUILD_BYPRODUCTS ${GOOGLETEST_LIB_DIR}/lib/libgtest${STATIC_EXT} ${GOOGLETEST_LIB_DIR}/lib/libgtest_main${STATIC_EXT}
)

# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR ${source_dir}/googletest/include)
set(GMOCK_INCLUDE_DIR ${source_dir}/googlemock/include)
# Specify MainTest's link libraries
ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARIES ${binary_dir}/lib/libgtest${STATIC_EXT} ${binary_dir}/lib/libgtest_main${STATIC_EXT})

add_dependencies(test-resources googletest)
list(APPEND STORM_TEST_LINK_LIBRARIES ${GTEST_LIBRARIES})

#############################################################
##
##	Threads
##
#############################################################

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
list(APPEND STORM_DEP_IMP_TARGETS Threads::Threads)

