cmake_minimum_required(VERSION 2.6)
project(sylvan C CXX)

set(CMAKE_C_FLAGS "-g -O3 -Wextra -Wall -Werror -fno-strict-aliasing -std=gnu11")

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
    tls.h
)

# We need to make sure that the binary is put into a folder that is independent of the
# build type. Otherwise -- for example when using Xcode -- the binary might end up in a
# sub-folder "Debug" or "Release".
set_target_properties(sylvan PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(sylvan m pthread)

include(CheckIncludeFiles)
check_include_files(hwloc.h HAVE_HWLOC)

if(HAVE_HWLOC)
    set_target_properties(sylvan PROPERTIES COMPILE_DEFINITIONS "USE_HWLOC=1")
    target_link_libraries(sylvan hwloc)
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
