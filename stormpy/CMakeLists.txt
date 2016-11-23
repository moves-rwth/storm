find_package(PythonInterp REQUIRED QUIET)
find_package(PythonLibs REQUIRED QUIET)

message(STATUS "Python executable: ${PYTHON_EXECUTABLE}")
message(STATUS "Python include: ${PYTHON_INCLUDE_DIRS}")
message(STATUS "Python include: ${PYTHON_LIBRARIES}")

set(STORMPY_OUTPUT_DIR "${PROJECT_BINARY_DIR}/stormpy")
set(STORMPY_SOURCE_DIR "${PROJECT_SOURCE_DIR}/stormpy")

if(STORM_HAVE_CLN)
    set(STORMPY_USE_CLN 1)
else()
    set(STORMPY_USE_CLN 0)
endif()

# Set configuration file
get_directory_property(STORMPY_INCLUDE_DIRS_PROP INCLUDE_DIRECTORIES)
foreach(arg ${STORMPY_INCLUDE_DIRS_PROP})
    set(STORMPY_INCLUDE_DIRS "${STORMPY_INCLUDE_DIRS}${sep}${arg}")
    set(sep ":")
endforeach()
set(STORMPY_COMPILE_ARGS ${CMAKE_CXX_FLAGS})
set(STORMPY_LIBRARY_DIRS "${PROJECT_BINARY_DIR}/src/storm")
set(STORMPY_RPATH "${PROJECT_BINARY_DIR}/src/storm")
configure_file (
    "${PROJECT_SOURCE_DIR}/stormpy/setup.cfg.in"
    "${PROJECT_SOURCE_DIR}/stormpy/setup.cfg"
)

# Add targets
add_custom_target(PythonBindingsBuild
    COMMAND ${PYTHON_EXECUTABLE} setup.py build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS storm
)
add_custom_target(PythonBindingsDevelop
    COMMAND ${PYTHON_EXECUTABLE} setup.py develop
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS storm
)
add_custom_target(PythonBindingsInstall
    COMMAND ${PYTHON_EXECUTABLE} setup.py install --user
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS storm
)
