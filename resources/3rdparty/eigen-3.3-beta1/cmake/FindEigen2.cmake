# - Try to find Eigen2 lib
#
# This module supports requiring a minimum version, e.g. you can do
#   find_package(Eigen2 2.0.3)
# to require version 2.0.3 to newer of Eigen2.
#
# Once done this will define
#
#  STORMEIGEN2_FOUND - system has eigen lib with correct version
#  STORMEIGEN2_INCLUDE_DIR - the eigen include directory
#  STORMEIGEN2_VERSION - eigen version

# Copyright (c) 2006, 2007 Montel Laurent, <montel@kde.org>
# Copyright (c) 2008, 2009 Gael Guennebaud, <g.gael@free.fr>
# Redistribution and use is allowed according to the terms of the BSD license.

if(NOT Eigen2_FIND_VERSION)
  if(NOT Eigen2_FIND_VERSION_MAJOR)
    set(Eigen2_FIND_VERSION_MAJOR 2)
  endif(NOT Eigen2_FIND_VERSION_MAJOR)
  if(NOT Eigen2_FIND_VERSION_MINOR)
    set(Eigen2_FIND_VERSION_MINOR 0)
  endif(NOT Eigen2_FIND_VERSION_MINOR)
  if(NOT Eigen2_FIND_VERSION_PATCH)
    set(Eigen2_FIND_VERSION_PATCH 0)
  endif(NOT Eigen2_FIND_VERSION_PATCH)

  set(Eigen2_FIND_VERSION "${Eigen2_FIND_VERSION_MAJOR}.${Eigen2_FIND_VERSION_MINOR}.${Eigen2_FIND_VERSION_PATCH}")
endif(NOT Eigen2_FIND_VERSION)

macro(_eigen2_check_version)
  file(READ "${STORMEIGEN2_INCLUDE_DIR}/StormEigen/src/Core/util/Macros.h" _eigen2_version_header)

  string(REGEX MATCH "define[ \t]+STORMEIGEN_WORLD_VERSION[ \t]+([0-9]+)" _eigen2_world_version_match "${_eigen2_version_header}")
  set(STORMEIGEN2_WORLD_VERSION "${CMAKE_MATCH_1}")
  string(REGEX MATCH "define[ \t]+STORMEIGEN_MAJOR_VERSION[ \t]+([0-9]+)" _eigen2_major_version_match "${_eigen2_version_header}")
  set(STORMEIGEN2_MAJOR_VERSION "${CMAKE_MATCH_1}")
  string(REGEX MATCH "define[ \t]+STORMEIGEN_MINOR_VERSION[ \t]+([0-9]+)" _eigen2_minor_version_match "${_eigen2_version_header}")
  set(STORMEIGEN2_MINOR_VERSION "${CMAKE_MATCH_1}")

  set(STORMEIGEN2_VERSION ${STORMEIGEN2_WORLD_VERSION}.${STORMEIGEN2_MAJOR_VERSION}.${STORMEIGEN2_MINOR_VERSION})
  if((${STORMEIGEN2_WORLD_VERSION} NOTEQUAL 2) OR (${STORMEIGEN2_MAJOR_VERSION} GREATER 10) OR (${STORMEIGEN2_VERSION} VERSION_LESS ${Eigen2_FIND_VERSION}))
    set(STORMEIGEN2_VERSION_OK FALSE)
  else()
    set(STORMEIGEN2_VERSION_OK TRUE)
  endif()

  if(NOT STORMEIGEN2_VERSION_OK)

    message(STATUS "Eigen2 version ${STORMEIGEN2_VERSION} found in ${STORMEIGEN2_INCLUDE_DIR}, "
                   "but at least version ${Eigen2_FIND_VERSION} is required")
  endif(NOT STORMEIGEN2_VERSION_OK)
endmacro(_eigen2_check_version)

if (STORMEIGEN2_INCLUDE_DIR)

  # in cache already
  _eigen2_check_version()
  set(STORMEIGEN2_FOUND ${STORMEIGEN2_VERSION_OK})

else (STORMEIGEN2_INCLUDE_DIR)

find_path(STORMEIGEN2_INCLUDE_DIR NAMES StormEigen/Core
     PATHS
     ${INCLUDE_INSTALL_DIR}
     ${KDE4_INCLUDE_DIR}
     PATH_SUFFIXES eigen2
   )

if(STORMEIGEN2_INCLUDE_DIR)
  _eigen2_check_version()
endif(STORMEIGEN2_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Eigen2 DEFAULT_MSG STORMEIGEN2_INCLUDE_DIR STORMEIGEN2_VERSION_OK)

mark_as_advanced(STORMEIGEN2_INCLUDE_DIR)

endif(STORMEIGEN2_INCLUDE_DIR)

