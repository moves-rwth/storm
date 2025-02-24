file(GLOB_RECURSE STORM_DFT_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-dft/*/*.cpp)
file(GLOB_RECURSE STORM_DFT_HEADERS RELATIVE "${PROJECT_SOURCE_DIR}/src/storm-dft" ${PROJECT_SOURCE_DIR}/src/storm-dft/*/*.h)

# Create storm-dft.
add_library(storm-dft SHARED)
target_sources(storm-dft
        PRIVATE
        ${STORM_DFT_SOURCES}
        PUBLIC
        FILE_SET fs_storm_dft_headers TYPE HEADERS BASE_DIRS "${PROJECT_SOURCE_DIR}/src" FILES ${STORM_DFT_HEADERS})
target_precompile_headers(storm-dft REUSE_FROM storm)
target_link_libraries(storm-dft PUBLIC storm storm-gspn storm-conv storm-parsers storm-pars ${STORM_DFT_LINK_LIBRARIES})

list(APPEND STORM_TARGETS storm-dft)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

# installation
install(TARGETS storm-dft EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL FILE_SET fs_storm_dft_headers )

