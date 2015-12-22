#############################################################
##
## STORM PYTHON
##
#############################################################
if(STORM_PYTHON)
    add_library(stormpy-info SHARED ${CMAKE_CURRENT_SOURCE_DIR}/storm-info.cpp)
    target_link_libraries(stormpy-info storm ${BOOST_PYTHON_LIB} ${PYTHON_LIBRARIES})
    set_target_properties(stormpy-info PROPERTIES
                             OUTPUT_NAME _info
                             LIBRARY_OUTPUT_DIRECTORY ${STORMPY_OUTPUT_DIR}/info
                             PREFIX ""
                             SUFFIX ".so"
                            )
    add_library(stormpy-core SHARED ${CMAKE_CURRENT_SOURCE_DIR}/storm-core.cpp)
    target_link_libraries(stormpy-core storm ${BOOST_PYTHON_LIB} ${PYTHON_LIBRARIES})
    set_target_properties(stormpy-core PROPERTIES
                             OUTPUT_NAME _core
                             LIBRARY_OUTPUT_DIRECTORY ${STORMPY_OUTPUT_DIR}/core
                             PREFIX ""
                             SUFFIX ".so"
                            )
                                                 

    add_custom_target(stormpy DEPENDS stormpy-info stormpy-core)


endif()
