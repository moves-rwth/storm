file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-gspn/*.h ${PROJECT_SOURCE_DIR}/src/storm-gspn/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-gspn)



file(GLOB_RECURSE STORM_GSPN_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-gspn/*/*.cpp)
file(GLOB_RECURSE STORM_GSPN_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-gspn/*/*.h)


# Create storm-pgcl.
add_library(storm-gspn SHARED ${STORM_GSPN_SOURCES} ${STORM_GSPN_HEADERS})
list(APPEND STORM_TARGETS storm-gspn)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)
target_link_libraries(storm-gspn storm ${STORM_GSPN_LINK_LIBRARIES})

# installation
install(TARGETS storm-gspn EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
