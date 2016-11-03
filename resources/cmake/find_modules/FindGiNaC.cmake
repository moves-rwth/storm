# - Try to find libginac
# Once done this will define
#  GINAC_FOUND - System has ginac
#  GINAC_INCLUDE_DIR - The ginac include directory
#  GINAC_LIBRARIES - The libraries needed to use ginac
#  GINAC_VERSION_STRING - The version of ginac ("major.minor.micro")

# use pkg-config to get the directories and then use these values
# in the find_path() and find_library() calls
find_package(PkgConfig QUIET)
PKG_CHECK_MODULES(PC_GINAC QUIET ginac)

find_path(GINAC_INCLUDE_DIR NAMES ginac.h
   HINTS
   ${PC_GINAC_INCLUDEDIR}
   ${PC_GINAC_INCLUDE_DIRS}
   PATH_SUFFIXES ginac
   )

find_library(GINAC_LIBRARIES NAMES ginac
   HINTS
   ${PC_GINAC_LIBDIR}
   ${PC_GINAC_LIBRARY_DIRS}
   )

if(PC_GINAC_VERSION)
    set(GINAC_VERSION_STRING ${PC_GINAC_VERSION})
elseif(GINAC_INCLUDE_DIR AND EXISTS "${GINAC_INCLUDE_DIR}/ginac.h")
    file(STRINGS "${GINAC_INCLUDE_DIR}/ginac.h" ginac_major_version
         REGEX "^#define[\t ]+GINACLIB_MAJOR_VERSION[\t ]+.+")
    file(STRINGS "${GINAC_INCLUDE_DIR}/ginac.h" ginac_minor_version
         REGEX "^#define[\t ]+GINACLIB_MINOR_VERSION[\t ]+.+")
    file(STRINGS "${GINAC_INCLUDE_DIR}/ginac.h" ginac_micro_version
         REGEX "^#define[\t ]+GINACLIB_MICRO_VERSION[\t ]+.+")
    string(REGEX REPLACE "^#define[\t ]+GINACLIB_MAJOR_VERSION[\t ]+(.+)" "\\1"
           ginac_major_version "${ginac_major_version}")
    string(REGEX REPLACE "^#define[\t ]+GINACLIB_MINOR_VERSION[\t ]+(.+)" "\\1"
           ginac_minor_version "${ginac_minor_version}")
    string(REGEX REPLACE "^#define[\t ]+GINACLIB_MICRO_VERSION[\t ]+(.+)" "\\1"
           ginac_micro_version "${ginac_micro_version}")
	set(GINAC_VERSION_STRING "${ginac_major_version}.${ginac_minor_version}.${ginac_micro_version}")
    unset(ginac_major_version)
    unset(ginac_minor_version)
    unset(ginac_micro_version)
endif()

# handle the QUIETLY and REQUIRED arguments and set GINAC_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GINAC
                                  REQUIRED_VARS GINAC_LIBRARIES GINAC_INCLUDE_DIR
                                  VERSION_VAR GINAC_VERSION_STRING)

mark_as_advanced(GINAC_INCLUDE_DIR GINAC_LIBRARIES)
