
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
ExternalProject_Add(
        l3pp_ext
        GIT_REPOSITORY https://github.com/hbruintjes/l3pp.git
        GIT_TAG e4f8d7fe6c328849aff34d2dfd6fd592c14070d5
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/l3pp
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_INSTALL ON
)
ExternalProject_Get_Property(l3pp_ext source_dir)
set(l3pp_INCLUDE "${source_dir}/")
storm_add_imported_library_interface(l3pp "${l3pp_INCLUDE}" "")
list(APPEND STORM_DEP_TARGETS l3pp)
add_dependencies(l3pp l3pp_ext)

#############################################################
##
##	gmm
##
#############################################################

storm_add_imported_library_interface(gmm "${STORM_3RDPARTY_SOURCE_DIR}/gmm-5.2/include" "")
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
        SOURCE_DIR ${STORM_3RDPARTY_INCLUDE_DIR}/StormEigen
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/StormEigen-3.4.1alpha
        # First check whether patch was already applied (--reverse --check), otherwise apply patch
        PATCH_COMMAND git apply ${STORM_3RDPARTY_SOURCE_DIR}/patches/eigen341alpha.patch --reverse --check || git apply ${STORM_3RDPARTY_SOURCE_DIR}/patches/eigen341alpha.patch
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_INSTALL ON
)
storm_add_imported_library_interface(StormEigen "${STORM_3RDPARTY_INCLUDE_DIR}" "")
list(APPEND STORM_DEP_TARGETS StormEigen)
add_dependencies(StormEigen eigen_src)

#############################################################
##
##	Boost
##
#############################################################

# Boost Option variables
set(Boost_USE_STATIC_LIBS ${USE_BOOST_STATIC_LIBRARIES})
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_NO_BOOST_CMAKE ON)
set(Boost_NO_WARN_NEW_VERSIONS ON)

find_package(Boost 1.65.1 QUIET REQUIRED COMPONENTS filesystem system)
if (NOT Boost_FOUND)
    if (Boost_VERSION)
        message(FATAL_ERROR "The required Boost version is 1.65.1 or newer, however, only ${Boost_VERSION} was found.")
    else ()
        message(FATAL_ERROR "Boost was not found.")
    endif ()
endif ()
if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
    set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
endif ()

if (${Boost_VERSION} VERSION_GREATER_EQUAL "1.81.0")
    message(STATUS "Storm - Using workaround for Boost >= 1.81")
    add_compile_options(-DBOOST_PHOENIX_STL_TUPLE_H_) # or add compile_definitions?
endif()

set(CNTVAR 1)
foreach(BOOSTLIB ${Boost_LIBRARIES})
    add_imported_library(target-boost-${CNTVAR} SHARED ${BOOSTLIB} ${Boost_INCLUDE_DIRS})
    list(APPEND STORM_DEP_TARGETS target-boost-${CNTVAR}_SHARED)
    MATH(EXPR CNTVAR "${CNTVAR}+1")
endforeach()
message(STATUS "Storm - Using boost ${Boost_VERSION} (library version ${Boost_LIB_VERSION}).")
# set the information for the config header
set(STORM_BOOST_INCLUDE_DIR "${Boost_INCLUDE_DIRS}")


#############################################################
##
##	ExprTk
##
#############################################################

# Use the shipped version of ExprTK
message (STATUS "Storm - Including ExprTk.")
storm_add_imported_library_interface(ExprTk "${STORM_3RDPARTY_SOURCE_DIR}/exprtk" "")
list(APPEND STORM_DEP_TARGETS ExprTk)

#############################################################
##
##	Parallel Hashmap
##
#############################################################

# Use the shipped version of Parallel Hashmap
message (STATUS "Storm - Including Parallel Hashmap.")
storm_add_imported_library_interface(phmap "${STORM_3RDPARTY_SOURCE_DIR}/parallel_hashmap" "")
list(APPEND STORM_DEP_TARGETS phmap)

#############################################################
##
##	cpphoafparser
##
#############################################################

# Use the shipped version of cpphoafparser
message (STATUS "Storm - Including cpphoafparser 0.99.2")
storm_add_imported_library_interface(cppHOAparser "${STORM_3RDPARTY_SOURCE_DIR}/cpphoafparser-0.99.2/include" "")
list(APPEND STORM_DEP_TARGETS cppHOAparser)

#############################################################
##
##	ModernJSON
##
#############################################################

#use the shipped version of modernjson
message (STATUS "Storm - Including ModernJSON.")
storm_add_imported_library_interface(ModernJSON "${STORM_3RDPARTY_SOURCE_DIR}/modernjson/include/" "")
list(APPEND STORM_DEP_TARGETS ModernJSON)

#############################################################
##
##	Z3 (optional)
##
#############################################################

find_package(Z3 QUIET)

# Z3 Defines
set(STORM_HAVE_Z3 ${Z3_FOUND})

