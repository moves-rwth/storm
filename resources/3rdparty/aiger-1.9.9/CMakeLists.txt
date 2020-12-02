cmake_minimum_required(VERSION 3.1)

project(aiger LANGUAGES C VERSION 1.9.9)

set(PROJECT_DESCRIPTION "Aiger, a library for And-Inverter Graphs")
set(PROJECT_URL "http://fmv.jku.at/aiger/")

message(STATUS "CMake build configuration for Aiger ${PROJECT_VERSION}")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# Some info
message(STATUS "OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER_ID}")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
set(CMAKE_C_FLAGS "-O2 -Wextra -Wall -fno-strict-aliasing ${CMAKE_C_FLAGS}")

include_directories(${CMAKE_SOURCE_DIR}/src)

include_directories("${PROJECT_SOURCE_DIR}/../../../src")
include_directories("${PROJECT_BINARY_DIR}/../../../include")
include_directories(src)

add_subdirectory(src)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Library for And-Inverter Graphs")
set(CPACK_PACKAGE_VENDOR "Armin Biere")
set(CPACK_PACKAGE_CONTACT "Armin Biere <biere@jku.at>")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_PATCH_VERSION})
include(CPack)

# pkg-config
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/aiger.pc.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/aiger.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/aiger.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
