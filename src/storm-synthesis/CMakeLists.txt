file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-synthesis/*.h ${PROJECT_SOURCE_DIR}/src/storm-synthesis/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-synthesis)



file(GLOB_RECURSE STORM_SYN_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-synthesis/*/*.cpp)
file(GLOB_RECURSE STORM_SYN_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-synthesis/*/*.h)


# Create storm-synthesis.
add_library(storm-synthesis SHARED ${STORM_SYN_SOURCES} ${STORM_SYN_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-synthesis PROPERTIES DEFINE_SYMBOL "")
add_dependencies(storm-synthesis storm-pomdp)
list(APPEND STORM_TARGETS storm-synthesis)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-synthesis PUBLIC storm storm-pomdp)

# Install storm headers to include directory.
foreach(HEADER ${STORM_SYN_HEADERS})
	string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
	string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
	string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		DEPENDS ${HEADER}
	)
	list(APPEND STORM_SYN_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_syn_headers DEPENDS ${STORM_SYN_OUTPUT_HEADERS} ${STORM_SYN_HEADERS})
add_dependencies(storm-synthesis copy_storm_syn_headers)

# installation
install(TARGETS storm-synthesis RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

