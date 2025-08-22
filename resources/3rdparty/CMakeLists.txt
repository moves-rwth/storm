if(PROJECT_TOP_LEVEL)
    add_custom_target(resources)
    add_dependencies(resources storm_resources)
endif()
add_custom_target(test-resources)


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
        GIT_TAG 513c7589232396e4b8975113b7709a8f39b46c85
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
add_dependencies(storm_resources l3pp)
list(APPEND STORM_DEP_TARGETS l3pp)

#############################################################
##
##	gmm
##
#############################################################

if(NOT STORM_DISABLE_GMM)
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
    set(STORM_HAVE_GMM ON)
else()
    message (WARNING "Storm - Skipping gmm.")
    set(STORM_HAVE_GMM OFF)
endif()

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
        PATCH_COMMAND git apply ${STORM_3RDPARTY_SOURCE_DIR}/patches/eigen341alpha.patch --reverse --check -q || git apply ${STORM_3RDPARTY_SOURCE_DIR}/patches/eigen341alpha.patch
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
        LOG_INSTALL ON
)
if(STORM_WARNING_AS_ERROR AND STORM_COMPILER_GCC)
        # Ignore warning in Eigen
        add_compile_options(-Wno-class-memaccess)
endif()
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
        PATTERN ".git" EXCLUDE)
add_dependencies(storm_resources eigen3storm)
list(APPEND STORM_DEP_TARGETS eigen3storm)

#############################################################
##
##	Boost
##
#############################################################

# Boost Option variables
set(Boost_NO_WARN_NEW_VERSIONS ON)
find_package(Boost 1.70.0 QUIET REQUIRED NO_MODULE)
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
        $<INSTALL_INTERFACE:${exprTk_INSTALL_DIR}/exprtk>
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

if(NOT STORM_DISABLE_Z3)
    find_package(Z3 QUIET 4.8.7 NO_MODULE)
    # Notice that such old z3 versions do not provide config files, so we could bump this version ahead.
    if(TARGET z3::libz3)
        set(STORM_HAVE_Z3 ON)
        set(STORM_Z3_CONFIG ON) # For the config file, did we include z3 via a config (yes)

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
        # Debian and Ubuntu packages are as of Debian 14 and Ubuntu 24.04 very old,
        # such that no Cmake config files are provided. For the time being, we use a FindZ3.cmake script.
        find_package(Z3 QUIET 4.8.7)

        if (Z3_FOUND)

            set(STORM_HAVE_Z3 ON)
            set(STORM_Z3_CONFIG OFF) # For the config file, did we include z3 via a config (no)

            if ("${Z3_VERSION}" VERSION_EQUAL "4.10.1")
                message (WARNING "Storm cannot be linked against Z3 version 4.10.1, see issue 252.")
                message(WARNING "Storm - Could not link with z3. Building of Prism/JANI models will not be supported.")
                set(Z3_FOUND FALSE)
            else()
                # Check whether the version of z3 supports optimization
                message (STATUS "Storm - Linking with Z3 (version ${Z3_VERSION}). (Z3 version supports optimization)")

                add_library(z3 SHARED IMPORTED)
                set_target_properties(
                        z3
                        PROPERTIES
                        IMPORTED_LOCATION ${Z3_LIBRARIES}
                        INTERFACE_INCLUDE_DIRECTORIES ${Z3_INCLUDE_DIR}
                )
                list(APPEND STORM_DEP_IMP_TARGETS z3)
            endif()
        else()
            message (WARNING "Storm - Z3 not found. Building of Prism/JANI models will not be supported.")
        endif()
    endif()
else()
    message (WARNING "Storm - Not linking with Z3. Building of Prism/JANI models will not be supported.")
endif()

if (Z3_FOUND)
    # TODO We may remove STORM_HAVE_Z3_OPTIMIZE checks
    # TODO We may remove STORM_Z3_API_USES_STANDARD_INTEGERS checks
    set(STORM_Z3_API_USES_STANDARD_INTEGERS ON)
    set(STORM_HAVE_Z3_OPTIMIZE ON)
endif()

#############################################################
##
##	GLPK (optional)
##
#############################################################

if(NOT STORM_DISABLE_GLPK)
    set(STORM_HAVE_GLPK OFF)
    find_package(GLPK QUIET)

    if(GLPK_FOUND)
        set(STORM_HAVE_GLPK ON)
        message (STATUS "Storm - Linking with GLPK ${GLPK_VERSION_STRING}")

        add_library(glpk SHARED IMPORTED)
        set_target_properties(
                glpk
                PROPERTIES
                IMPORTED_LOCATION ${GLPK_LIBRARIES}
                INTERFACE_INCLUDE_DIRECTORIES ${GLPK_INCLUDE_DIR}
        )
        list(APPEND STORM_DEP_IMP_TARGETS glpk)
    else()
        message (WARNING "Storm - Not linking with GLPK.")
    endif()
