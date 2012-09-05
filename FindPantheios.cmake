# Locate the Pantheois Logging Framework.
#
# Defines the following variables:
#
#   PANTHEIOS_FOUND - Found the Pantheios Logging Framework
#   PANTHEIOS_INCLUDE_DIRS - Include directories
#
# Accepts the following variables as input:
#
#   PANTHEIOS_ROOT - (as a CMake or environment variable)
#                The root directory of the pantheios install prefix
#
#   PANTHEIOS_USE_DYNAMIC_RUNTIME
#
# If you want to use splitting, specify LRSplit and than preface the components with L and R, so e.g. LRSplit LFile RSyslog
# To use more than one BackEnd, specify NBackEnd followed by a list of components. NBackEnd requires the NFrontEnd.
#
# Possible Components for BackEnd:
# ACELogger
# COMErrorObject
# File
# FileCallback
# FPrintf
# FPrintfCallback
# Null
# Speech
# Syslog
# WindowsConsole
# WindowsConsoleCallback
# WindowsDebugger
# WindowsDebuggerCallback
# WindowsEventLog
# WindowsMessageBox
# WindowsSyslog
# WindowsSyslogCallback
#
# Possible components for FrontEnd:
# NoFrontEnd
# SimpleFrontEnd
# NFrontEnd
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
# Credits:
# 
# HUGE thanks to Michael Wild
# Additional thanks to:
# Mateusz Loskot
# Rolf Eike Beer



# default for WideString option
set(PANTHEIOS_WIDESTRING 0)
# default for Front- and BackEnd
set(PANTHEIOS_FRONTEND "simple")
set(PANTHEIOS_BACKEND "file")
set(PANTHEIOS_BACKEND_L OFF)
set(PANTHEIOS_BACKEND_R OFF)
set(PANTHEIOS_BACKEND_LIST)

