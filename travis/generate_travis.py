
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
    ("Build dependencies", "BuildDep"),
    ("Build library (1st run)", "BuildLib1"),
    ("Build library (2nd run)", "BuildLib"),
    ("Build all", "BuildAll"),
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
    s += "  timeout: 600\n"
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
                buildConfig += "      env: BUILD={} COMPILER={} LINUX={}\n".format(build, compiler, linux)
                buildConfig += "      install: export OS=linux; export COMPILER='{}'; export LINUX='{}';\n".format(compiler, linux)
                buildConfig += "        travis/install_linux.sh\n"
                buildConfig += "      script: export OS=linux; export COMPILER='{}'; export LINUX='{}';\n".format(compiler, linux)
                buildConfig += "        travis/postsubmit.sh {} {}\n".format(build, stage[1])
                buildConfig += "      before_cache:\n"
                buildConfig += "        docker cp storm:/storm/. .\n"
            s += buildConfig

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
                buildConfig += "      env: BUILD={} COMPILER={} STL=libc++\n".format(build, compiler)
                buildConfig += "      install: export OS=osx; export COMPILER='{}'; export STL='libc++';\n".format(compiler)
                buildConfig += "        travis/install_osx.sh\n"
                buildConfig += "      script: export OS=osx; export COMPILER='{}'; export STL='libc++';\n".format(compiler)
                buildConfig += "        travis/postsubmit.sh {} {}\n".format(build, stage[1])
            s += buildConfig

    print(s)
