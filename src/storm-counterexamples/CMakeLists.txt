file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-counterexamples/*.h ${PROJECT_SOURCE_DIR}/src/storm-counterexamples/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-counterexamples)



file(GLOB_RECURSE STORM_CEX_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-counterexamples/*/*.cpp)
file(GLOB_RECURSE STORM_CEX_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-counterexamples/*/*.h)


# Create storm-counterexamples.
add_library(storm-counterexamples SHARED ${STORM_CEX_SOURCES} ${STORM_CEX_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-counterexamples PROPERTIES DEFINE_SYMBOL "")
#add_dependencies(storm resources)
list(APPEND STORM_TARGETS storm-counterexamples)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-counterexamples PUBLIC storm)

# Install storm headers to include directory.
foreach(HEADER ${STORM_CEX_HEADERS})
	string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
	string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
	string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		DEPENDS ${HEADER}
	)
	list(APPEND STORM_CEX_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_cex_headers DEPENDS ${STORM_CEX_OUTPUT_HEADERS} ${STORM_CEX_HEADERS})
add_dependencies(storm-counterexamples copy_storm_cex_headers)

# installation
install(TARGETS storm-counterexamples EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