# Use FIND_PACKAGE( Pantheios COMPONENTS ... ) to enable modules
if( Pantheios_FIND_COMPONENTS )
	list(FIND Pantheios_FIND_COMPONENTS "LRSplit" PANTHEIOS_use_lrsplit)
	list(FIND Pantheios_FIND_COMPONENTS "NFrontEnd" PANTHEIOS_use_nfe)
	list(FIND Pantheios_FIND_COMPONENTS "NBackEnd" PANTHEIOS_use_nbe)
	list(FIND Pantheios_FIND_COMPONENTS "WideString" PANTHEIOS_use_ws)
	
	list(REMOVE_ITEM Pantheios_FIND_COMPONENTS "LRSplit" "NFrontEnd" "NBackEnd" "WideString")
	
	if (NOT PANTHEIOS_use_ws EQUAL -1)
		# Use WideString
		set(PANTHEIOS_WIDESTRING 1)
	endif()
	
	if (NOT PANTHEIOS_use_lrsplit EQUAL -1)
		# Found LRSplit
		set(PANTHEIOS_BACKEND "lrsplit")
		if (NOT PANTHEIOS_use_nbe EQUAL -1)
			# Also found NBe
			message(FATAL_ERROR "Pantheios: Use either LRSplit or NBackEnd, not both.")
		endif()
		if (NOT PANTHEIOS_use_nfe EQUAL -1)
			# Also found NFe
			message(FATAL_ERROR "Pantheios: Use either LRSplit or NFrontEnd, not both.")
		endif()
		
		foreach( component ${Pantheios_FIND_COMPONENTS} )
			# LRSplit L BackEnds
			string(SUBSTRING ${component} 0 1 _sub_comp_head)
			string(LENGTH ${component} _length_comp)
			math(EXPR _length_comp_tail "${_length_comp} - 1")
			string(SUBSTRING ${component} 1 ${_length_comp_tail} _sub_comp_tail)
			
			if ((_sub_comp_tail STREQUAL "ACELogger") OR (_sub_comp_tail STREQUAL "COMErrorObject") OR (_sub_comp_tail STREQUAL "File") OR (_sub_comp_tail STREQUAL "FileCallback") OR (_sub_comp_tail STREQUAL "FPrintf") OR (_sub_comp_tail STREQUAL "FPrintfCallback") OR (_sub_comp_tail STREQUAL "Null") OR (_sub_comp_tail STREQUAL "Speech") OR (_sub_comp_tail STREQUAL "Syslog") OR (_sub_comp_tail STREQUAL "WindowsConsole") OR (_sub_comp_tail STREQUAL "WindowsConsoleCallback") OR (_sub_comp_tail STREQUAL "WindowsDebugger") OR (_sub_comp_tail STREQUAL "WindowsDebuggerCallback") OR (_sub_comp_tail STREQUAL "WindowsEventLog") OR (_sub_comp_tail STREQUAL "WindowsMessageBox") OR (_sub_comp_tail STREQUAL "WindowsSyslog") OR (_sub_comp_tail STREQUAL "WindowsSyslogCallback"))
				if ((_sub_comp_head STREQUAL L) OR (_sub_comp_head STREQUAL R))
					message(STATUS "Pantheios: Setting LRSplit BackEnd ${_sub_comp_head} to ${_sub_comp_tail}")
					string(TOLOWER ${_sub_comp_tail} _sub_comp_tail_low)
					set(PANTHEIOS_BACKEND_${_sub_comp_head} "${_sub_comp_tail_low}")
				else ()
					message(FATAL_ERROR "Pantheios: Internal Parsing Error")
				endif()
			
			# FrontEnds
			elseif (component STREQUAL "NoFrontEnd")
				message(STATUS "Pantheios: Setting FrontEnd to NoFrontEnd")
				set(PANTHEIOS_FRONTEND "null")
			elseif (component STREQUAL "SimpleFrontEnd")
				message(STATUS "Pantheios: Setting FrontEnd to SimpleFrontEnd")
				set(PANTHEIOS_FRONTEND "simple")		

			else ()
				message(FATAL_ERROR "Unknown Component: ${component}")
			endif ()
		endforeach(component)
	elseif (NOT PANTHEIOS_use_nbe EQUAL -1)
		# Found NBackEnd
		if (PANTHEIOS_use_nfe EQUAL -1)
			message(FATAL_ERROR "Pantheios: Usage of NBackEnd requires the NFrontEnd.")
		endif()
		set(PANTHEIOS_BACKEND "N")
		set(PANTHEIOS_FRONTEND "N")
		
		foreach( component ${Pantheios_FIND_COMPONENTS} )
			# Std BackEnds
			if ((component STREQUAL "ACELogger") OR (component STREQUAL "COMErrorObject") OR (component STREQUAL "File") OR (component STREQUAL "FileCallback") OR (component STREQUAL "FPrintf") OR (component STREQUAL "FPrintfCallback") OR (component STREQUAL "Null") OR (component STREQUAL "Speech") OR (component STREQUAL "Syslog") OR (component STREQUAL "WindowsConsole") OR (component STREQUAL "WindowsConsoleCallback") OR (component STREQUAL "WindowsDebugger") OR (component STREQUAL "WindowsDebuggerCallback") OR (component STREQUAL "WindowsEventLog") OR (component STREQUAL "WindowsMessageBox") OR (component STREQUAL "WindowsSyslog") OR (component STREQUAL "WindowsSyslogCallback"))
				message(STATUS "Pantheios: Adding BackEnd ${component}")
				string(TOLOWER ${component} _low_comp)
				list(APPEND PANTHEIOS_BACKEND_LIST ${_low_comp})
			else ()
				message(FATAL_ERROR "Unknown Component: ${component}")
			endif ()
		endforeach(component)
	else ()
		# Simple, one FE, one BE
		foreach( component ${Pantheios_FIND_COMPONENTS} )
			if ((component STREQUAL "ACELogger") OR (component STREQUAL "COMErrorObject") OR (component STREQUAL "File") OR (component STREQUAL "FileCallback") OR (component STREQUAL "FPrintf") OR (component STREQUAL "FPrintfCallback") OR (component STREQUAL "Null") OR (component STREQUAL "Speech") OR (component STREQUAL "Syslog") OR (component STREQUAL "WindowsConsole") OR (component STREQUAL "WindowsConsoleCallback") OR (component STREQUAL "WindowsDebugger") OR (component STREQUAL "WindowsDebuggerCallback") OR (component STREQUAL "WindowsEventLog") OR (component STREQUAL "WindowsMessageBox") OR (component STREQUAL "WindowsSyslog") OR (component STREQUAL "WindowsSyslogCallback"))
				message(STATUS "Pantheios: Setting BackEnd to ${component}")
				string(TOLOWER ${component} _low_comp)
				set(PANTHEIOS_BACKEND ${_low_comp})				
				
			# FrontEnds
			elseif (component STREQUAL "NoFrontEnd")
				message(STATUS "Pantheios: Setting FrontEnd to NoFrontEnd")
				set(PANTHEIOS_FRONTEND "null")
			elseif (component STREQUAL "SimpleFrontEnd")
				message(STATUS "Pantheios: Setting FrontEnd to SimpleFrontEnd")
				set(PANTHEIOS_FRONTEND "simple")			
			else ()
				message(FATAL_ERROR "Unknown Component: ${component}")
			endif ()
		endforeach(component)
	endif ()
