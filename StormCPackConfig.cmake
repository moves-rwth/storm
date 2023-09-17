include(InstallRequiredSystemLibraries)

# For help take a look at:
# http://www.cmake.org/Wiki/CMake:CPackConfiguration

### general settings
set(CPACK_PACKAGE_NAME "Storm")
set(CPACK_PACKAGE_VENDOR "RWTH Aachen University")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Storm - A probabilistic model checker written in C++.")
set(CPACK_PACKAGE_CONTACT "support@stormchecker.org")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

### versions
set(CPACK_PACKAGE_VERSION_MAJOR "${STORM_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${STORM_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${STORM_VERSION_PATCH}")

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

### source package settings
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_IGNORE_FILES "~$;[.]swp$;/[.]svn/;/[.]git/;.gitignore;/build/;tags;cscope.*")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-src")

# Dependencies to create DEB File
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev,libcln-dev,libgmp-dev,libginac-dev,automake,libglpk-dev,libhwloc-dev,libz3-dev,libxerces-c-dev,libeigen3-dev")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})

include(CPack)
