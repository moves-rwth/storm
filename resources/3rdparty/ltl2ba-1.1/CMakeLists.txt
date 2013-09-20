cmake_minimum_required (VERSION 2.8.6)

# Set project name
project (ltl2ba CXX C)

# Main Sources
file(GLOB_RECURSE LTL2BA_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE LTL2BA_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)


# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}/src")

if(MSVC)
	# required for GMM to compile, ugly error directive in their code
	add_definitions(/D_SCL_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS)
endif()

# Add the executables
add_executable(ltl2ba ${LTL2BA_SOURCES} ${LTL2BA_HEADERS})
