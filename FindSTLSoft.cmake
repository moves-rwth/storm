# Locate the STLSoft Headers.
#
# Defines the following variables:
#
#   STLSOFT_FOUND - Found the Pantheios Logging Framework
#   STLSOFT_INCLUDE_DIRS - Include directories
#
# Accepts the following variables as input:
#
#   STLSOFT_ROOT - (as a CMake or environment variable)
#                The root directory of the STLSoft install prefix
#
#  

#=============================================================================
# Copyright 2012 Philipp Berger <admin@philippberger.de>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)
#




if(STLSOFT_INCLUDE_DIR)
	if (NOT STLSOFT_ROOT)
		get_filename_component(STLSOFT_ROOT "${STLSOFT_INCLUDE_DIR}" PATH)
	endif()
	
	get_filename_component(STLSOFT_ROOT_HINT "${STLSOFT_INCLUDE_DIR}" PATH)
endif()

find_path(STLSOFT_INCLUDE_DIR stlsoft/stlsoft.h
    PATH_SUFFIXES include
	HINTS ${STLSOFT_ROOT} ${STLSOFT_ROOT_HINT}
	ENV STLSOFT_ROOT
)
set(STLSOFT_INCLUDE_DIRS ${STLSOFT_INCLUDE_DIR})

# handle the QUIETLY and REQUIRED arguments and set STLSOFT_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(STLSoft
	REQUIRED_VARS STLSOFT_INCLUDE_DIR
)
								  

mark_as_advanced(STLSOFT_INCLUDE_DIR)

