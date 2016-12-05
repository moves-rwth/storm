cmake_minimum_required(VERSION 3.0.0)
project(pystorm)


find_package(storm REQUIRED)
add_subdirectory(resources/pybind11)


file(GLOB_RECURSE STORM_CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp)

pybind11_add_module(stormpy.core ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_core.cpp ${STORM_CORE_SOURCES})
target_include_directories(stormpy.core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${storm_INCLUDE_DIR})
target_link_libraries(stormpy.core PRIVATE storm)

file(GLOB_RECURSE STORM_INFO_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/info/*.cpp)

pybind11_add_module(stormpy.info ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_info.cpp ${STORM_INFO_SOURCES})
target_include_directories(stormpy.info PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${storm_INCLUDE_DIR})
target_link_libraries(stormpy.info PRIVATE storm)


file(GLOB_RECURSE STORM_EXPRESSIONS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/expressions/*.cpp)

pybind11_add_module(stormpy.expressions ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_expressions.cpp ${STORM_EXPRESSIONS_SOURCES})
target_include_directories(stormpy.expressions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${storm_INCLUDE_DIR})
target_link_libraries(stormpy.expressions PRIVATE storm)


file(GLOB_RECURSE STORM_LOGIC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/logic/*.cpp)

pybind11_add_module(stormpy.logic ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_logic.cpp ${STORM_LOGIC_SOURCES})
target_include_directories(stormpy.logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${storm_INCLUDE_DIR})
target_link_libraries(stormpy.logic PRIVATE storm)

file(GLOB_RECURSE STORM_STORAGE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/storage/*.cpp)

pybind11_add_module(stormpy.storage ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_storage.cpp ${STORM_STORAGE_SOURCES})
target_include_directories(stormpy.storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${storm_INCLUDE_DIR})
target_link_libraries(stormpy.storage PRIVATE storm)

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