endif(Pantheios_FIND_COMPONENTS)

if (PANTHEIOS_USE_DYNAMIC_RUNTIME)
	set(PANTHEIOS_LIB_LINKTYPE "dll")
else ()
	set(PANTHEIOS_LIB_LINKTYPE "mt")
endif ()

if(PANTHEIOS_INCLUDE_DIR)
	if (NOT PANTHEIOS_ROOT)
		get_filename_component(PANTHEIOS_ROOT "${PANTHEIOS_INCLUDE_DIR}" PATH)
	endif()
	
	get_filename_component(PANTHEIOS_ROOT_HINT "${PANTHEIOS_INCLUDE_DIR}" PATH)
endif()

find_path(PANTHEIOS_INCLUDE_DIR pantheios/pantheios.h
    PATH_SUFFIXES include
	HINTS ${PANTHEIOS_ROOT} ${PANTHEIOS_ROOT_HINT}
	ENV PANTHEIOS_ROOT
)

# No idea what the stuff for ICC et. al. is, so I don't handle it here...
set(_P_COMP_TAG)
set(_P_OS_TAG)
set(_P_ARCH_TAG)
if(MSVC)
	if(MSVC60)
		set(_P_COMP_TAG vc6)
	elseif(MSVC70)
		set(_P_COMP_TAG vc7)
	elseif(MSVC71)
		set(_P_COMP_TAG vc71)
	elseif(MSVC80)
		set(_P_COMP_TAG vc8)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(_P_ARCH_TAG .x64)
		endif()
	elseif(MSVC90)
		set(_P_COMP_TAG vc9)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(_P_ARCH_TAG .x64)
		endif()
	elseif(MSVC10)
		set(_P_COMP_TAG vc10)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(_P_ARCH_TAG .x64)
		endif()
	elseif(MSVC11)
		set(_P_COMP_TAG vc11)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(_P_ARCH_TAG .x64)
		endif()
	endif()
elseif(CMAKE_COMPILER_IS_GNUCC)
	execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
    list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
    list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)
	set(_P_COMP_TAG gcc${GCC_MAJOR}${GCC_MINOR})
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_P_ARCH_TAG .file64bit)
	endif()
else()
	message(FATAL_ERROR "Pantheios: Your compiler/environment is currently unsupported.")
endif()

set(_P_LIB_TAG ${_P_COMP_TAG}${_P_OS_TAG}${_P_ARCH_TAG})

# Is this the right way?
set(PANTHEIOS_INCLUDE_DIRS ${PANTHEIOS_INCLUDE_DIR})

set(_P_REQUIRED_LIBVARS)
set(PANTHEIOS_LIBRARIES)
set(PANTHEIOS_LIBRARIES_DEBUG)
set(PANTHEIOS_LIBRARIES_RELEASE)


