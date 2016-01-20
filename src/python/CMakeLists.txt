#############################################################
##
## STORM PYTHON
##
#############################################################

function (add_python_module _libname _modulename _sources)
    add_library(${_libname} SHARED ${_sources})
    target_link_libraries(${_libname} storm ${BOOST_PYTHON_LIB} ${PYTHON_LIBRARIES})
    set_target_properties(${_libname} PROPERTIES
            OUTPUT_NAME "_${_modulename}"
            LIBRARY_OUTPUT_DIRECTORY "${STORMPY_OUTPUT_DIR}/${_modulename}"
            PREFIX ""
            SUFFIX ".so"
            )
endfunction()



if(STORM_PYTHON)
    add_python_module(stormpy-info info ${CMAKE_CURRENT_SOURCE_DIR}/storm-info.cpp)
    add_python_module(stormpy-core core ${CMAKE_CURRENT_SOURCE_DIR}/storm-core.cpp)
    add_python_module(stormpy-logic logic ${CMAKE_CURRENT_SOURCE_DIR}/storm-logic.cpp)
    add_custom_target(stormpy DEPENDS stormpy-info stormpy-core stormpy-logic)
endif()
