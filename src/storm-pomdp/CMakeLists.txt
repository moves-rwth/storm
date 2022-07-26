file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-pomdp/*.h ${PROJECT_SOURCE_DIR}/src/storm-pomdp/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-pomdp)



file(GLOB_RECURSE STORM_POMDP_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-pomdp/*/*.cpp)
file(GLOB_RECURSE STORM_POMDP_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-pomdp/*/*.h)


# Create storm-pomdp.
add_library(storm-pomdp SHARED ${STORM_POMDP_SOURCES} ${STORM_POMDP_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-pomdp PROPERTIES DEFINE_SYMBOL "")
#add_dependencies(storm resources)
list(APPEND STORM_TARGETS storm-pomdp)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-pomdp PUBLIC storm storm-parsers storm-pars ${STORM_POMDP_LINK_LIBRARIES})

# Install storm headers to include directory.
foreach(HEADER ${STORM_POMDP_HEADERS})
    string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
    string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
    string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
    add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            DEPENDS ${HEADER}
    )
    list(APPEND STORM_POMDP_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_pomdp_headers DEPENDS ${STORM_POMDP_OUTPUT_HEADERS} ${STORM_POMDP_HEADERS})
add_dependencies(storm-pomdp copy_storm_pomdp_headers)

# installation
install(TARGETS storm-pomdp RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

