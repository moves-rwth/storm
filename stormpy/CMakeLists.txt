CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT("stormpy")

SET(STORMPY_OUTPUT_DIR "${PROJECT_BINARY_DIR}/stormpy")

FIND_PACKAGE ( PythonInterp REQUIRED )
FIND_PACKAGE ( PythonLibs REQUIRED )

file(STRINGS "${PROJECT_SOURCE_DIR}/setup.cfg" include_dirs
        REGEX "^include_dirs=.*$")
string(REGEX REPLACE "^include_dirs=(.+)$" "\\1" include_dirs "${include_dirs}")
string(REPLACE ":" ";" include_dirs ${include_dirs})

file(STRINGS "${PROJECT_SOURCE_DIR}/setup.cfg" library_dirs
        REGEX "^library_dirs=.*$")
string(REGEX REPLACE "^library_dirs=(.+)$" "\\1" library_dirs "${library_dirs}")
string(REPLACE ":" ";" library_dirs ${library_dirs})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/resources/pybind11/include ${include_dirs} ${PYTHON_INCLUDE_DIRS})
LINK_DIRECTORIES(${library_dirs})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

ADD_SUBDIRECTORY( src )

add_custom_target(stormpy_files)
add_custom_command(TARGET stormpy_files POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_SOURCE_DIR}/lib/stormpy ${STORMPY_OUTPUT_DIR}
    )

add_custom_target(stormpy DEPENDS
    stormpy_files
    stormpy_core
    stormpy_info
    stormpy_expressions
    stormpy_storage
    stormpy_logic
    )
