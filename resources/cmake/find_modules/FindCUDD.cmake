# - Try to find libglpk
# Once done this will define
#  CUDD_FOUND - System has cudd
#  CUDD_INCLUDE_DIR - The cudd include directory
#  CUDD_LIBRARIES - The libraries needed to use cudd
#  CUDD_VERSION_STRING - The version of cudd ("major.minor.release")

# use pkg-config to get the directories and then use these values
# in the find_path() and find_library() calls
find_package(PkgConfig QUIET)
PKG_CHECK_MODULES(PC_CUDD QUIET cudd)

find_path(CUDD_INCLUDE_DIR NAMES cudd.h
   HINTS
   ${PC_CUDD_INCLUDEDIR}
   ${PC_CUDD_INCLUDE_DIRS}
   PATH_SUFFIXES cudd
   )

find_library(CUDD_LIBRARIES NAMES cudd
   HINTS
   ${PC_CUDD_LIBDIR}
   ${PC_CUDD_LIBRARY_DIRS}
   )

if(PC_CUDD_VERSION)
    set(CUDD_VERSION_STRING ${PC_CUDD_VERSION})
elseif(CUDD_INCLUDE_DIR AND EXISTS "${CUDD_INCLUDE_DIR}/cudd.h")
    file(STRINGS "${CUDD_INCLUDE_DIR}/cudd.h" cudd_version
         REGEX "^#define[\t ]+CUDD_VERSION[\t ]+\".+\"")
    string(REGEX REPLACE "^#define[\t ]+CUDD_VERSION[\t ]+\"(.+)\"" "\\1"
           CUDD_VERSION_STRING "${cudd_version}")
    unset(cudd_version)
endif()

# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CUDD
                                  REQUIRED_VARS CUDD_LIBRARIES CUDD_INCLUDE_DIR
                                  VERSION_VAR CUDD_VERSION_STRING)

mark_as_advanced(CUDD_INCLUDE_DIR CUDD_LIBRARIES)
