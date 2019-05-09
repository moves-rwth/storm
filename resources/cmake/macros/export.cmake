
# Add all targets to the build-tree export set
export(TARGETS ${STORM_TARGETS} FILE "${PROJECT_BINARY_DIR}/stormTargets.cmake")

message(STATUS "Registered with cmake")
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE storm)

set(DEP_TARGETS "")
foreach(dt ${STORM_DEP_TARGETS})
	export_target(DEP_TARGETS ${dt})
endforeach()

set(EXP_OPTIONS "")
foreach(option ${EXPORTED_OPTIONS})
    set(EXP_OPTIONS "${EXP_OPTIONS}\nset(${option} \"${${option}}\")")
endforeach()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/stormConfigVersion.cmake
     VERSION 0.1.0
     COMPATIBILITY SameMajorVersion )

# For the build tree
set(CONF_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/include/")
configure_package_config_file(
        resources/cmake/stormConfig.cmake.in
        ${PROJECT_BINARY_DIR}/stormConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_DIR}
        PATH_VARS INCLUDE_INSTALL_DIR
)

 # For the install tree
file(RELATIVE_PATH REL_INCLUDE_DIR "${CMAKE_INSTALL_DIR}" "${INCLUDE_INSTALL_DIR}")
set(CONF_INCLUDE_DIRS "\${storm_CMAKE_DIR}/${REL_INCLUDE_DIR}/storm")

configure_package_config_file(
    resources/cmake/stormConfig.cmake.in
    ${PROJECT_BINARY_DIR}/stormConfig.install.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DIR}
    PATH_VARS INCLUDE_INSTALL_DIR
)
