cmake_minimum_required (VERSION 2.8.6)

# Set project name
project (glpk CXX C)

# Add the resources/cmake folder to Module Search Path for FindMySQL.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/resources/")

# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/amd")
include_directories("${PROJECT_SOURCE_DIR}/src/bflib")
include_directories("${PROJECT_SOURCE_DIR}/src/cglib")
include_directories("${PROJECT_SOURCE_DIR}/src/colamd")
include_directories("${PROJECT_SOURCE_DIR}/src/env")
include_directories("${PROJECT_SOURCE_DIR}/src/minisat")
include_directories("${PROJECT_SOURCE_DIR}/src/misc")
include_directories("${PROJECT_SOURCE_DIR}/src/proxy")
include_directories("${PROJECT_SOURCE_DIR}/src/zlib")

#############################################################
##
##	CMake options of GLPK
##
#############################################################
option(WITH_GLPK_EXAMPLES "Also build examples and standalone executables" OFF)
option(WITH_GMP "Use GNU MO bignum library" OFF)
option(ENABLE_DL "Enable shared library support" OFF)
option(ENABLE_ODBC "Enable MathProg ODBC support" OFF)
option(ENABLE_MYSQL "Enable MathProg MySQL support" OFF)
option(DEBUG "Sets whether the DEBUG mode is used" OFF)
set(ADDITIONAL_INCLUDE_DIRS "" CACHE STRING "Additional directories added to the include directories.")
set(ADDITIONAL_LINK_DIRS "" CACHE STRING "Additional directories added to the link directories.")

# If the DEBUG option was turned on, we will target a debug version and a release version otherwise
if (DEBUG)
    set (CMAKE_BUILD_TYPE "DEBUG")
else()
    set (CMAKE_BUILD_TYPE "RELEASE")
endif()
message(STATUS "GLPK - Building ${CMAKE_BUILD_TYPE} version.")
message(STATUS "GLPK - CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "GLPK - CMAKE_BUILD_TYPE (ENV): $ENV{CMAKE_BUILD_TYPE}")


#############################################################
##
##	Compiler specific settings and definitions
##
#############################################################

if(CMAKE_COMPILER_IS_GNUCC)
	message(STATUS "GLPK - Using Compiler Configuration: GCC")
	# Set standard flags for GCC
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-return-type -Wno-parentheses -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-address -Wno-unused-label")
	add_definitions(-DHAVE_CONFIG_H=1)
elseif(MSVC)
	message(STATUS "GLPK - Using Compiler Configuration: MSVC")
	add_definitions(/D_SCL_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS)
	add_definitions(/D_VARIADIC_MAX=10)
	add_definitions(/DNOMINMAX)	
	add_definitions(/DHAVE_CONFIG_H=1)
else(CLANG)
	message(STATUS "GLPK - Using Compiler Configuration: Clang (LLVM)")
	# As CLANG is not set as a variable, we need to set it in case we have not matched another compiler.
	set (CLANG ON)
	# Set standard flags for clang
	set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -funroll-loops -O3")

	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-return-type -Wno-return-type-c-linkage -Wno-unused-label -Wno-unused-value -Wno-unused-function -Wno-unused-variable -Wno-logical-op-parentheses -Wno-macro-redefined -Wno-uninitialized -Wno-sometimes-uninitialized -Wno-self-assign")

	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
	add_definitions(-DHAVE_CONFIG_H=1)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -version-info=37:0:1 -export-symbols-regex='^glp_*'")
endif()

#############################################################
##
##	CMake-generated Config File for GLPK
##
#############################################################
include(CheckIncludeFiles)
include(CheckFunctionExists)
CHECK_INCLUDE_FILES(sys/time.h HAVE_SYS_TIME_H)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_INCLUDE_FILES(ltdl.h HAVE_LTDL)
CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN)
if(WITH_GMP)
	message(STATUS "GLPK - Building with GMP Support")
	CHECK_INCLUDE_FILE(gmp.h HAVE_GMP_H)
	if (NOT HAVE_GMP_H)
		message(SEND_ERROR "Could not find gmp.h!")
	endif()
	set(GLPK_HAVE_GMP "define")
