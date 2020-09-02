file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-figaro/*.h ${PROJECT_SOURCE_DIR}/src/storm-figaro/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-figaro)



file(GLOB_RECURSE STORM_FIGARO_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-figaro/*/*.cpp)
file(GLOB_RECURSE STORM_FIGARO_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-figaro/*/*.h)


# Create storm-figaro.
add_library(storm-figaro SHARED ${STORM_FIGARO_SOURCES} ${STORM_FIGARO_HEADERS} model/FigaroModelTemplate.h)

# Remove define symbol for shared libstorm.
set_target_properties(storm-figaro PROPERTIES DEFINE_SYMBOL "")
#add_dependencies(storm resources)
list(APPEND STORM_TARGETS storm-figaro)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-figaro PUBLIC storm storm-parsers ${STORM_FIGARO_LINK_LIBRARIES})

# Install storm headers to include directory.
foreach(HEADER ${STORM_FIGARO_HEADERS})
	string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
	string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
	string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		DEPENDS ${HEADER}
	)
	list(APPEND STORM_FIGARO_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_figaro_headers DEPENDS ${STORM_FIGARO_OUTPUT_HEADERS} ${STORM_FIGARO_HEADERS})
add_dependencies(storm-figaro copy_storm_figaro_headers)

# installation
install(TARGETS storm-figaro EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

