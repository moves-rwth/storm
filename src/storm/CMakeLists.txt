file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm/*.h ${PROJECT_SOURCE_DIR}/src/storm/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm)

#############################################################
##
##	Source file aggregation and clustering
##
#############################################################
file(GLOB_RECURSE STORM_HEADERS ${PROJECT_SOURCE_DIR}/src/storm/*.h)
file(GLOB_RECURSE STORM_SOURCES_WITHOUT_MAIN ${PROJECT_SOURCE_DIR}/src/storm/*/*.cpp)
file(GLOB_RECURSE STORM_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm/storm.cpp)
file(GLOB_RECURSE STORM_DFT_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm/storm-dyftee.cpp)

# Additional include files like the storm-config.h
file(GLOB_RECURSE STORM_BUILD_HEADERS ${PROJECT_BINARY_DIR}/include/*.h)

set(STORM_LIB_SOURCES ${STORM_SOURCES_WITHOUT_MAIN})
set(STORM_LIB_HEADERS ${STORM_HEADERS})
set(STORM_MAIN_SOURCES  ${STORM_MAIN_FILE})
set(STORM_DFT_MAIN_SOURCES ${STORM_DFT_MAIN_FILE})

# Add custom additional include or link directories
if (ADDITIONAL_INCLUDE_DIRS)
	message(STATUS "StoRM - Using additional include directories ${ADDITIONAL_INCLUDE_DIRS}")
	include_directories(${ADDITIONAL_INCLUDE_DIRS})
endif(ADDITIONAL_INCLUDE_DIRS)
if (ADDITIONAL_LINK_DIRS)
	message(STATUS "StoRM - Using additional link directories ${ADDITIONAL_LINK_DIRS}")
	link_directories(${ADDITIONAL_LINK_DIRS})
endif(ADDITIONAL_LINK_DIRS)

###############################################################################
##                                                                            
##	Binary creation (All link_directories() calls must be made before this point.)
##
###############################################################################

# Create libstorm.
add_library(storm SHARED ${STORM_LIB_SOURCES} ${STORM_LIB_HEADERS})
# Remove define symbol for shared libstorm.
set_target_properties(storm PROPERTIES DEFINE_SYMBOL "")
add_dependencies(storm resources)
target_link_libraries(storm ${STORM_LINK_LIBRARIES})

# Create storm.
add_executable(storm-main ${STORM_MAIN_SOURCES} ${STORM_MAIN_HEADERS})
target_link_libraries(storm-main storm)
set_target_properties(storm-main PROPERTIES OUTPUT_NAME "storm")

# Create storm-dft.
add_executable(storm-dft-main ${STORM_DFT_MAIN_SOURCES} ${STORM_MAIN_HEADERS})
target_link_libraries(storm-dft-main storm) # Adding headers for xcode
set_target_properties(storm-dft-main PROPERTIES OUTPUT_NAME "storm-dft")

# Install storm headers to include directory.
foreach(HEADER ${STORM_LIB_HEADERS})
	string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
	string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
	string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		DEPENDS ${HEADER}
	)
	list(APPEND STORM_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_headers DEPENDS ${STORM_OUTPUT_HEADERS} ${STORM_LIB_HEADERS})
add_dependencies(storm copy_storm_headers)
add_dependencies(storm copy_resources_headers)