#core and util libraries
foreach(l core util)
    find_library(PANTHEIOS_${l}_${PANTHEIOS_LIB_LINKTYPE}_DEBUG_LIBRARY
        pantheios.1.${l}.${_P_LIB_TAG}.${PANTHEIOS_LIB_LINKTYPE}.debug
        PATH_SUFFIXES lib
        HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
        ENV PANTHEIOS_ROOT
    )

    find_library(PANTHEIOS_${l}_${PANTHEIOS_LIB_LINKTYPE}_LIBRARY
        pantheios.1.${l}.${_P_LIB_TAG}.${PANTHEIOS_LIB_LINKTYPE}
        PATH_SUFFIXES lib
        HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
        ENV PANTHEIOS_ROOT
    )

    list(APPEND _P_REQUIRED_LIBVARS
        PANTHEIOS_${l}_${PANTHEIOS_LIB_LINKTYPE}_DEBUG_LIBRARY
        PANTHEIOS_${l}_${PANTHEIOS_LIB_LINKTYPE}_LIBRARY
    )
    list(APPEND PANTHEIOS_LIBRARIES
        debug ${PANTHEIOS_${l}_${PANTHEIOS_LIB_LINKTYPE}_DEBUG_LIBRARY}
        optimized ${PANTHEIOS_${l}_${PANTHEIOS_LIB_LINKTYPE}_LIBRARY}
    )
endforeach()

# set PANTHEIOS_LIBRARY_DIRS
get_filename_component(PANTHEIOS_LIBRARY_DIRS ${PANTHEIOS_core_${PANTHEIOS_LIB_LINKTYPE}_LIBRARY} PATH)



# backend libraries (split, sole, local, remote and common)
set(_P_LT ${PANTHEIOS_LIB_LINKTYPE})
set(_P_BE ${PANTHEIOS_BACKEND})

find_library(PANTHEIOS_be_${_P_BE}_${_P_LT}_DEBUG_LIBRARY
	pantheios.1.be.${_P_BE}.${_P_LIB_TAG}.${_P_LT}.debug
	PATH_SUFFIXES lib
	HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
	ENV PANTHEIOS_ROOT
)

find_library(PANTHEIOS_be_${_P_BE}_${_P_LT}_LIBRARY
	pantheios.1.be.${_P_BE}.${_P_LIB_TAG}.${_P_LT}
	PATH_SUFFIXES lib
	HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
	ENV PANTHEIOS_ROOT
)

list(APPEND _P_REQUIRED_LIBVARS
	PANTHEIOS_be_${_P_BE}_${_P_LT}_DEBUG_LIBRARY
	PANTHEIOS_be_${_P_BE}_${_P_LT}_LIBRARY
)
list(APPEND PANTHEIOS_LIBRARIES
	debug ${PANTHEIOS_be_${_P_BE}_${_P_LT}_DEBUG_LIBRARY}
	optimized ${PANTHEIOS_be_${_P_BE}_${_P_LT}_LIBRARY}
)

if (_P_BE STREQUAL N)
	# N Backend, go through list
	message(STATUS "Pantheios: Dbg: Lib-n")
	
	foreach (blib PANTHEIOS_BACKEND_LIST)
		find_library(PANTHEIOS_bec_${blib}_${_P_LT}_DEBUG_LIBRARY
			pantheios.1.bec.${blib}.${_P_LIB_TAG}.${_P_LT}.debug
			PATH_SUFFIXES lib
			HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
			ENV PANTHEIOS_ROOT
		)

		find_library(PANTHEIOS_bec_${blib}_${_P_LT}_LIBRARY
			pantheios.1.bec.${blib}.${_P_LIB_TAG}.${_P_LT}
			PATH_SUFFIXES lib
			HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
			ENV PANTHEIOS_ROOT
		)

		list(APPEND _P_REQUIRED_LIBVARS
			PANTHEIOS_bec_${blib}_${_P_LT}_DEBUG_LIBRARY
			PANTHEIOS_bec_${blib}_${_P_LT}_LIBRARY
		)
		list(APPEND PANTHEIOS_LIBRARIES
			debug ${PANTHEIOS_bec_${blib}_${_P_LT}_DEBUG_LIBRARY}
			optimized ${PANTHEIOS_bec_${blib}_${_P_LT}_LIBRARY}
		)
	endforeach()
