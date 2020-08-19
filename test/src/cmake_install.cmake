# Install script for directory: /Users/jipspel/Documents/Tools/storm/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-counterexamples/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-parsers/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-version-info/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-cli-utilities/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-pgcl/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-pgcl-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-gspn/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-gspn-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-dft/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-dft-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-pars/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-pars-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-pomdp/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-pomdp-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-conv/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/storm-conv-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/storm/test/src/test/cmake_install.cmake")

endif()

