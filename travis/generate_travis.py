
# Configuration for Linux
configs_linux = [
    # OS, compiler
    ("ubuntu-16.10", "gcc", "-6"),
    ("debian-9", "gcc", "-6"),
]

# Configurations for Mac
configs_mac = [
    # OS, compiler
    ("osx", "clang", "-4.0"),
]

# Build types
build_types = [
    "DefaultDebug",
    "DefaultRelease",
]

# Stages in travis
stages = [
    ("Build (1st run)", "Build1"),
    ("Build (2nd run)", "Build2"),
    ("Build (3rd run)", "Build3"),
    ("Build (4th run)", "Build4"),
    ("Test all", "TestAll"),
]


if __name__ == "__main__":
    s = ""
    # Initial config
    s += "# This file was inspired from https://github.com/google/fruit\n"
    s += "\n"
    s += "#\n"
    s += "# General config\n"
    s += "#\n"
    s += "branches:\n"
    s += "  only:\n"
    s += "  - master\n"
    s += "dist: trusty\n"
    s += "language: cpp\n"
    s += "\n"
    s += "# Enable caching\n"
    s += "cache:\n"
    s += "  timeout: 1000\n"
    s += "  directories:\n"
    s += "  - build\n"
    s += "  - travis/mtime_cache\n"
    s += "\n"
    s += "# Enable docker support\n"
    s += "services:\n"
    s += "- docker\n"
    s += "sudo: required\n"
    s += "\n"
    s += "#\n"
    s += "# Configurations\n"
    s += "#\n"
    s += "jobs:\n"
    s += "  include:\n"

    # Generate all configurations
    for stage in stages:
        s += "\n"
        s += "    ###\n"
        s += "    # Stage: {}\n".format(stage[0])
        s += "    ###\n"
        s += "\n"
        # Mac OS X
        for config in configs_mac:
            osx = config[0]
            compiler = "{}{}".format(config[1], config[2])
            s += "    # {}\n".format(osx)
            buildConfig = ""
            for build in build_types:
                buildConfig += "    - stage: {}\n".format(stage[0])
                buildConfig += "      os: osx\n"
                buildConfig += "      compiler: {}\n".format(config[1])
                buildConfig += "      env: CONFIG={} COMPILER={} STL=libc++\n".format(build, compiler)
                buildConfig += "      install:\n"
                if stage[1] == "Build1":
                    buildConfig += "        - rm -rf build\n"
                buildConfig += "        - travis/install_osx.sh\n"
                buildConfig += "      script:\n"
                buildConfig += "        - travis/build.sh {}\n".format(stage[1])
                buildConfig += "      after_failure:\n"
                buildConfig += "        - find build -iname '*err*.log' -type f -print -exec cat {} \;\n"
            s += buildConfig

        # Linux via Docker
        for config in configs_linux:
            linux = config[0]
            compiler = "{}{}".format(config[1], config[2])
            s += "    # {}\n".format(linux)
            buildConfig = ""
            for build in build_types:
                buildConfig += "    - stage: {}\n".format(stage[0])
                buildConfig += "      os: linux\n"
                buildConfig += "      compiler: {}\n".format(config[1])
                buildConfig += "      env: CONFIG={} LINUX={} COMPILER={}\n".format(build, linux, compiler)
                buildConfig += "      install:\n"
                if stage[1] == "Build1":
                    buildConfig += "        - rm -rf build\n"
                buildConfig += "        - travis/install_linux.sh\n"
                buildConfig += "      script:\n"
                buildConfig += "        - travis/build.sh {}\n".format(stage[1])
                buildConfig += "      before_cache:\n"
                buildConfig += "        - docker cp storm:/storm/. .\n"
                buildConfig += "      after_failure:\n"
                buildConfig += "        - find build -iname '*err*.log' -type f -print -exec cat {} \;\n"
            s += buildConfig

    print(s)
