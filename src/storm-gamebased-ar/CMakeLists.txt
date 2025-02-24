file(GLOB_RECURSE STORM_GBAR_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-gamebased-ar/*/*.cpp)
file(GLOB_RECURSE STORM_GBAR_HEADERS RELATIVE " ${PROJECT_SOURCE_DIR}/src/storm-gamebased-ar" ${PROJECT_SOURCE_DIR}/src/storm-gamebased-ar/*/*.h)

add_library(storm-gamebased-ar SHARED)
target_sources(storm-gamebased-ar
		PRIVATE
		${STORM_GBAR_SOURCES}
		PUBLIC
		FILE_SET fs_storm_gbar_headers TYPE HEADERS BASE_DIRS "${PROJECT_SOURCE_DIR}/src" FILES ${STORM_GBAR_HEADERS})
target_precompile_headers(storm-gamebased-ar REUSE_FROM storm)
target_link_libraries(storm-gamebased-ar PUBLIC storm)

list(APPEND STORM_TARGETS storm-gamebased-ar)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

# installation
install(TARGETS storm-gamebased-ar EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL FILE_SET fs_storm_gbar_headers)

