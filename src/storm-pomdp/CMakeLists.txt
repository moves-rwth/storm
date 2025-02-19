file(GLOB_RECURSE STORM_POMDP_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-pomdp/*/*.cpp)
file(GLOB_RECURSE STORM_POMDP_HEADERS RELATIVE " ${PROJECT_SOURCE_DIR}/src/storm-pomdp"  ${PROJECT_SOURCE_DIR}/src/storm-pomdp/*/*.h)

# Create storm-pomdp.
add_library(storm-pomdp SHARED)
target_sources(storm-pomdp
		PRIVATE
		${STORM_POMDP_SOURCES}
		PUBLIC
		FILE_SET fs_storm_pomdp_headers TYPE HEADERS BASE_DIRS "${PROJECT_SOURCE_DIR}/src" FILES ${STORM_POMDP_HEADERS})
target_precompile_headers(storm-pomdp REUSE_FROM storm)
target_link_libraries(storm-pomdp PUBLIC storm storm-parsers storm-pars)

list(APPEND STORM_TARGETS storm-pomdp)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

# installation
install(TARGETS storm-pomdp RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL FILE_SET fs_storm_pomdp_headers)

