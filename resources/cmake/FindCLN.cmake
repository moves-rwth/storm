# - Try to find libcln
# Once done this will define
#  CLN_FOUND - System has cln
#  CLN_INCLUDE_DIR - The cln include directory
#  CLN_LIBRARIES - The libraries needed to use cln
#  CLN_VERSION_STRING - The version of cln ("major.minor.patch")

# use pkg-config to get the directories and then use these values
# in the find_path() and find_library() calls
find_package(PkgConfig QUIET)
PKG_CHECK_MODULES(PC_CLN QUIET cln)

find_path(CLN_INCLUDE_DIR NAMES cln/cln.h
   HINTS
   ${PC_CLN_INCLUDEDIR}
   ${PC_CLN_INCLUDE_DIRS}
   PATH_SUFFIXES cln
   )

find_library(CLN_LIBRARIES NAMES cln
   HINTS
   ${PC_CLN_LIBDIR}
   ${PC_CLN_LIBRARY_DIRS}
   )

if(PC_CLN_VERSION)
    set(CLN_VERSION_STRING ${PC_CLN_VERSION})
elseif(CLN_INCLUDE_DIR AND EXISTS "${CLN_INCLUDE_DIR}/version.h")
    file(STRINGS "${CLN_INCLUDE_DIR}/version.h" cln_version
         REGEX "^#define[\t ]+CL_VERSION[\t ]+.+")
    string(REGEX REPLACE "^#define[\t ]+CL_VERSION[\t ]+(.+)" "\\1"
           CLN_VERSION_STRING "${cln_version}")
    unset(cln_version)
endif()

# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CLN
                                  REQUIRED_VARS CLN_LIBRARIES CLN_INCLUDE_DIR
                                  VERSION_VAR CLN_VERSION_STRING)

mark_as_advanced(CLN_INCLUDE_DIR CLN_LIBRARIES)
