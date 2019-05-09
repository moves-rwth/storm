# Base path for test files
set(STORM_TESTS_BASE_PATH "${PROJECT_SOURCE_DIR}/src/test/storm")

# Test Sources
file(GLOB_RECURSE ALL_FILES ${STORM_TESTS_BASE_PATH}/*.h ${STORM_TESTS_BASE_PATH}/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" test)

# Note that the tests also need the source files, except for the main file
include_directories(${GTEST_INCLUDE_DIR})

foreach (testsuite abstraction adapter builder logic modelchecker parser permissiveschedulers solver storage transformer utility)

	  file(GLOB_RECURSE TEST_${testsuite}_FILES ${STORM_TESTS_BASE_PATH}/${testsuite}/*.h ${STORM_TESTS_BASE_PATH}/${testsuite}/*.cpp)
      add_executable (test-${testsuite} ${TEST_${testsuite}_FILES} ${STORM_TESTS_BASE_PATH}/storm-test.cpp)
	  target_link_libraries(test-${testsuite} storm storm-parsers storm-conv)
	  target_link_libraries(test-${testsuite} ${STORM_TEST_LINK_LIBRARIES})

	  add_dependencies(test-${testsuite} test-resources)
	  add_test(NAME run-test-${testsuite} COMMAND $<TARGET_FILE:test-${testsuite}>)
      add_dependencies(tests test-${testsuite})
	
endforeach ()
