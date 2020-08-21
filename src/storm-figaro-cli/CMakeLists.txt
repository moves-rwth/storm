add_executable(storm-figaro-cli ${PROJECT_SOURCE_DIR}/src/storm-figaro-cli/storm-figaro.cpp)
target_link_libraries(storm-figaro-cli storm-figaro  storm-cli-utilities)
set_target_properties(storm-figaro-cli PROPERTIES OUTPUT_NAME "storm-figaro")

add_dependencies(binaries storm-figaro-cli)

# installation
install(TARGETS storm-figaro-cli EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
