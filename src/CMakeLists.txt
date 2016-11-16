#############################################################
##
##	Source file aggregation and clustering
##
#############################################################
file(GLOB_RECURSE STORM_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE STORM_HEADERS_CLI ${PROJECT_SOURCE_DIR}/src/cli/*.h)
file(GLOB_RECURSE STORM_SOURCES_WITHOUT_MAIN ${PROJECT_SOURCE_DIR}/src/*/*.cpp)
file(GLOB_RECURSE STORM_SOURCES_CLI ${PROJECT_SOURCE_DIR}/src/cli/*.cpp)
file(GLOB_RECURSE STORM_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm.cpp)
file(GLOB_RECURSE STORM_DFT_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm-dyftee.cpp)
file(GLOB_RECURSE STORM_PGCL_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm-pgcl.cpp)
file(GLOB_RECURSE STORM_GSPN_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm-gspn.cpp)

# Additional include files like the storm-config.h
file(GLOB_RECURSE STORM_BUILD_HEADERS ${PROJECT_BINARY_DIR}/include/*.h)

set(STORM_LIB_SOURCES ${STORM_SOURCES_WITHOUT_MAIN})
list(REMOVE_ITEM STORM_LIB_SOURCES ${STORM_SOURCES_CLI})
set(STORM_LIB_HEADERS ${STORM_HEADERS})
list(REMOVE_ITEM STORM_LIB_HEADERS ${STORM_HEADERS_CLI})
set(STORM_MAIN_SOURCES ${STORM_SOURCES_CLI} ${STORM_MAIN_FILE})
set(STORM_DFT_MAIN_SOURCES ${STORM_SOURCES_CLI} ${STORM_DFT_MAIN_FILE})
set(STORM_PGCL_MAIN_SOURCES ${STORM_SOURCES_CLI} ${STORM_PGCL_MAIN_FILE})
set(STORM_GSPN_MAIN_SOURCES ${STORM_SOURCES_CLI} ${STORM_GSPN_MAIN_FILE})
set(STORM_MAIN_HEADERS ${STORM_HEADERS_CLI})

file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/src/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}")

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

# Create storm-pgcl.
add_executable(storm-pgcl-main ${STORM_PGCL_MAIN_SOURCES} ${STORM_MAIN_HEADERS})
target_link_libraries(storm-pgcl-main storm)
set_target_properties(storm-pgcl-main PROPERTIES OUTPUT_NAME "storm-pgcl")

add_executable(storm-gspn-main ${STORM_GSPN_MAIN_SOURCES} ${STORM_MAIN_HEADERS})
target_link_libraries(storm-gspn-main storm) # Adding headers for xcode
set_target_properties(storm-gspn-main PROPERTIES OUTPUT_NAME "storm-gspn")

target_link_libraries(storm ${STORM_LINK_LIBRARIES})


INSTALL(TARGETS storm-main
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
