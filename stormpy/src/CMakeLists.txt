# This is a CMake example for Python

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src ${stormpy_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${pybind11_incdir})

FILE(GLOB core_files "core/*.cpp")
#FILE(GLOB info_files "info/*.cpp")
#FILE(GLOB expressions_files "expressions/*.cpp")
FILE(GLOB storage_files "storage/*.cpp")
FILE(GLOB logic_files "logic/*.cpp")
ADD_LIBRARY(stormpy_core SHARED mod_core.cpp ${core_files})
ADD_LIBRARY(stormpy_info SHARED mod_info.cpp)
ADD_LIBRARY(stormpy_expressions SHARED mod_expressions.cpp)
ADD_LIBRARY(stormpy_storage SHARED mod_storage.cpp ${storage_files})
ADD_LIBRARY(stormpy_logic SHARED mod_logic.cpp ${logic_files})

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

SET_TARGET_PROPERTIES(stormpy_expressions PROPERTIES
        OUTPUT_NAME expressions
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${STORMPY_OUTPUT_DIR}/expressions
)

SET_TARGET_PROPERTIES(stormpy_storage PROPERTIES
        OUTPUT_NAME storage
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${STORMPY_OUTPUT_DIR}/storage
)

SET_TARGET_PROPERTIES(stormpy_logic PROPERTIES
        OUTPUT_NAME logic
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${STORMPY_OUTPUT_DIR}/logic
)

TARGET_LINK_LIBRARIES(stormpy_core storm ${PYTHON_LIBRARIES})
TARGET_LINK_LIBRARIES(stormpy_info storm ${PYTHON_LIBRARIES})
TARGET_LINK_LIBRARIES(stormpy_expressions storm ${PYTHON_LIBRARIES})
TARGET_LINK_LIBRARIES(stormpy_storage storm ${PYTHON_LIBRARIES})
TARGET_LINK_LIBRARIES(stormpy_logic storm ${PYTHON_LIBRARIES})
