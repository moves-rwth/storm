# Create storm-gspn.
add_executable(storm-gspn-cli ${PROJECT_SOURCE_DIR}/src/storm-gspn-cli/storm-gspn.cpp)
target_link_libraries(storm-gspn-cli storm-gspn storm-cli-utilities) # Adding headers for xcode
set_target_properties(storm-gspn-cli PROPERTIES OUTPUT_NAME "storm-gspn")

add_dependencies(binaries storm-gspn-cli)

# installation
install(TARGETS storm-gspn-cli EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
