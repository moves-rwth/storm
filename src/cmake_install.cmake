# Install script for directory: /Users/jipspel/Documents/Tools/copies-storm/public-storm/src

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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-counterexamples/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-parsers/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-version-info/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-cli-utilities/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-pgcl/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-pgcl-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-gspn/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-gspn-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-dft/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-dft-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-pars/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-pars-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-pomdp/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-pomdp-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-conv/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/storm-conv-cli/cmake_install.cmake")
  include("/Users/jipspel/Documents/Tools/copies-storm/public-storm/src/test/cmake_install.cmake")

endif()

