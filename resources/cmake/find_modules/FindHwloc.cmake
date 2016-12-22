# Try to find hwloc libraries and headers.
#
# Usage of this module:
#
#  find_package(hwloc)
#
# Variables defined by this module:
#
#  HWLOC_FOUND              System has hwloc libraries and headers
#  HWLOC_LIBRARIES          The hwloc library
#  HWLOC_INCLUDE_DIRS       The location of HWLOC headers

find_path(
  HWLOC_PREFIX
  NAMES include/hwloc.h
)

if (NOT HWLOC_PREFIX AND NOT $ENV{HWLOC_BASE} STREQUAL "")
  set(HWLOC_PREFIX $ENV{HWLOC_BASE})
endif()


find_library(
  HWLOC_LIBRARIES
  NAMES hwloc
  HINTS ${HWLOC_PREFIX}/lib
)

find_path(
  HWLOC_INCLUDE_DIRS
  NAMES hwloc.h
  HINTS ${HWLOC_PREFIX}/include
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  HWLOC DEFAULT_MSG
  HWLOC_LIBRARIES
  HWLOC_INCLUDE_DIRS
)

mark_as_advanced(
  HWLOC_LIBRARIES
  HWLOC_INCLUDE_DIRS
)

if (HWLOC_FOUND)
  if (NOT $ENV{HWLOC_LIB} STREQUAL "")
# set(HWLOC_LIBRARIES "$ENV{HWLOC_LIB}")
  endif()
endif()