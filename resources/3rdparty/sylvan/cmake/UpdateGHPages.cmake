# Copyright (c) 2011-2013 Thomas Heller
# Modified by Tom van Dijk
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

find_package(Git)

if(NOT GIT_FOUND)
    message(FATAL_ERROR "Git not found!")
endif()

if(NOT GHPAGES_REPOSITORY)
    set(GHPAGES_REPOSITORY git@github.com:trolando/sylvan.git --branch gh-pages)
endif()

if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/gh-pages")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" pull --rebase
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gh-pages"
        RESULT_VARIABLE git_pull_result)
    if(NOT "${git_pull_result}" EQUAL "0")
        message(FATAL_ERROR "Updating the GitHub pages branch failed.")
    endif()
else()
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone ${GHPAGES_REPOSITORY} gh-pages
        RESULT_VARIABLE git_clone_result)
    if(NOT "${git_clone_result}" EQUAL "0")
        message(FATAL_ERROR "Cloning the GitHub pages branch failed. Trying to clone ${GHPAGES_REPOSITORY}")
    endif()
endif()

# first delete all files
file(REMOVE_RECURSE "${CMAKE_CURRENT_BINARY_DIR}/gh-pages/*")

# copy all documentation files to target branch
file(COPY "${CMAKE_CURRENT_BINARY_DIR}/html/"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/gh-pages"
     PATTERN ".doctrees" EXCLUDE
     PATTERN ".buildinfo" EXCLUDE
     )

# git add -A *
execute_process(
    COMMAND "${GIT_EXECUTABLE}" add -A *
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gh-pages"
    RESULT_VARIABLE git_add_result)
if(NOT "${git_add_result}" EQUAL "0")
    message(FATAL_ERROR "Adding files to the GitHub pages branch failed.")
endif()

# check if there are changes to commit
execute_process(
    COMMAND "${GIT_EXECUTABLE}" diff-index --quiet HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gh-pages"
    RESULT_VARIABLE git_diff_index_result)
if(NOT "${git_diff_index_result}" EQUAL "0")
    # commit changes
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" commit -m "Updated documentation"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gh-pages"
        RESULT_VARIABLE git_commit_result)
    if(NOT "${git_commit_result}" EQUAL "0")
        message(FATAL_ERROR "Commiting to the GitHub pages branch failed.")
    endif()

    # push everything up to github
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" push
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gh-pages"
        RESULT_VARIABLE git_push_result)
    if(NOT "${git_push_result}" EQUAL "0")
        message(FATAL_ERROR "Pushing to the GitHub pages branch failed.")
    endif()
endif()
