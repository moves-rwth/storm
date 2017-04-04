# Try to find GMP
# Once done this will define:
# - GMP_FOUND - True if the system has GMP
# - GMP_INCLUDE_DIRS - include directories for compiling
# - GMP_LIBRARIES - libraries for linking
# - GMP_DEFINITIONS - cflags suggested by pkg-config

find_package(PkgConfig)
pkg_check_modules(PC_GMP QUIET gmp)

set(GMP_DEFINITIONS ${PC_GMP_CFLAGS_OTHER})

find_path(GMP_INCLUDE_DIR gmp.h
          HINTS ${PC_GMP_INCLUDEDIR} ${PC_GMP_INCLUDE_DIRS})

find_library(GMP_LIBRARIES NAMES gmp libgmp
             HINTS ${PC_GMP_LIBDIR} ${PC_GMP_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GMP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_LIBRARIES GMP_INCLUDE_DIR)

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARIES)