elseif (_P_BE STREQUAL lrsplit)
	# LRSplit
	message(STATUS "Pantheios: Dbg: Lib-lrsplit")
	
	# Left side
	foreach (t bec bel)
		find_library(PANTHEIOS_${t}_${PANTHEIOS_BACKEND_L}_${_P_LT}_DEBUG_LIBRARY
			pantheios.1.${t}.${PANTHEIOS_BACKEND_L}.${_P_LIB_TAG}.${_P_LT}.debug
			PATH_SUFFIXES lib
			HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
			ENV PANTHEIOS_ROOT
		)

		find_library(PANTHEIOS_${t}_${PANTHEIOS_BACKEND_L}_${_P_LT}_LIBRARY
			pantheios.1.${t}.${PANTHEIOS_BACKEND_L}.${_P_LIB_TAG}.${_P_LT}
			PATH_SUFFIXES lib
			HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
			ENV PANTHEIOS_ROOT
		)
		list(APPEND _P_REQUIRED_LIBVARS
			PANTHEIOS_${t}_${PANTHEIOS_BACKEND_L}_${_P_LT}_DEBUG_LIBRARY
			PANTHEIOS_${t}_${PANTHEIOS_BACKEND_L}_${_P_LT}_LIBRARY
		)
		list(APPEND PANTHEIOS_LIBRARIES
			debug ${PANTHEIOS_${t}_${PANTHEIOS_BACKEND_L}_${_P_LT}_DEBUG_LIBRARY}
			optimized ${PANTHEIOS_${t}_${PANTHEIOS_BACKEND_L}_${_P_LT}_LIBRARY}
		)
	endforeach()
	# Right side
	foreach (t bec ber)
		find_library(PANTHEIOS_${t}_${PANTHEIOS_BACKEND_R}_${_P_LT}_DEBUG_LIBRARY
			pantheios.1.${t}.${PANTHEIOS_BACKEND_R}.${_P_LIB_TAG}.${_P_LT}.debug
			PATH_SUFFIXES lib
			HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
			ENV PANTHEIOS_ROOT
		)

		find_library(PANTHEIOS_${t}_${PANTHEIOS_BACKEND_R}_${_P_LT}_LIBRARY
			pantheios.1.${t}.${PANTHEIOS_BACKEND_R}.${_P_LIB_TAG}.${_P_LT}
			PATH_SUFFIXES lib
			HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
			ENV PANTHEIOS_ROOT
		)
		list(APPEND _P_REQUIRED_LIBVARS
			PANTHEIOS_${t}_${PANTHEIOS_BACKEND_R}_${_P_LT}_DEBUG_LIBRARY
			PANTHEIOS_${t}_${PANTHEIOS_BACKEND_R}_${_P_LT}_LIBRARY
		)
		list(APPEND PANTHEIOS_LIBRARIES
			debug ${PANTHEIOS_${t}_${PANTHEIOS_BACKEND_R}_${_P_LT}_DEBUG_LIBRARY}
			optimized ${PANTHEIOS_${t}_${PANTHEIOS_BACKEND_R}_${_P_LT}_LIBRARY}
		)
	endforeach()
