# - Try to find Eigen3 lib
#
# This module supports requiring a minimum version, e.g. you can do
#   find_package(Eigen3 3.1.2)
# to require version 3.1.2 or newer of Eigen3.
#
# Once done this will define
#
#  STORMEIGEN3_FOUND - system has eigen lib with correct version
#  STORMEIGEN3_INCLUDE_DIR - the eigen include directory
#  STORMEIGEN3_VERSION - eigen version
#
# This module reads hints about search locations from 
# the following enviroment variables:
#
# STORMEIGEN3_ROOT
# STORMEIGEN3_ROOT_DIR

# Copyright (c) 2006, 2007 Montel Laurent, <montel@kde.org>
# Copyright (c) 2008, 2009 Gael Guennebaud, <g.gael@free.fr>
# Copyright (c) 2009 Benoit Jacob <jacob.benoit.1@gmail.com>
# Redistribution and use is allowed according to the terms of the 2-clause BSD license.

if(NOT Eigen3_FIND_VERSION)
  if(NOT Eigen3_FIND_VERSION_MAJOR)
    set(Eigen3_FIND_VERSION_MAJOR 2)
  endif(NOT Eigen3_FIND_VERSION_MAJOR)
  if(NOT Eigen3_FIND_VERSION_MINOR)
    set(Eigen3_FIND_VERSION_MINOR 91)
  endif(NOT Eigen3_FIND_VERSION_MINOR)
  if(NOT Eigen3_FIND_VERSION_PATCH)
    set(Eigen3_FIND_VERSION_PATCH 0)
  endif(NOT Eigen3_FIND_VERSION_PATCH)

  set(Eigen3_FIND_VERSION "${Eigen3_FIND_VERSION_MAJOR}.${Eigen3_FIND_VERSION_MINOR}.${Eigen3_FIND_VERSION_PATCH}")
endif(NOT Eigen3_FIND_VERSION)

macro(_eigen3_check_version)
  file(READ "${STORMEIGEN3_INCLUDE_DIR}/StormEigen/src/Core/util/Macros.h" _eigen3_version_header)

  string(REGEX MATCH "define[ \t]+STORMEIGEN_WORLD_VERSION[ \t]+([0-9]+)" _eigen3_world_version_match "${_eigen3_version_header}")
  set(STORMEIGEN3_WORLD_VERSION "${CMAKE_MATCH_1}")
  string(REGEX MATCH "define[ \t]+STORMEIGEN_MAJOR_VERSION[ \t]+([0-9]+)" _eigen3_major_version_match "${_eigen3_version_header}")
  set(STORMEIGEN3_MAJOR_VERSION "${CMAKE_MATCH_1}")
  string(REGEX MATCH "define[ \t]+STORMEIGEN_MINOR_VERSION[ \t]+([0-9]+)" _eigen3_minor_version_match "${_eigen3_version_header}")
  set(STORMEIGEN3_MINOR_VERSION "${CMAKE_MATCH_1}")

  set(STORMEIGEN3_VERSION ${STORMEIGEN3_WORLD_VERSION}.${STORMEIGEN3_MAJOR_VERSION}.${STORMEIGEN3_MINOR_VERSION})
  if(${STORMEIGEN3_VERSION} VERSION_LESS ${Eigen3_FIND_VERSION})
    set(STORMEIGEN3_VERSION_OK FALSE)
  else(${STORMEIGEN3_VERSION} VERSION_LESS ${Eigen3_FIND_VERSION})
    set(STORMEIGEN3_VERSION_OK TRUE)
  endif(${STORMEIGEN3_VERSION} VERSION_LESS ${Eigen3_FIND_VERSION})

  if(NOT STORMEIGEN3_VERSION_OK)

    message(STATUS "Eigen3 version ${STORMEIGEN3_VERSION} found in ${STORMEIGEN3_INCLUDE_DIR}, "
                   "but at least version ${Eigen3_FIND_VERSION} is required")
  endif(NOT STORMEIGEN3_VERSION_OK)
endmacro(_eigen3_check_version)

if (STORMEIGEN3_INCLUDE_DIR)

  # in cache already
  _eigen3_check_version()
  set(STORMEIGEN3_FOUND ${STORMEIGEN3_VERSION_OK})

else (STORMEIGEN3_INCLUDE_DIR)

  find_path(STORMEIGEN3_INCLUDE_DIR NAMES signature_of_eigen3_matrix_library
      HINTS
      ENV STORMEIGEN3_ROOT 
      ENV STORMEIGEN3_ROOT_DIR
      PATHS
      ${CMAKE_INSTALL_PREFIX}/include
      ${KDE4_INCLUDE_DIR}
      PATH_SUFFIXES eigen3 eigen
    )

  if(STORMEIGEN3_INCLUDE_DIR)
    _eigen3_check_version()
  endif(STORMEIGEN3_INCLUDE_DIR)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Eigen3 DEFAULT_MSG STORMEIGEN3_INCLUDE_DIR STORMEIGEN3_VERSION_OK)

  mark_as_advanced(STORMEIGEN3_INCLUDE_DIR)

endif(STORMEIGEN3_INCLUDE_DIR)

