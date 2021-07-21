# - Try to find libspot
# Once done this will define
#  SPOT_FOUND - System has glpk
#  SPOT_INCLUDE_DIR - The glpk include directory
#  SPOT_LIBRARIES - The libraries needed to use glpk
#  SPOT_VERSION - The version of spot

# use pkg-config to get the directories and then use these values

# in the find_path() and find_library() calls
find_package(PkgConfig QUIET)
PKG_CHECK_MODULES(PC_SPOT QUIET spot)

find_path(SPOT_INCLUDE_DIR NAMES spot/misc/_config.h
   HINTS
   ${PC_SPOT_INCLUDEDIR}
   ${PC_SPOT_INCLUDE_DIRS}
   )

find_library(SPOT_LIBRARIES NAMES spot
   HINTS
   ${PC_SPOT_LIBDIR}
   ${PC_SPOT_LIBRARY_DIRS}
   )

if(PC_SPOT_VERSION)
    set(SPOT_VERSION ${PC_SPOT_VERSION})
elseif(SPOT_INCLUDE_DIR AND EXISTS "${SPOT_INCLUDE_DIR}/spot/misc/_config.h")
    file(STRINGS "${SPOT_INCLUDE_DIR}/spot/misc/_config.h" SPOT_VERSION
         REGEX "^#define[\t ]+SPOT_VERSION[\t ]+\".+\"")
    string(REGEX REPLACE "^#define[\t ]+SPOT_VERSION[\t ]+\"(.+)\"" "\\1" SPOT_VERSION "${SPOT_VERSION}")
endif()

# handle the QUIETLY and REQUIRED arguments and set SPOT_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SPOT
                                  REQUIRED_VARS SPOT_LIBRARIES SPOT_INCLUDE_DIR
                                  VERSION_VAR SPOT_VERSION)

mark_as_advanced(SPOT_INCLUDE_DIR SPOT_LIBRARIES)
