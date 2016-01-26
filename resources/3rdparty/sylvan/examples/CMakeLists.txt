cmake_minimum_required(VERSION 2.6)
project(sylvan C)

set(CMAKE_C_FLAGS "-g -O3 -Wextra -Wall -Werror -fno-strict-aliasing -std=gnu11")

include_directories(.)

add_executable(mc mc.c getrss.h getrss.c)
target_link_libraries(mc sylvan)

add_executable(lddmc lddmc.c getrss.h getrss.c)
target_link_libraries(lddmc sylvan)

include(CheckIncludeFiles)
check_include_files("gperftools/profiler.h" HAVE_PROFILER)

if(HAVE_PROFILER)
    set_target_properties(mc PROPERTIES COMPILE_DEFINITIONS "HAVE_PROFILER")
    target_link_libraries(mc profiler)

    set_target_properties(lddmc PROPERTIES COMPILE_DEFINITIONS "HAVE_PROFILER")
    target_link_libraries(lddmc profiler)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # add argp library for OSX
    target_link_libraries(mc argp)
    target_link_libraries(lddmc argp)
endif()


