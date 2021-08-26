# Base path for test files
set(STORM_TESTS_BASE_PATH "${PROJECT_SOURCE_DIR}/src/test/storm-pars")

# Test Sources
file(GLOB_RECURSE ALL_FILES ${STORM_TESTS_BASE_PATH}/*.h ${STORM_TESTS_BASE_PATH}/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" test)

# Note that the tests also need the source files, except for the main file
include_directories(${GTEST_INCLUDE_DIR})

foreach (testsuite analysis modelchecker utility derivative)

	  file(GLOB_RECURSE TEST_${testsuite}_FILES ${STORM_TESTS_BASE_PATH}/${testsuite}/*.h ${STORM_TESTS_BASE_PATH}/${testsuite}/*.cpp)
      add_executable (test-pars-${testsuite} ${TEST_${testsuite}_FILES} ${STORM_TESTS_BASE_PATH}/storm-test.cpp analysis/MonotonicityCheckerTest.cpp)
	  target_link_libraries(test-pars-${testsuite} storm-pars storm-parsers)
	  target_link_libraries(test-pars-${testsuite} ${STORM_TEST_LINK_LIBRARIES})

	  add_dependencies(test-pars-${testsuite} test-resources)
	  add_test(NAME run-test-pars-${testsuite} COMMAND $<TARGET_FILE:test-pars-${testsuite}>)
      add_dependencies(tests test-pars-${testsuite})
	
endforeach ()
