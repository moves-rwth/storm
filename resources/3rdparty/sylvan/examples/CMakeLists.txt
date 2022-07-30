add_executable(bddmc)
target_sources(bddmc PRIVATE bddmc.c getrss.c getrss.h)
target_include_directories(bddmc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bddmc PRIVATE sylvan::sylvan)
target_compile_features(bddmc PRIVATE c_std_11 cxx_std_11)
target_compile_options(bddmc PRIVATE -Wall -Wextra -Wno-deprecated)

add_executable(lddmc)
target_sources(lddmc PRIVATE lddmc.c getrss.c getrss.h)
target_include_directories(lddmc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lddmc PRIVATE sylvan::sylvan)
target_compile_features(lddmc PRIVATE c_std_11 cxx_std_11)
target_compile_options(lddmc PRIVATE -Wall -Wextra -Wno-deprecated)

add_executable(ldd2bdd)
target_sources(ldd2bdd PRIVATE ldd2bdd.c getrss.c getrss.h)
target_include_directories(ldd2bdd PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ldd2bdd PRIVATE sylvan::sylvan)
target_compile_features(ldd2bdd PRIVATE c_std_11 cxx_std_11)
target_compile_options(ldd2bdd PRIVATE -Wall -Wextra -Wno-deprecated)

add_executable(nqueens)
target_sources(nqueens PRIVATE nqueens.c)
target_link_libraries(nqueens PRIVATE sylvan::sylvan)
target_compile_features(nqueens PRIVATE c_std_11 cxx_std_11)
target_compile_options(nqueens PRIVATE -Wall -Wextra -Wno-deprecated)

add_executable(simple)
target_sources(simple PRIVATE simple.cpp)
target_link_libraries(simple PRIVATE sylvan::sylvan)
target_compile_features(simple PRIVATE c_std_11 cxx_std_11)
target_compile_options(simple PRIVATE -Wall -Wextra -Wno-deprecated)

# Check if we have Meddly
find_library(MEDDLY_FOUND meddly)
if(MEDDLY_FOUND)
    add_executable(ldd2meddly)
    target_sources(ldd2meddly PRIVATE ldd2meddly.cpp)
    target_link_libraries(ldd2meddly PRIVATE sylvan::sylvan stdc++ meddly)
    set_target_properties(ldd2meddly PROPERTIES COMPILE_OPTIONS "-Wno-unused")
    target_compile_features(ldd2meddly PRIVATE c_std_11 cxx_std_11)
    target_compile_options(ldd2meddly PRIVATE -Wall -Wextra -Wno-deprecated)

    add_executable(medmc)
    target_sources(medmc PRIVATE medmc.cpp)
    target_link_libraries(medmc PRIVATE stdc++ meddly)
    set_target_properties(medmc PROPERTIES COMPILE_OPTIONS "-Wno-unused")
    target_compile_features(medmc PRIVATE c_std_11 cxx_std_11)
    target_compile_options(medmc PRIVATE -Wall -Wextra -Wno-deprecated)
endif()

# Add profiler to some of the examples, if available
# In the future, maybe not use gperftools anymore, just Linux perf...
include(CheckIncludeFiles)
check_include_files("gperftools/profiler.h" HAVE_PROFILER)

if(HAVE_PROFILER)
    set_target_properties(bddmc PROPERTIES COMPILE_DEFINITIONS "HAVE_PROFILER")
    target_link_libraries(bddmc PRIVATE profiler)

    set_target_properties(lddmc PROPERTIES COMPILE_DEFINITIONS "HAVE_PROFILER")
    target_link_libraries(lddmc PRIVATE profiler)

    set_target_properties(nqueens PROPERTIES COMPILE_DEFINITIONS "HAVE_PROFILER")
    target_link_libraries(nqueens PRIVATE profiler)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # add argp library for OSX
    find_package(Argp REQUIRED)
    include_directories(${ARGP_INCLUDE_PATH})
    target_link_libraries(bddmc PRIVATE ${ARGP_LIBRARIES})
    target_link_libraries(lddmc PRIVATE ${ARGP_LIBRARIES})
    if(MEDDLY_FOUND)
        target_link_libraries(medmc PRIVATE ${ARGP_LIBRARIES})
        target_link_libraries(ldd2meddly PRIVATE ${ARGP_LIBRARIES})
    endif()
    target_link_libraries(ldd2bdd PRIVATE ${ARGP_LIBRARIES})
    target_link_libraries(nqueens PRIVATE ${ARGP_LIBRARIES})
endif()


