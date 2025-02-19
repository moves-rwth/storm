file(GLOB_RECURSE STORM_GSPN_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-gspn/*/*.cpp)
file(GLOB_RECURSE STORM_GSPN_HEADERS RELATIVE "${PROJECT_SOURCE_DIR}/src/storm-gspn" ${PROJECT_SOURCE_DIR}/src/storm-gspn/*/*.h)

add_library(storm-gspn SHARED)
target_sources(storm-gspn
		PRIVATE
		${STORM_GSPN_SOURCES}
		PUBLIC
		FILE_SET fs_storm_gspn_headers TYPE HEADERS BASE_DIRS "${PROJECT_SOURCE_DIR}/src" FILES ${STORM_GSPN_HEADERS})
target_precompile_headers(storm-gspn REUSE_FROM storm)
target_link_libraries(storm-gspn PUBLIC storm storm-conv storm-parsers ${STORM_GSPN_LINK_LIBRARIES})

# Remove define symbol for shared libstorm.
list(APPEND STORM_TARGETS storm-gspn)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

# installation
install(TARGETS storm-gspn EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL FILE_SET fs_storm_gspn_headers)

