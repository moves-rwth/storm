include(InstallRequiredSystemLibraries)

### General settings
# General project information is taken from the project() variables
set(CPACK_PACKAGE_VENDOR "Storm Developers")
set(CPACK_PACKAGE_CONTACT "support@stormchecker.org")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

set(CPACK_GENERATOR "DEB")

### Source package settings
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "~$;[.]swp$;/[.]svn/;/[.]git/;.gitignore;/build/;tags;cscope.*")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-src")

# Debian package
set(CPACK_DEBIAN_PACKAGE_DEPENDS "cmake, automake, libboost-all-dev, libcln-dev, libginac-dev, libglpk-dev, libgmp-dev, libhwloc-dev, libeigen3-dev, libxerces-c-dev, libz3-dev")

include(CPack)
