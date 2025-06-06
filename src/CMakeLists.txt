add_custom_target(binaries)


add_subdirectory(storm)
add_subdirectory(storm-counterexamples)
add_subdirectory(storm-permissive)
add_subdirectory(storm-gamebased-ar)
add_subdirectory(storm-parsers)
add_subdirectory(storm-version-info)
add_subdirectory(storm-conv)
add_subdirectory(storm-dft)
add_subdirectory(storm-gspn)
add_subdirectory(storm-pars)
add_subdirectory(storm-pomdp)
# We also build the CLI utilities, as other libraries may be built upon them.
add_subdirectory(storm-cli-utilities)

if(STORM_BUILD_EXECUTABLES)
    # Typically an idea to switch this off, if you want to build a library on top of Storm.
    add_subdirectory(storm-cli)
    add_subdirectory(storm-conv-cli)
    add_subdirectory(storm-dft-cli)
    add_subdirectory(storm-gspn-cli)
    add_subdirectory(storm-pars-cli)
    add_subdirectory(storm-pomdp-cli)
endif()

if (STORM_BUILD_TESTS)
    add_subdirectory(test)
endif()

set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)
