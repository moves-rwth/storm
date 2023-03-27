cmake_minimum_required(VERSION 3.13)

project(sylvan
    VERSION 1.7.1
    DESCRIPTION "Sylvan, a parallel decision diagram library"
    HOMEPAGE_URL "https://github.com/trolando/sylvan"
    LANGUAGES C CXX
)

# Add CMake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")

# Add the Sylvan library target
add_subdirectory(src)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # If we are in the root, add some options to build examples/tests
    option(SYLVAN_BUILD_EXAMPLES "Build example tools" ON)
    option(SYLVAN_BUILD_DOCS "Build documentation" OFF)
    option(SYLVAN_BUILD_TESTS "Build tests" ON)

    # Build examples
    if(SYLVAN_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()

    # Make documentation
    if(SYLVAN_BUILD_DOCS)
        configure_file("docs/conf.py.in" "docs/conf.py" @ONLY)
        find_package(Sphinx REQUIRED)
        Sphinx_add_targets(sylvan ${CMAKE_CURRENT_BINARY_DIR}/docs ${CMAKE_CURRENT_SOURCE_DIR}/docs ${CMAKE_CURRENT_BINARY_DIR})
        add_custom_target(update_gh_pages COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_MODULE_PATH}/UpdateGHPages.cmake")
        add_dependencies(update_gh_pages sylvan_html)
    endif()

    # Add tests
    if(SYLVAN_BUILD_TESTS)
        enable_testing()
        add_subdirectory(test)
    endif()
endif()


