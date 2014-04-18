cmake_minimum_required (VERSION 2.8.6)

# Set project name
project (cudd C CXX)

# Main Sources
file(GLOB_RECURSE CUDD_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE CUDD_HEADERS_CXX ${PROJECT_SOURCE_DIR}/src/*.hh)
file(GLOB_RECURSE CUDD_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE CUDD_SOURCES_CXX ${PROJECT_SOURCE_DIR}/src/*.cc)


# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/cudd")
include_directories("${PROJECT_SOURCE_DIR}/src/dddmp")
include_directories("${PROJECT_SOURCE_DIR}/src/epd")
include_directories("${PROJECT_SOURCE_DIR}/src/mtr")
include_directories("${PROJECT_SOURCE_DIR}/src/nanotrav")
include_directories("${PROJECT_SOURCE_DIR}/src/st")
include_directories("${PROJECT_SOURCE_DIR}/src/util")

if(MSVC)
	# required for GMM to compile, ugly error directive in their code
	add_definitions(/D_SCL_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS)
endif()

# Since we do not target Alphas, this symbol is always set
add_definitions(-DHAVE_IEEE_754)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(STATUS "CUDD: Targeting 64bit architecture")
	add_definitions(-DSIZEOF_VOID_P=8)
	add_definitions(-DSIZEOF_LONG=8)
endif()

# Add the library
add_library(cudd ${CUDD_SOURCES} ${CUDD_HEADERS} ${CUDD_HEADERS_CXX} ${CUDD_SOURCES_CXX})