file(GLOB_RECURSE STORM_PARS_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-pars/*/*.cpp)
file(GLOB_RECURSE STORM_PARS_HEADERS RELATIVE "${PROJECT_SOURCE_DIR}/src/storm-pars"  ${PROJECT_SOURCE_DIR}/src/storm-pars/*/*.h)

# Create storm-pars.
add_library(storm-pars SHARED)
target_sources(storm-pars
		PRIVATE
		${STORM_PARS_SOURCES}
		PUBLIC
		FILE_SET fs_storm_pars_headers TYPE HEADERS BASE_DIRS "${PROJECT_SOURCE_DIR}/src" FILES ${STORM_PARS_HEADERS})
target_precompile_headers(storm-pars REUSE_FROM storm)
target_link_libraries(storm-pars PUBLIC storm)

list(APPEND STORM_TARGETS storm-pars)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

# installation
install(TARGETS storm-pars EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL FILE_SET fs_storm_pars_headers)

