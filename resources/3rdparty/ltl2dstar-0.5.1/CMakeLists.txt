cmake_minimum_required (VERSION 2.8.6)

# Set project name
project (ltl2dstar CXX C)

# Main Sources
file(GLOB_RECURSE LTL2DSTAR_HEADERS ${PROJECT_SOURCE_DIR}/src/*.hpp)
file(GLOB_RECURSE LTL2DSTAR_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)


# Add base folder for better inclusion paths
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost REQUIRED)

if(Boost_FOUND)
    if ((NOT Boost_LIBRARY_DIRS) OR ("${Boost_LIBRARY_DIRS}" STREQUAL ""))
        set(Boost_LIBRARY_DIRS "${Boost_INCLUDE_DIRS}/stage/lib")
    endif ()
	
    message(STATUS "BOOST_INCLUDE_DIRS is ${Boost_INCLUDE_DIRS}")
    message(STATUS "BOOST_LIBRARY_DIRS is ${Boost_LIBRARY_DIRS}")
	
    include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

if(MSVC)
	# required for GMM to compile, ugly error directive in their code
	add_definitions(/D_SCL_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS)
	# required as the PRCTL Parser bloats object files (COFF) beyond their maximum size (see http://msdn.microsoft.com/en-us/library/8578y171(v=vs.110).aspx)
	add_definitions(/bigobj)
endif()

# Add the executables
# Must be created *after* Boost was added because of LINK_DIRECTORIES
add_library(ltl2dstar ${LTL2DSTAR_SOURCES} ${LTL2DSTAR_HEADERS})

# Add target link deps for Boost program options
target_link_libraries(ltl2dstar ${Boost_LIBRARIES})   
