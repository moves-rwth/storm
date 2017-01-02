file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-dft/*.h ${PROJECT_SOURCE_DIR}/src/storm-dft/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-dft)



file(GLOB_RECURSE STORM_DFT_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-dft/*/*.cpp)
file(GLOB_RECURSE STORM_DFT_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-dft/*/*.h)


# Create storm-pgcl.
add_library(storm-dft SHARED ${STORM_DFT_SOURCES} ${STORM_DFT_HEADERS})
target_link_libraries(storm-dft storm storm-gspn ${STORM_DFT_LINK_LIBRARIES})
