project(carlext)
cmake_minimum_required(VERSION 3.3)
include(ExternalProject)

option(STORM_3RDPARTY_BINARY_DIR "3rd party bin dir")

message(STATUS "Carl-Storm - Storm 3rdparty binary dir: ${STORM_3RDPARTY_BINARY_DIR}")

ExternalProject_Add(carl-config
	GIT_REPOSITORY https://github.com/moves-rwth/carl-storm
	GIT_TAG master
	PREFIX here
	SOURCE_DIR source_dir
	BINARY_DIR ${STORM_3RDPARTY_BINARY_DIR}/carl
	CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DBOOST_INCLUDEDIR=${Boost_INCLUDE_DIRS} -DBOOST_LIBRARYDIR=${Boost_LIBRARY_DIRS} -DBoost_NO_SYSTEM_PATHS=ON -DEXPORT_TO_CMAKE=ON -DTHREAD_SAFE=ON -DUSE_CLN_NUMBERS=${Use_CLN_NUMBERS} -DUSE_GINAC=${Use_GINAC} -DCMAKE_SKIP_INSTALL_ALL_DEPENDENCY=ON -DCMAKE_INSTALL_PREFIX:PATH=${STORM_3RDPARTY_BINARY_DIR}/carl
	BUILD_IN_SOURCE 0
	LOG_UPDATE OFF
	LOG_CONFIGURE OFF
	BUILD_COMMAND "" # Disable build step.
	INSTALL_COMMAND "" # Disable install step too.
)
add_custom_target(build-carl)
add_dependencies(build-carl carl-config)

message(STATUS "Carl-Storm - Finished configuration.")