else ()
	# normal
	message(STATUS "Pantheios: Dbg: Lib-normal")
	foreach (t bec)
		find_library(PANTHEIOS_${t}_${PANTHEIOS_BACKEND}_${_P_LT}_DEBUG_LIBRARY
			pantheios.1.${t}.${PANTHEIOS_BACKEND}.${_P_LIB_TAG}.${_P_LT}.debug
			PATH_SUFFIXES lib
			HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
			ENV PANTHEIOS_ROOT
		)

		find_library(PANTHEIOS_${t}_${PANTHEIOS_BACKEND}_${_P_LT}_LIBRARY
			pantheios.1.${t}.${PANTHEIOS_BACKEND}.${_P_LIB_TAG}.${_P_LT}
			PATH_SUFFIXES lib
			HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
			ENV PANTHEIOS_ROOT
		)
		list(APPEND _P_REQUIRED_LIBVARS
			PANTHEIOS_${t}_${PANTHEIOS_BACKEND}_${_P_LT}_DEBUG_LIBRARY
			PANTHEIOS_${t}_${PANTHEIOS_BACKEND}_${_P_LT}_LIBRARY
		)
		list(APPEND PANTHEIOS_LIBRARIES
			debug ${PANTHEIOS_${t}_${PANTHEIOS_BACKEND}_${_P_LT}_DEBUG_LIBRARY}
			optimized ${PANTHEIOS_${t}_${PANTHEIOS_BACKEND}_${_P_LT}_LIBRARY}
		)
	endforeach()
endif()

# frontent libraries
set(PANTHEIOS_fe_DEBUG_LIBRARY)
set(PANTHEIOS_fe_LIBRARY)
if(NOT PANTHEIOS_FRONTENT STREQUAL null)
	set(_P_FE ${PANTHEIOS_FRONTEND})
	find_library(PANTHEIOS_${_P_FE}_${_P_LT}_DEBUG_LIBRARY
		pantheios.1.fe.${_P_FE}.${_P_LIB_TAG}.${_P_LT}.debug
		PATH_SUFFIXES lib
		HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
		ENV PANTHEIOS_ROOT
	)
	find_library(PANTHEIOS_${_P_FE}_${_P_LT}_LIBRARY
		pantheios.1.fe.${_P_FE}.${_P_LIB_TAG}.${_P_LT}
		PATH_SUFFIXES lib
		HINTS ${PANTHEIOS_ROOT_HINT} ${PANTHEIOS_ROOT}
		ENV PANTHEIOS_ROOT
	)

	list(APPEND _P_REQUIRED_LIBVARS
		PANTHEIOS_${_P_FE}_${_P_LT}_DEBUG_LIBRARY
		PANTHEIOS_${_P_FE}_${_P_LT}_LIBRARY
	)
	list(APPEND PANTHEIOS_LIBRARIES
		debug ${PANTHEIOS_${_P_FE}_${_P_LT}_DEBUG_LIBRARY}
		optimized ${PANTHEIOS_${_P_FE}_${_P_LT}_LIBRARY}
	)
endif()

# gcc needs the core library mentioned a second time at the end 
# (see Pantheios FAQ Q/A8)
# At this point, the core has to be found already, 
# so only the additions to the lists are repeated here... 
if(CMAKE_COMPILER_IS_GNUCC)  
    list(APPEND _P_REQUIRED_LIBVARS
        PANTHEIOS_core_${PANTHEIOS_LIB_LINKTYPE}_DEBUG_LIBRARY
        PANTHEIOS_core_${PANTHEIOS_LIB_LINKTYPE}_LIBRARY
    )
    list(APPEND PANTHEIOS_LIBRARIES
        debug ${PANTHEIOS_core_${PANTHEIOS_LIB_LINKTYPE}_DEBUG_LIBRARY}
        optimized ${PANTHEIOS_core_${PANTHEIOS_LIB_LINKTYPE}_LIBRARY}
    )
endif()

# copy to NAME_LIBS
set(PANTHEIOS_LIBS ${PANTHEIOS_LIBRARIES})	

# handle the QUIETLY and REQUIRED arguments and set Pantheios_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Pantheios
	REQUIRED_VARS PANTHEIOS_INCLUDE_DIR ${_P_REQUIRED_LIBVARS}
)
								  
message(STATUS ${PANTHEIOS_INCLUDE_DIR} ${PANTHEIOS_LIBRARIES})
mark_as_advanced(PANTHEIOS_INCLUDE_DIR PANTHEIOS_LIBRARIES)

