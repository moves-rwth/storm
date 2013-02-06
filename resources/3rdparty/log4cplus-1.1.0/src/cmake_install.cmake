# Install script for directory: /Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src

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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/liblog4cplus.a")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblog4cplus.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblog4cplus.a")
    EXECUTE_PROCESS(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblog4cplus.a")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4cplus" TYPE FILE FILES
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/appender.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/asyncappender.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/clogger.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/config.hxx"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/configurator.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/consoleappender.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/fileappender.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/fstreams.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/hierarchy.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/hierarchylocker.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/layout.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/log4judpappender.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/logger.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/loggingmacros.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/loglevel.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/mdc.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/ndc.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/nteventlogappender.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/nullappender.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/socketappender.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/streams.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/syslogappender.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/tchar.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/tracelogger.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/tstring.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/version.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/win32debugappender.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/win32consoleappender.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4cplus/boost" TYPE FILE FILES "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/boost/deviceappender.hxx")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4cplus/helpers" TYPE FILE FILES
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/appenderattachableimpl.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/fileinfo.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/lockfile.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/loglog.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/logloguser.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/pointer.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/property.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/queue.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/sleep.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/snprintf.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/socket.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/socketbuffer.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/stringhelper.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/thread-config.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/helpers/timehelper.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4cplus/internal" TYPE FILE FILES
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/internal/env.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/internal/internal.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/internal/socket.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4cplus/spi" TYPE FILE FILES
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/spi/appenderattachable.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/spi/factory.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/spi/filter.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/spi/loggerfactory.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/spi/loggerimpl.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/spi/loggingevent.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/spi/objectregistry.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/spi/rootlogger.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4cplus/thread/impl" TYPE FILE FILES
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/thread/impl/syncprims-impl.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/thread/impl/syncprims-pthreads.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/thread/impl/syncprims-win32.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/thread/impl/threads-impl.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/thread/impl/tls.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4cplus/thread" TYPE FILE FILES
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/thread/syncprims-pub-impl.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/thread/syncprims.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/thread/threads.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/log4cplus/config" TYPE FILE FILES
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/config/macosx.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/config/win32.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/src/../include/log4cplus/config/windowsh-inc.h"
    "/Users/chris/Documents/workspace/mrmc/MRMC/resources/3rdparty/log4cplus-1.1.0/include/log4cplus/config/defines.hxx"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

