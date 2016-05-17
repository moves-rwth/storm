# This is a CMake example for Python

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src ${stormpy_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${pybind11_incdir})

FILE(GLOB core_files "core/*.cpp")
FILE(GLOB info_files "info/*.cpp")
ADD_LIBRARY(stormpy_core SHARED mod_core.cpp ${core_files})
ADD_LIBRARY(stormpy_info SHARED mod_info.cpp ${info_files})

SET(STORMPY_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
SET(STORMPY_OUTPUT_DIR "${PROJECT_BINARY_DIR}/stormpy")

SET_TARGET_PROPERTIES(stormpy_core PROPERTIES
        OUTPUT_NAME core
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${STORMPY_OUTPUT_DIR}/
)

SET_TARGET_PROPERTIES(stormpy_info PROPERTIES
        OUTPUT_NAME info
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${STORMPY_OUTPUT_DIR}/info
)

add_custom_target(stormpy_files)
add_custom_command(TARGET stormpy_files POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_SOURCE_DIR}/stormpy ${STORMPY_OUTPUT_DIR}
    )

if(BUILD_STATIC)
    TARGET_LINK_LIBRARIES(stormpy_core lib_storm_static ${stormpy_LIBRARIES_STATIC} ${PYTHON_LIBRARIES} ${libs_static} )
    TARGET_LINK_LIBRARIES(stormpy_info lib_storm_static ${stormpy_LIBRARIES_STATIC} ${PYTHON_LIBRARIES} ${libs_static} )
else()
    TARGET_LINK_LIBRARIES(stormpy_core lib_storm ${stormpy_LIBRARIES_DYNAMIC} ${PYTHON_LIBRARIES} ${libs_dynamic})
    TARGET_LINK_LIBRARIES(stormpy_info lib_storm ${stormpy_LIBRARIES_DYNAMIC} ${PYTHON_LIBRARIES} ${libs_dynamic})
endif()
#
add_custom_target(stormpy DEPENDS stormpy_files stormpy_core stormpy_info)