else()
    message (STATUS "Storm - Not linking with GLPK.")
    set(STORM_HAVE_GLPK OFF)
endif()


#############################################################
##
##	Gurobi (optional)
##
#############################################################

if(NOT STORM_DISABLE_GUROBI)
    find_package(GUROBI QUIET)
    if(GUROBI_FOUND)
        set(STORM_HAVE_GUROBI ${GUROBI_FOUND})
        if (EXISTS ${GUROBI_LIBRARY})
            message (STATUS "Storm - Linking with Gurobi (${GUROBI_CXX_LIBRARY}).")
            add_library(GUROBI UNKNOWN IMPORTED)
            set_target_properties(
                    GUROBI
                    PROPERTIES
                    IMPORTED_LOCATION ${GUROBI_LIBRARY}
                    INTERFACE_INCLUDE_DIRECTORIES ${GUROBI_INCLUDE_DIRS}
            )
            list(APPEND STORM_DEP_IMP_TARGETS GUROBI)
        else()
            # The FindGurobi.cmake script needs to be updated every now and then as the library file contains the version number...
            message(FATAL_ERROR "Gurobi Library ${GUROBI_LIBRARY} not found. If your Gurobi Version is higher then 11.0.0, please contact the Storm developers.")
        endif()
    else()
        message (STATUS "Storm - Not linking with Gurobi. ")
        set(STORM_HAVE_GUROBI OFF)
    endif()
else()
    message (STATUS "Storm - Not linking with Gurobi. ")
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
# Set compile flags for dependencies
if(STORM_USE_CLN_EA OR STORM_USE_CLN_RF)
    set(SHIPPED_CARL_USE_CLN_NUMBERS ON)
    set(SHIPPED_CARL_USE_GINAC ON)
else()
    set(SHIPPED_CARL_USE_CLN_NUMBERS OFF)
    set(SHIPPED_CARL_USE_GINAC OFF)
endif()

set(STORM_HAVE_CARL OFF)
set(CARL_MINVERSION "14.30")
set(CARL_C14VERSION "14")

include(FetchContent)
FETCHCONTENT_DECLARE(
        carl
        GIT_REPOSITORY ${STORM_CARL_GIT_REPO}
        GIT_TAG ${STORM_CARL_GIT_TAG}
)
SET(EXCLUDE_TESTS_FROM_ALL ON)  #carl without tests
SET(THREAD_SAFE ON) #carl thread safe code
SET(USE_CLN_NUMBERS ${SHIPPED_CARL_USE_CLN_NUMBERS})
SET(USE_GINAC ${SHIPPED_CARL_USE_GINAC})
SET(PORTABLE ${STORM_PORTABLE})
SET(CARL_LIB_INSTALL_DIR "lib/storm/resources")
SET(CARL_INCLUDE_INSTALL_DIR "include/storm/resources") # maybe use resources folder /carl instead for consistency?
SET(CARL_EXPORT_TO_CMAKE OFF)
SET(CARL_CMAKE_INSTALL_DIR "lib/cmake/storm")
# We do not want to build or own version of ginac and cln, it adds levels of complexity that we currently do not want to handle.
SET(ALLOW_SHIPPED_CLN OFF)
SET(ALLOW_SHIPPED_GINAC OFF)
FETCHCONTENT_MAKEAVAILABLE(carl)
include(${carl_BINARY_DIR}/carlConfig.cmake)
# Read variables from this config.
set(STORM_HAVE_CLN ${CARL_HAVE_CLN})
set(STORM_HAVE_GINAC ${CARL_HAVE_GINAC})
message(STATUS "Storm - Using carl version ${carl_VERSION} from ${carl_SOURCE_DIR} (CLN: ${STORM_HAVE_CLN}, GiNaC: ${STORM_HAVE_GINAC})")
#
# TODO this old flag is no longer really relevant as CARL must always be present.
set(STORM_HAVE_CARL ON)
# This flag is currently required to pick the right carl adapter.
# TODO It can be removed as the old versions that did not support forward declarations are no longer supported either way.
set(STORM_CARL_SUPPORTS_FWD_DECL ON)
add_dependencies(storm_resources lib_carl)
list(APPEND STORM_FETCHED_TARGETS lib_carl)

# Some checks to see whether we correctly configured carl.
if(SHIPPED_CARL_USE_CLN_NUMBERS AND NOT STORM_HAVE_CLN)
    message(FATAL_ERROR "Cannot use CLN numbers. Consider installing {cln, ginac} or run cmake with STORM_USE_CLN_EA=OFF and STORM_USE_CLN_RF=OFF. Please also consult the documentation regarding dependencies for more information about prerequisites. ")
endif()
if(SHIPPED_CARL_USE_GINAC AND NOT STORM_HAVE_GINAC)
    message(FATAL_ERROR "Cannot use CLN numbers without ginac. Consider installing ginac  or run cmake with STORM_USE_CLN_EA=OFF and STORM_USE_CLN_RF=OFF. Please also consult the documentation regarding dependencies for more information about prerequisites.")