if(Z3_FOUND)
    if(NOT EXISTS "${Z3_INCLUDE_DIR}/z3_version.h")
        message(FATAL_ERROR "No file z3_version.h found in ${Z3_INCLUDE_DIR}.")
    endif()

    # Parse z3 version from version file
    file(STRINGS ${Z3_INCLUDE_DIR}/z3_version.h Z3_VERSION_MAJOR REGEX "^#define[\t ]+Z3_MAJOR_VERSION .*")
    file(STRINGS ${Z3_INCLUDE_DIR}/z3_version.h Z3_VERSION_MINOR REGEX "^#define[\t ]+Z3_MINOR_VERSION .*")
    file(STRINGS ${Z3_INCLUDE_DIR}/z3_version.h Z3_VERSION_PATCH REGEX "^#define[\t ]+Z3_BUILD_NUMBER .*")
    string(REGEX MATCH "[0-9]+$" Z3_VERSION_MAJOR "${Z3_VERSION_MAJOR}")
    string(REGEX MATCH "[0-9]+$" Z3_VERSION_MINOR "${Z3_VERSION_MINOR}")
    string(REGEX MATCH "[0-9]+$" Z3_VERSION_PATCH "${Z3_VERSION_PATCH}")
    set(Z3_VERSION "${Z3_VERSION_MAJOR}.${Z3_VERSION_MINOR}.${Z3_VERSION_PATCH}")

    if(Z3_VERSION)
        # Split Z3 version into its components
        string(REPLACE "." ";" Z3_VERSION_LIST ${Z3_VERSION})
        list(GET Z3_VERSION_LIST 0 STORM_Z3_VERSION_MAJOR)
        list(GET Z3_VERSION_LIST 1 STORM_Z3_VERSION_MINOR)
        list(GET Z3_VERSION_LIST 2 STORM_Z3_VERSION_PATCH)

        if ("${Z3_VERSION}" VERSION_EQUAL "4.10.1")
            message (WARNING "Storm cannot be linked against Z3 version 4.10.1, see issue 252.")
            message(WARNING "Storm - Could not link with z3. Building of Prism/JANI models will not be supported.")
            set(Z3_FOUND FALSE)
        else()
            # Check whether the version of z3 supports optimization
            if(NOT "${Z3_VERSION}" VERSION_LESS "4.5.0")
                set(STORM_HAVE_Z3_OPTIMIZE ON)
                message (STATUS "Storm - Linking with Z3 (version ${Z3_VERSION}). (Z3 version supports optimization)")
            else()
                message (STATUS "Storm - Linking with Z3 (version ${Z3_VERSION}). (Z3 version does not support optimization)")
            endif()
            if (NOT "${Z3_VERSION}" VERSION_LESS "4.7.1")
                set(STORM_Z3_API_USES_STANDARD_INTEGERS ON)
            endif()

            add_imported_library(Z3 SHARED ${Z3_LIBRARIES} ${Z3_INCLUDE_DIRS})
            list(APPEND STORM_DEP_TARGETS Z3_SHARED)
        endif()
    else()
        message(WARNING "Storm - Could not obtain Z3 version. Building of Prism/JANI models will not be supported.")
        set(Z3_FOUND FALSE)
    endif()

else()
    message (WARNING "Storm - Z3 not found. Building of Prism/JANI models will not be supported.")
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
    find_package(GUROBI QUIET REQUIRED)
    set(STORM_HAVE_GUROBI ${GUROBI_FOUND})
    if (GUROBI_FOUND)
		if (EXISTS ${GUROBI_LIBRARY})
            message (STATUS "Storm - Linking with Gurobi (${GUROBI_CXX_LIBRARY}).")
            add_imported_library(GUROBI SHARED ${GUROBI_LIBRARY} ${GUROBI_INCLUDE_DIRS})
            list(APPEND STORM_DEP_TARGETS GUROBI_SHARED)
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
# Boost must have been configured above. It would be good to check that.


# Set compile flags for dependencies
if(STORM_USE_CLN_EA OR STORM_USE_CLN_RF)
    set(SHIPPED_CARL_USE_CLN_NUMBERS ON)
    set(SHIPPED_CARL_USE_GINAC ON)
else()
    set(SHIPPED_CARL_USE_CLN_NUMBERS OFF)
    set(SHIPPED_CARL_USE_GINAC OFF)
endif()

set(STORM_HAVE_CARL OFF)
set(CARL_MINVERSION "14.26")
set(CARL_C14VERSION "14")

include(FetchContent)
FETCHCONTENT_DECLARE(
        carl
        GIT_REPOSITORY ${STORM_CARL_GIT_REPO}
        GIT_TAG ${STORM_CARL_GIT_TAG}
)
SET(EXCLUDE_TESTS_FROM_ALL ON)
SET(THREAD_SAFE ON)
SET(Boost_NO_SYSTEM_PATHS ON)
SET(BOOST_INCLUDEDIR ${Boost_INCLUDE_DIRS})
SET(BOOST_LIBRARYDIR ${Boost_LIBRARY_DIRS})
SET(USE_CLN_NUMBERS ${SHIPPED_CARL_USE_CLN_NUMBERS})
SET(CARL_LIB_INSTALL_DIR "lib/storm")
SET(CARL_INCLUDE_INSTALL_DIR "include/storm")
SET(CARL_EXPORT_TO_CMAKE OFF)
FETCHCONTENT_MAKEAVAILABLE(carl)


