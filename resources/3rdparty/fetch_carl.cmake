
set(STORM_HAVE_CARL OFF)
set(CARL_MINVERSION "14.26")
set(CARL_C14VERSION "14")


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
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_PHOENIX_STL_TUPLE_H_")
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
#
#set(STORM_SHIPPED_CARL OFF)
#if(carl_FOUND AND NOT STORM_FORCE_SHIPPED_CARL)
#    get_target_property(carlLOCATION lib_carl LOCATION)
#    if("${carlLOCATION}" STREQUAL "carlLOCATION-NOTFOUND")
#        if(EXISTS ${STORM_3RDPARTY_BINARY_DIR}/carl)
#            message(WARNING "Storm - Library for carl location is not found but the directory ${STORM_3RDPARTY_BINARY_DIR}/carl exists. Will (re-)try to build a shipped version of carl.")
#            set(STORM_SHIPPED_CARL ON)
#        else()
#            message(FATAL_ERROR "Library location for carl is not found, did you build carl?")
#        endif()
#    elseif(EXISTS ${carlLOCATION})
#        #empty on purpose
#    else()
#        if(EXISTS ${STORM_3RDPARTY_BINARY_DIR}/carl)
#            message(WARNING "Storm - File ${carlLOCATION} does not exist but the directory ${STORM_3RDPARTY_BINARY_DIR}/carl exists. Will (re-)try to build a shipped version of carl.")
#            set(STORM_SHIPPED_CARL ON)
#        else()
#            message(FATAL_ERROR "File ${carlLOCATION} does not exist, did you build carl?")
#        endif()
#    endif()
#    if("${carl_VERSION_MAJOR}" STREQUAL "${CARL_C14VERSION}")
#        message(STATUS "Storm - Found carl-storm version")
#        # empty on purpose. Maybe put a warning here?
#        if("${carl_VERSION_MAJOR}.${carl_VERSION_MINOR}" VERSION_LESS "${CARL_MINVERSION}")
#            message(FATAL_ERROR "Carl version outdated. We require ${CARL_MINVERSION}. Found ${carl_VERSION_MAJOR}.${carl_VERSION_MINOR} at ${carlLOCATION}")
#        endif()
#    else()
#        message(FATAL_ERROR "We only support a diverged version of carl, indicated by Carl version 14.x. These versions can be found at https://github.com/moves-rwth/carl-storm.
#			      On this system, we found ${carl_VERSION_MAJOR}.${carl_VERSION_MINOR} at ${carlLOCATION}")
#    endif()
#
#    set(STORM_HAVE_CARL ON)
#    message(STATUS "Storm - Use system version of carl.")
#    message(STATUS "Storm - Linking with preinstalled carl ${carl_VERSION} (include: ${carl_INCLUDE_DIR}, library ${carl_LIBRARIES}, CARL_USE_CLN_NUMBERS: ${CARL_USE_CLN_NUMBERS}, CARL_USE_GINAC: ${CARL_USE_GINAC}).")
#    set(STORM_HAVE_CLN ${CARL_USE_CLN_NUMBERS})
#    set(STORM_HAVE_GINAC ${CARL_USE_GINAC})
#else()
#    set(STORM_SHIPPED_CARL ON)
#endif()

#if (STORM_SHIPPED_CARL)
include(FetchContent)
FETCHCONTENT_DECLARE(
        carl
        SOURCE_DIR /Users/junges/carl
        #GIT_REPOSITORY https://github.com/sjunges/carl-storm.git
        #GIT_TAG  cmakeupdates
)
SET(EXCLUDE_TESTS_FROM_ALL ON)
SET(CARL_COMPILE_RELEASE ON)
SET(THREAD_SAFE ON)
SET(Boost_NO_SYSTEM_PATHS ON)
SET(BOOST_INCLUDEDIR ${Boost_INCLUDE_DIRS})
SET(BOOST_LIBRARYDIR ${Boost_LIBRARY_DIRS})
SET(CARL_LIB_INSTALL_DIR "lib/storm")
SET(CARL_INCLUDE_INSTALL_DIR "include/storm")
FETCHCONTENT_MAKEAVAILABLE(carl)
message(STATUS ${carl_BINARY_DIR})
# TODO


include(${carl_BINARY_DIR}/carlConfig.cmake)
#
set(STORM_HAVE_CLN ${CARL_USE_CLN_NUMBERS})
message(STATUS "carl version ${carl_VERSION} use cln: ${STORM_HAVE_CLN}")
set(STORM_HAVE_GINAC ${CARL_USE_GINAC})
#
add_dependencies(storm_resources lib_carl)
set(STORM_HAVE_CARL ON)
#
#    message(STATUS "Storm - Linking with shipped carl ${carl_VERSION} (include: ${carl_INCLUDE_DIR}, library ${carl_LIBRARIES}, CARL_USE_CLN_NUMBERS: ${CARL_USE_CLN_NUMBERS}, CARL_USE_GINAC: ${CARL_USE_GINAC}).")
#
#    # install the carl dynamic library if we built it
#    if(MACOSX)
#        install(FILES ${STORM_3RDPARTY_BINARY_DIR}/carl/lib/libcarl.${carl_VERSION}${DYNAMIC_EXT} DESTINATION lib)
#    else()
#        install(FILES ${STORM_3RDPARTY_BINARY_DIR}/carl/lib/libcarl${DYNAMIC_EXT}.${carl_VERSION} DESTINATION lib)
#    endif()
#endif()

if("${carl_VERSION_MAJOR}.${carl_VERSION_MINOR}" VERSION_EQUAL "14.22")
    # This version is too old for forward declarations and updating requires moving the git,
    # so we warn users but start warning them now.
    set(STORM_CARL_SUPPORTS_FWD_DECL OFF)
    message(WARNING "Uses an outdated repo for Carl. Carl is now hosted at https://github.com/moves-rwth/carl-storm")
elseif("${carl_VERSION_MAJOR}.${carl_VERSION_MINOR}" VERSION_EQUAL "14.23")
    # This version is too old for forward declarations, but we keep supporting it for the moment.
    set(STORM_CARL_SUPPORTS_FWD_DECL OFF)
else()
    set(STORM_CARL_SUPPORTS_FWD_DECL ON)
endif()


# The library that needs symbols must be first, then the library that resolves the symbol.
list(APPEND STORM_DEP_IMP_TARGETS lib_carl)
list(APPEND STORM_DEP_IMP_TARGETS GMPXX_SHARED GMP_SHARED)

