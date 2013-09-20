cmake_minimum_required (VERSION 2.8.6)

# Set project name
project (cudd C)

# Main Sources
file(GLOB_RECURSE CUDD_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE CUDD_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)


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

# Add the library
add_library(cudd ${CUDD_SOURCES} ${CUDD_HEADERS})