endif()

#############################################################
##
##	MathSAT (optional)
##
#############################################################

if(NOT STORM_DISABLE_MATHSAT)
    find_package(MATHSAT QUIET)
    if(MATHSAT_FOUND)
        set(STORM_HAVE_MATHSAT ON)
        add_library(mathsat SHARED IMPORTED)
        set_target_properties(
                mathsat
                PROPERTIES
                IMPORTED_LOCATION ${MATHSAT_LIBRARIES}
                INTERFACE_INCLUDE_DIRECTORIES ${MATHSAT_INCLUDE_DIRS}
        )
        list(APPEND STORM_DEP_IMP_TARGETS mathsat)
        message(STATUS "Storm - Linking with MathSAT: (library: ${mathsat_}; include: ${MATHSAT_INCLUDE_DIRS})")
    else()
        set(STORM_HAVE_MATHSAT OFF)
        if(MATHSAT_ROOT STREQUAL "")
            message(STATUS "Storm - MathSAT not found.")
        else()
            message(WARNING "Storm - MathSAT not found, even though MATHSAT_ROOT is set.")
        endif()
    endif()
else()
    set(STORM_HAVE_MATHSAT OFF)
    message(STATUS "Storm - Not linking with MathSat.")
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
##	Xerces (optional)
##
#############################################################

if(NOT STORM_DISABLE_XERCES)
    find_package(XercesC QUIET)
    if(TARGET XercesC::XercesC)
        set(STORM_HAVE_XERCES ON)
        message (STATUS "Storm (GSPN) - Linking with Xerces-c ${XercesC_VERSION}")
        list(APPEND STORM_GSPN_LINK_LIBRARIES XercesC::XercesC)
    else()
        set(STORM_HAVE_XERCES OFF)
        message (STATUS "Storm - Could not find Xerces, will disable parsing XML formats (for GSPNs)")
    endif()
else()
    set(STORM_HAVE_XERCES OFF)
    message (STATUS "Storm - Building without Xerces disables parsing XML formats (for GSPNs)")
endif()

#############################################################
##
##	SoPlex (optional)
##
#############################################################

if(NOT STORM_DISABLE_SOPLEX)
    find_package(soplex QUIET)

    IF(TARGET libsoplex-pic)
        set(STORM_HAVE_SOPLEX ON)
        get_target_property(soplexLOC libsoplex-pic LOCATION)
        get_target_property(soplexINCLUDE libsoplex-pic INTERFACE_INCLUDE_DIRECTORIES)
        MESSAGE(STATUS "Storm - Linking with SoPlex version ${soplex_VERSION}: (libary: ${soplexLOC}; include: ${soplexINCLUDE})")
        list(APPEND STORM_DEP_IMP_TARGETS libsoplex-pic)
    ELSE()
        set(STORM_HAVE_SOPLEX OFF)
        MESSAGE(STATUS "Storm - Not linking with SoPlex.")
    ENDIF()
else()
    set(STORM_HAVE_SOPLEX OFF)
    MESSAGE(STATUS "Storm - Not linking with SoPlex.")
endif()


#############################################################
##
##	Spot (optional)
##
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
set(SYLVAN_PORTABLE ${STORM_PORTABLE})
if(STORM_WARNING_AS_ERROR)
    # Ignore explicit warning in storm_wrapper.cpp
    add_compile_options(-Wno-cpp)
endif()
SET(SYLVAN_INSTALL_BINDIR ${STORM_RESOURCE_BIN_INSTALL_DIR}/sylvan)
SET(SYLVAN_INSTALL_LIBDIR ${STORM_RESOURCE_LIBRARY_INSTALL_DIR}/sylvan)
SET(SYLVAN_INSTALL_INCLUDEDIR ${STORM_RESOURCE_INCLUDE_INSTALL_DIR}/sylvan)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON) # Likely only necessary on linux
SET(STORM_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/") # TODO this is ugly.
SET(STORM_BINARY_INCLUDE_DIR "${PROJECT_BINARY_DIR}/include/") #TODO this is ugly.
FETCHCONTENT_MAKEAVAILABLE(sylvanfetch)

add_dependencies(storm_resources sylvan)
list(APPEND STORM_FETCHED_TARGETS sylvan)

#############################################################
##
##	Google Test gtest
##
#############################################################

if(STORM_BUILD_TESTS)
    set(GTEST_VERSION "1.17.0")
    FETCHCONTENT_DECLARE(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.zip
    )
    FETCHCONTENT_MAKEAVAILABLE(googletest)

    set(GTEST_LIBRARIES GTest::gtest_main GTest::gmock)
    add_dependencies(test-resources ${GTEST_LIBRARIES})
    list(APPEND STORM_TEST_LINK_LIBRARIES ${GTEST_LIBRARIES})
endif()

#############################################################
##
##	Threads
##
#############################################################

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
list(APPEND STORM_DEP_IMP_TARGETS Threads::Threads)

