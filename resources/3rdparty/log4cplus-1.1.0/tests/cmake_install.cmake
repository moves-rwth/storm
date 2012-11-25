# Install script for directory: /Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/appender_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/configandwatch_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/customloglevel_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/fileappender_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/filter_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/hierarchy_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/loglog_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/ndc_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/ostream_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/patternlayout_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/performance_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/priority_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/propertyconfig_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/thread_test/cmake_install.cmake")
  INCLUDE("/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/tests/timeformat_test/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