else()
	set(GLPK_HAVE_GMP "undef")
endif()

if (UNIX AND NOT APPLE)
	set(LIBIODBC "libiodbc.so")
	set(LIBODBC "libodbc.so")
	set(LIBMYSQL "libmysqlclient.so")
elseif(UNIX AND APPLE)
	set(LIBIODBC "libiodbc.dylib")
	set(LIBODBC "libodbc.dylib")
	set(LIBMYSQL "libmysqlclient.dylib")
elseif(MSVC)
	set(LIBIODBC "odbc32.lib")
	set(LIBODBC "odbc32.lib")
	set(LIBMYSQL "mysqlclient.lib")
endif()

if(ENABLE_ODBC)
	message(STATUS "GLPK - Building with ODBC Support")
	find_package(ODBC REQUIRED)
	if (NOT ENABLE_DL)
		message(SEND_ERROR "ENABLE_ODBC required ENABLE_DL")
	endif()
	include_directories(ODBC_INCLUDE_DIRECTORIES)
	set(GLPK_HAVE_ODBC "define")
	set(GLPK_ODBC_DLNAME "${ODBC_LIBRARIES}")
else()
	set(GLPK_HAVE_ODBC "undef")
	set(GLPK_ODBC_DLNAME "")
endif()

if(ENABLE_MYSQL)
	message(STATUS "GLPK - Building with MySQL Support")
	find_package(MySQL REQUIRED)
	if (NOT ENABLE_DL)
		message(SEND_ERROR "ENABLE_MYSQL required ENABLE_DL")
	endif()
	include_directories(MYSQL_INCLUDE_DIR)
	set(GLPK_HAVE_MYSQL "define")
	set(GLPK_MYSQL_DLNAME "${MYSQL_LIBRARIES}")
else()
	set(GLPK_HAVE_MYSQL "undef")
	set(GLPK_MYSQL_DLNAME "")
endif()

# Convert to CMake Variables to be used in the Config File
if(HAVE_SYS_TIME_H)
	set(GLPK_HAVE_SYS_TIME_H "define")
else()
	set (GLPK_HAVE_SYS_TIME_H "undef")
endif()
if(HAVE_GETTIMEOFDAY)
	set(GLPK_HAVE_GETTIMEOFDAY "define")
else()
	set (GLPK_HAVE_GETTIMEOFDAY "undef")
endif()
if(HAVE_LTDL)
	set(GLPK_HAVE_LTDL "define")
else()
	set (GLPK_HAVE_LTDL "undef")
endif()
if(HAVE_DLFCN)
	set(GLPK_HAVE_DLFCN "define")
else()
	set (GLPK_HAVE_DLFCN "undef")
endif()


# Configure a header file to pass some of the CMake settings to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.cmake.in"
	"${PROJECT_BINARY_DIR}/include/config.h"
)
# Add the binary dir include directory for config.h
include_directories("${PROJECT_BINARY_DIR}/include")

