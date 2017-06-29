set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(storm)
add_subdirectory(storm-pgcl)
add_subdirectory(storm-pgcl-cli)
add_subdirectory(storm-gspn)
add_subdirectory(storm-gspn-cli)
add_subdirectory(storm-dft)
add_subdirectory(storm-dft-cli)
add_subdirectory(storm-pars)
add_subdirectory(storm-pars-cli)

add_subdirectory(test)

set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)
