cmake_minimum_required(VERSION 3.0.0)
project(pystorm)


find_package(storm REQUIRED)
add_subdirectory(resources/pybind11)


file(GLOB_RECURSE PYCARL_CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp)

pybind11_add_module(pycarl.core ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_core.cpp ${PYCARL_CORE_SOURCES})
target_include_directories(pycarl.core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pycarl.core PRIVATE lib_carl)


file(GLOB_RECURSE PYCARL_CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/formula/*.cpp)

pybind11_add_module(pycarl.formula ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_formula.cpp ${PYCARL_FORMULA_SOURCES})
target_include_directories(pycarl.formula PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pycarl.formula PRIVATE lib_carl)


file(GLOB_RECURSE PYCARL_CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/parse/*.cpp)

pybind11_add_module(pycarl.parse ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_parse.cpp ${PYCARL_PARSE_SOURCES})
target_include_directories(pycarl.parse PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pycarl.parse PRIVATE lib_carl)


#set(STORMPY_OUTPUT_DIR "${PROJECT_BINARY_DIR}/stormpy")
#set(STORMPY_SOURCE_DIR "${PROJECT_SOURCE_DIR}/stormpy")
#
#if(STORM_HAVE_CLN)
#    set(STORMPY_USE_CLN 1)
#else()
#    set(STORMPY_USE_CLN 0)
#endif()
#
## Set configuration file
#get_directory_property(STORMPY_INCLUDE_DIRS_PROP INCLUDE_DIRECTORIES)
#foreach(arg ${STORMPY_INCLUDE_DIRS_PROP})
#    set(STORMPY_INCLUDE_DIRS "${STORMPY_INCLUDE_DIRS}${sep}${arg}")
#    set(sep ":")
#endforeach()
#set(STORMPY_COMPILE_ARGS ${CMAKE_CXX_FLAGS})
#set(STORMPY_LIBRARY_DIRS "${PROJECT_BINARY_DIR}/src/storm")
#set(STORMPY_RPATH "${PROJECT_BINARY_DIR}/src/storm")
#configure_file (
#    "${PROJECT_SOURCE_DIR}/stormpy/setup.cfg.in"
#    "${PROJECT_SOURCE_DIR}/stormpy/setup.cfg"
#)
#