include(${carl_BINARY_DIR}/carlConfig.cmake)
#
set(STORM_HAVE_CLN ${CARL_USE_CLN_NUMBERS})
message(STATUS "carl version ${carl_VERSION} use cln: ${STORM_HAVE_CLN}")
set(STORM_HAVE_GINAC ${CARL_USE_GINAC})
#
add_dependencies(storm_resources lib_carl)
set(STORM_HAVE_CARL ON)
set(STORM_CARL_SUPPORTS_FWD_DECL ON)


# The library that needs symbols must be first, then the library that resolves the symbol.
list(APPEND STORM_DEP_IMP_TARGETS lib_carl)
list(APPEND STORM_DEP_IMP_TARGETS GMPXX_SHARED GMP_SHARED)


if(STORM_USE_CLN_RF AND NOT STORM_HAVE_CLN)
    message(FATAL_ERROR "Cannot use CLN numbers if carl is build without.")
endif()
if(STORM_USE_CLN_RF AND NOT STORM_HAVE_GINAC)
    message(FATAL_ERROR "Cannot use CLN numbers if carl is build without ginac.")
endif()


if(STORM_HAVE_CLN)
    get_target_property(CLN_INCLUDE_DIR CLN_SHARED INTERFACE_INCLUDE_DIRECTORIES)
endif()

if(STORM_USE_CLN_EA OR STORM_USE_CLN_RF)
    list(APPEND STORM_DEP_IMP_TARGETS GINAC_SHARED CLN_SHARED)
endif()

#
#get_target_property(GMPXX_LIB GMPXX_SHARED IMPORTED_LIB_LOCATION)
#get_target_property(GMP_LIB GMP_SHARED IMPORTED_LIB_LOCATION)
#get_target_property(GMPXX_INCLUDE_DIR GMPXX_SHARED INTERFACE_INCLUDE_DIRECTORIES)
#get_target_property(GMP_INCLUDE_DIR GMP_SHARED INTERFACE_INCLUDE_DIRECTORIES)
#get_filename_component(GMP_LIB_LOCATION ${GMP_LIB} DIRECTORY)
#get_filename_component(GMPXX_LIB_LOCATION ${GMPXX_LIB} DIRECTORY)
#

#############################################################
##
##	MathSAT (optional)
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
##
#############################################################

include(${STORM_3RDPARTY_SOURCE_DIR}/include_spot.cmake)

#############################################################
##
##	Sylvan
##
#############################################################

if (STORM_DEBUG_SYLVAN)
    set(SYLVAN_BUILD_TYPE "Debug")
else()
    set(SYLVAN_BUILD_TYPE "Release")
endif()

ExternalProject_Add(
        sylvan
        DOWNLOAD_COMMAND ""
        PREFIX "sylvan"
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/sylvan
        CMAKE_ARGS -DPROJECT_NAME=storm -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DSYLVAN_BUILD_DOCS=OFF -DSYLVAN_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=${SYLVAN_BUILD_TYPE} -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DSYLVAN_GMP=ON -DUSE_CARL=ON -Dcarl_DIR=${carl_BINARY_DIR} -DBUILD_SHARED_LIBS=OFF
        BINARY_DIR ${STORM_3RDPARTY_BINARY_DIR}/sylvan
        BUILD_IN_SOURCE 0
        INSTALL_COMMAND ""
        INSTALL_DIR ${STORM_3RDPARTY_BINARY_DIR}/sylvan
        LOG_CONFIGURE ON
        LOG_BUILD ON
        DEPENDS lib_carl
        BUILD_BYPRODUCTS ${STORM_3RDPARTY_BINARY_DIR}/sylvan/src/libsylvan${STATIC_EXT}
        BUILD_ALWAYS 1
)
# BUILD ALWAYS ON due to: https://stackoverflow.com/questions/46708124/cmake-doesnt-rebuild-externalproject-on-changes

ExternalProject_Get_Property(sylvan source_dir)
ExternalProject_Get_Property(sylvan binary_dir)
set(sylvan_INCLUDE_DIR "${source_dir}/src")
set(sylvan_LIBRARY "${binary_dir}/src/libsylvan${STATIC_EXT}")
message(STATUS "Storm - Using shipped version of sylvan.")
message(STATUS "Storm - Linking with sylvan.")
add_imported_library(sylvan STATIC ${sylvan_LIBRARY} ${sylvan_INCLUDE_DIR})
add_dependencies(sylvan_STATIC sylvan)

list(APPEND STORM_DEP_TARGETS sylvan_STATIC)

#############################################################
##
##	Google Test gtest
##
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
list(APPEND STORM_DEP_TARGETS Threads::Threads)


add_custom_target(copy_resources_headers DEPENDS ${PHMAP_BINDIR_HEADERS})
