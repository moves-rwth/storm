#############################################################
##
## STORM PYTHON
##
#############################################################

function (add_python_module _libname _modulename, _sources)
    add_library(${_libname} SHARED ${_sources})
    target_link_libraries(${_libname} storm ${BOOST_PYTHON_LIB} ${PYTHON_LIBRARIES})
    set_target_properties(${_libname} PROPERTIES
            OUTPUT_NAME "_${_modulename}"
            LIBRARY_OUTPUT_DIRECTORY ${STORMPY_OUTPUT_DIR}/${_modulename}
            PREFIX ""
            SUFFIX ".so"
            )
endfunction()



if(STORM_PYTHON)
    add_python_module(stormpy-info info ${CMAKE_CURRENT_SOURCE_DIR}/storm-info.cpp)
    add_python_module(stormpy-core core ${CMAKE_CURRENT_SOURCE_DIR}/storm-core.cpp)

    #    add_library(stormpy-info SHARED ${CMAKE_CURRENT_SOURCE_DIR}/storm-info.cpp)
#    target_link_libraries(stormpy-info storm ${BOOST_PYTHON_LIB} ${PYTHON_LIBRARIES})
#    set_target_properties(stormpy-info PROPERTIES
#                             OUTPUT_NAME _info
#                             LIBRARY_OUTPUT_DIRECTORY ${STORMPY_OUTPUT_DIR}/info
#                             PREFIX ""
#                             SUFFIX ".so"
#                            )
#    add_library(stormpy-core SHARED ${CMAKE_CURRENT_SOURCE_DIR}/storm-core.cpp)
#    target_link_libraries(stormpy-core storm ${BOOST_PYTHON_LIB} ${PYTHON_LIBRARIES})
#    set_target_properties(stormpy-core PROPERTIES
#                             OUTPUT_NAME _core
#                             LIBRARY_OUTPUT_DIRECTORY ${STORMPY_OUTPUT_DIR}/core
#                             PREFIX ""
#                             SUFFIX ".so"
#                            )

                                                 

    add_custom_target(stormpy DEPENDS stormpy-info stormpy-core)


endif()