#############################################################
##
##	Source file aggregation and clustering
##
#############################################################
file(GLOB_RECURSE GLPK_LIB_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE GLPK_LIB_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

file(GLOB_RECURSE GLPK_LIB_DEF ${PROJECT_SOURCE_DIR}/w64/*.def)

set(GLPK_EXAMPLE_GLPSOL_SOURCES ${PROJECT_SOURCE_DIR}/examples/glpsol.c)
set(GLPK_EXAMPLE_IPTSAMP_SOURCES ${PROJECT_SOURCE_DIR}/examples/iptsamp.c)
set(GLPK_EXAMPLE_MPLSAMP1_SOURCES ${PROJECT_SOURCE_DIR}/examples/mplsamp1.c)
set(GLPK_EXAMPLE_MPLSAMP2_SOURCES ${PROJECT_SOURCE_DIR}/examples/mplsamp2.c)
set(GLPK_EXAMPLE_SAMPLE_SOURCES ${PROJECT_SOURCE_DIR}/examples/sample.c)
set(GLPK_EXAMPLE_SPXSAMP1_SOURCES ${PROJECT_SOURCE_DIR}/examples/spxsamp1.c)
set(GLPK_EXAMPLE_SPXSAMP2_SOURCES ${PROJECT_SOURCE_DIR}/examples/spxsamp2.c)
set(GLPK_EXAMPLE_NETGEN_SOURCES ${PROJECT_SOURCE_DIR}/examples/netgen.c)

# Additional include files like the storm-config.h
file(GLOB_RECURSE GLPK_BUILD_HEADERS ${PROJECT_BINARY_DIR}/include/*.h)

# Add custom additional include or link directories
if (ADDITIONAL_INCLUDE_DIRS)
	message(STATUS "GLPK - Using additional include directories ${ADDITIONAL_INCLUDE_DIRS}")
	include_directories(${ADDITIONAL_INCLUDE_DIRS})
endif(ADDITIONAL_INCLUDE_DIRS)
if (ADDITIONAL_LINK_DIRS)
	message(STATUS "GLPK - Using additional link directories ${ADDITIONAL_LINK_DIRS}")
	link_directories(${ADDITIONAL_LINK_DIRS})
endif(ADDITIONAL_LINK_DIRS)

###############################################################################
##                                                                            #
##	Executable Creation                                                       #
##                                                                            #
##  All link_directories() calls MUST be made before this point               #
##                                                                            #
###############################################################################
if(WITH_GLPK_EXAMPLES)
	add_executable(glpsol ${GLPK_EXAMPLE_GLPSOL_SOURCES})
	add_executable(iptsamp ${GLPK_EXAMPLE_IPTSAMP_SOURCES})
	add_executable(mplsamp1 ${GLPK_EXAMPLE_MPLSAMP1_SOURCES})
	add_executable(mplsamp2 ${GLPK_EXAMPLE_MPLSAMP2_SOURCES})
	add_executable(spxsamp1 ${GLPK_EXAMPLE_SPXSAMP1_SOURCES})
	add_executable(spxsamp2 ${GLPK_EXAMPLE_SPXSAMP2_SOURCES})
	add_executable(netgen ${GLPK_EXAMPLE_NETGEN_SOURCES})
	add_executable(sample ${GLPK_EXAMPLE_SAMPLE_SOURCES})
endif()
	
if(ENABLE_DL)
	set(GLPK_BUILD_TYPE SHARED)
else()
	set(GLPK_BUILD_TYPE STATIC)
endif()
if (NOT MSVC)
	set(GLPK_LIB_DEF "")
endif()

add_library(glpk ${GLPK_BUILD_TYPE} ${GLPK_LIB_SOURCES} ${GLPK_LIB_HEADERS} ${GLPK_LIB_DEF})
IF(UNIX)
	target_link_libraries(glpk m)
ENDIF(UNIX)
if(HAVE_LTDL)
	target_link_libraries(glpk ltdl)
endif(HAVE_LTDL)
if(HAVE_DLFCN)
	target_link_libraries(glpk dl)
endif(HAVE_DLFCN)

if(WITH_GLPK_EXAMPLES)
	# Dependency for the Executable
	add_dependencies(glpsol glpk)
	add_dependencies(iptsamp glpk)
	add_dependencies(mplsamp1 glpk)
	add_dependencies(mplsamp2 glpk)
	add_dependencies(spxsamp1 glpk)
	add_dependencies(spxsamp2 glpk)
	add_dependencies(netgen glpk)
	add_dependencies(sample glpk)
	target_link_libraries(glpsol glpk)
	target_link_libraries(iptsamp glpk)
	target_link_libraries(mplsamp1 glpk)
	target_link_libraries(mplsamp2 glpk)
	target_link_libraries(spxsamp1 glpk)
	target_link_libraries(spxsamp2 glpk)
	target_link_libraries(netgen glpk)
	target_link_libraries(sample glpk)
endif()

#############################################################
##
##	ODBC
##
#############################################################
if(ODBC_FOUND)
	target_link_libraries(glpk ${ODBC_LIBRARIES})
endif()

#############################################################
##
##	MySQL
##
#############################################################
if(MYSQL_FOUND)
	target_link_libraries(glpk ${MYSQL_LIBRARIES})
endif()
