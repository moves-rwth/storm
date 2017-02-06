cmake_minimum_required(VERSION 2.6)
project(sylvan C CXX)

add_library(sylvan
    avl.h
    lace.h
    lace.c
    llmsset.c
    llmsset.h
    refs.h
    refs.c
    sha2.h
    sha2.c
    stats.h
    stats.c
    storm_function_wrapper.h
    storm_function_wrapper.cpp
    sylvan.h
    sylvan_bdd.h
    sylvan_bdd.c
    sylvan_cache.h
    sylvan_cache.c
    sylvan_config.h
    sylvan_common.h
    sylvan_common.c
    sylvan_gmp.h
    sylvan_gmp.c
    sylvan_ldd.h
    sylvan_ldd.c
    sylvan_mtbdd.h
    sylvan_mtbdd.c
    sylvan_mtbdd_int.h
    sylvan_obj.hpp
    sylvan_obj.cpp
    sylvan_storm_rational_function.h
    sylvan_storm_rational_function.c
    tls.h
)

# We need to make sure that the binary is put into a folder that is independent of the
# build type. Otherwise -- for example when using Xcode -- the binary might end up in a
# sub-folder "Debug" or "Release".
set_target_properties(sylvan PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(sylvan m pthread gmp)

if(USE_CARL)
   message(STATUS "Sylvan - linking CARL.")
   target_link_libraries(sylvan ${carl_LIBRARIES})
endif(USE_CARL)

if(UNIX AND NOT APPLE)
    target_link_libraries(sylvan rt)
endif()

option(USE_HWLOC "Use HWLOC library if available" ON)

if(USE_HWLOC)
    include(CheckIncludeFiles)
    check_include_files(hwloc.h HAVE_HWLOC)
    if(HAVE_HWLOC)
        set_target_properties(sylvan PROPERTIES COMPILE_DEFINITIONS "USE_HWLOC=1")
        target_link_libraries(sylvan hwloc)
    endif()
endif()

option(SYLVAN_STATS "Collect statistics" OFF)
if(SYLVAN_STATS)
    set_target_properties(sylvan PROPERTIES COMPILE_DEFINITIONS "SYLVAN_STATS")
endif()

install(TARGETS
    sylvan
    DESTINATION "lib")

install(FILES 
    lace.h
    llmsset.h
    sylvan.h
    sylvan_cache.h
    sylvan_common.h
    sylvan_config.h
    sylvan_bdd.h
    sylvan_ldd.h
    sylvan_mtbdd.h
    sylvan_obj.hpp
    tls.h
    DESTINATION "include")
