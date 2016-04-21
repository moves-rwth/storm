# - Try to find libglpk
# Once done this will define
#  GLPK_FOUND - System has glpk
#  GLPK_INCLUDE_DIR - The glpk include directory
#  GLPK_LIBRARIES - The libraries needed to use glpk
#  GLPK_VERSION_STRING - The version of glpk ("major.minor")

# use pkg-config to get the directories and then use these values
# in the find_path() and find_library() calls
find_package(PkgConfig QUIET)
PKG_CHECK_MODULES(PC_GLPK QUIET glpk)

find_path(GLPK_INCLUDE_DIR NAMES glpk.h
   HINTS
   ${PC_GLPK_INCLUDEDIR}
   ${PC_GLPK_INCLUDE_DIRS}
   PATH_SUFFIXES glpk
   )

find_library(GLPK_LIBRARIES NAMES glpk
   HINTS
   ${PC_GLPK_LIBDIR}
   ${PC_GLPK_LIBRARY_DIRS}
   )

if(PC_GLPK_VERSION)
    set(GLPK_VERSION_STRING ${PC_GLPK_VERSION})
elseif(GLPK_INCLUDE_DIR AND EXISTS "${GLPK_INCLUDE_DIR}/glpk.h")
    file(STRINGS "${GLPK_INCLUDE_DIR}/glpk.h" glpk_major_version
         REGEX "^#define[\t ]+GLP_MAJOR_VERSION[\t ]+.+")
    file(STRINGS "${GLPK_INCLUDE_DIR}/glpk.h" glpk_minor_version
         REGEX "^#define[\t ]+GLP_MINOR_VERSION[\t ]+.+")
    string(REGEX REPLACE "^#define[\t ]+GLP_MAJOR_VERSION[\t ]+(.+)" "\\1"
           glpk_major_version "${glpk_major_version}")
    string(REGEX REPLACE "^#define[\t ]+GLP_MINOR_VERSION[\t ]+(.+)" "\\1"
           glpk_minor_version "${glpk_minor_version}")
	set(GLPK_VERSION_STRING "${glpk_major_version}.${glpk_minor_version}")
    unset(glpk_major_version)
    unset(glpk_minor_version)
endif()

# handle the QUIETLY and REQUIRED arguments and set GLPK_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLPK
                                  REQUIRED_VARS GLPK_LIBRARIES GLPK_INCLUDE_DIR
                                  VERSION_VAR GLPK_VERSION_STRING)

mark_as_advanced(GLPK_INCLUDE_DIR GLPK_LIBRARIES)
