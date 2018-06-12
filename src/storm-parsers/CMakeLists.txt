file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-parsers/*.h ${PROJECT_SOURCE_DIR}/src/storm-parsers/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-parsers)



file(GLOB_RECURSE STORM_PARSER_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-parsers/*/*.cpp)
file(GLOB_RECURSE STORM_PARSER_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-parsers/*/*.h)


# Disable Debug compiler flags for PrismParser to lessen memory consumption during compilation
SET_SOURCE_FILES_PROPERTIES(${PROJECT_SOURCE_DIR}/src/storm-parsers/parser/PrismParser.cpp PROPERTIES COMPILE_FLAGS -g0)
# Create storm-parsers.
add_library(storm-parsers SHARED ${STORM_PARSER_SOURCES} ${STORM_PARSER_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-parsers PROPERTIES DEFINE_SYMBOL "")
#add_dependencies(storm resources)
list(APPEND STORM_TARGETS storm-parsers)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-parsers PUBLIC storm)

# Install storm headers to include directory.
foreach(HEADER ${STORM_PARSER_HEADERS})
	string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
	string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
	string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		DEPENDS ${HEADER}
	)
	list(APPEND STORM_PARSER_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_parser_headers DEPENDS ${STORM_PARSER_OUTPUT_HEADERS} ${STORM_PARSER_HEADERS})
add_dependencies(storm-parsers copy_storm_parser_headers)

# installation
install(TARGETS storm-parsers EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

