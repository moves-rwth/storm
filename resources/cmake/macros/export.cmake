# Add all targets to the build-tree export set
export(EXPORT storm_Targets FILE "${PROJECT_BINARY_DIR}/stormTargets.cmake")

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
     VERSION ${STORM_VERSION_MAJOR}.${STORM_VERSION_MINOR}.${STORM_VERSION_PATCH}.${STORM_VERSION_TWEAK}
     COMPATIBILITY SameMinorVersion )


configure_package_config_file(
        resources/cmake/stormConfig.cmake.in
        ${PROJECT_BINARY_DIR}/stormConfig.cmake
        INSTALL_DESTINATION ${STORM_CMAKE_INSTALL_DIR}
        PATH_VARS STORM_INCLUDE_INSTALL_DIR
)
#
# # For the install tree
# TODO is this needed maybe?
#set(CONF_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/include/")
##file(RELATIVE_PATH REL_INCLUDE_DIR "${STORM_CMAKE_INSTALL_DIR}" "${STORM_INCLUDE_INSTALL_DIR}")
##set(CONF_INCLUDE_DIRS "\${storm_CMAKE_DIR}/${REL_INCLUDE_DIR}/storm")
#
#configure_package_config_file(
#    resources/cmake/stormConfig.cmake.in
#    ${PROJECT_BINARY_DIR}/stormConfig.install.cmake
#    INSTALL_DESTINATION ${STORM_CMAKE_INSTALL_DIR}
#    PATH_VARS STORM_INCLUDE_INSTALL_DIR
#)
