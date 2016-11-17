# Test Sources
# Note that the tests also need the source files, except for the main file
include_directories(${GTEST_INCLUDE_DIR})


configure_file (
		"${PROJECT_SOURCE_DIR}/src/test/storm-test-config.h.in"
		"${PROJECT_BINARY_DIR}/src/test/storm-config.h"
)

foreach (testsuite adapter builder logic modelchecker parser permissiveschedulers solver storage transformer utility)

	  file(GLOB_RECURSE TEST_${testsuite}_FILES ${STORM_CPP_TESTS_BASE_PATH}/${testsuite}/*.h ${STORM_CPP_TESTS_BASE_PATH}/${testsuite}/*.cpp)
      add_executable (test-${testsuite} ${TEST_${testsuite}_FILES})
	  target_link_libraries(test-${testsuite} storm)
	  target_link_libraries(test-${testsuite} ${STORM_TEST_LINK_LIBRARIES})

	  add_dependencies(test-${testsuite} test-resources)
	  add_test(run-test-${testsuite} test-${testsuite})
endforeach ()



#INSTALL(TARGETS storm-functional-tests
#	RUNTIME DESTINATION bin
#	LIBRARY DESTINATION lib
#	ARCHIVE DESTINATION lib
#)
