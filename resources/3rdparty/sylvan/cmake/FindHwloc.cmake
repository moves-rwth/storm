# Try to find HWLOC
# Once done this will define:
# - HWLOC_FOUND - True if the system has HWLOC
# - HWLOC_INCLUDE_DIRS - include directories for compiling
# - HWLOC_LIBRARIES - libraries for linking
# - HWLOC_DEFINITIONS - cflags suggested by pkg-config

find_package(PkgConfig)
pkg_check_modules(PC_HWLOC QUIET hwloc)

set(HWLOC_DEFINITIONS ${PC_HWLOC_FLAGS_OTHER})

find_path(HWLOC_INCLUDE_DIR hwloc.h
          HINTS ${PC_HWLOC_INCLUDEDIR} ${PC_HWLOC_INCLUDE_DIRS})

find_library(HWLOC_LIBRARIES NAMES hwloc
             HINTS ${PC_HWLOC_LIBDIR} ${PC_HWLOC_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set HWLOC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(HWLOC DEFAULT_MSG HWLOC_LIBRARIES HWLOC_INCLUDE_DIR)

mark_as_advanced(HWLOC_INCLUDE_DIR HWLOC_LIBRARIES)
