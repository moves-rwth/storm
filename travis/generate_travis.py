
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
    ("Build library", "BuildLib"),
    ("Build all", "BuildAll"),
    ("Test all", "TestAll"),
]


if __name__ == "__main__":
    s = ""

    # Generate all configurations
    # Linux via Docker
    for config in configs_linux:
        linux = config[0]
        compiler = "{}{}".format(config[1], config[2])
        s += "\n"
        s += "  ###\n"
        s += "  # {}\n".format(linux)
        s += "  ###\n"
        s += "\n"
        for build in build_types:
            for stage in stages:
                s += "  - stage: {}\n".format(stage[0])
                s += "    os: linux\n"
                s += "    compiler: {}\n".format(config[1])
                s += "    env: BUILD={} COMPILER={} LINUX={}\n".format(build, compiler, linux)
                s += "    install: export OS=linux; export COMPILER='{}'; export LINUX='{}'\n".format(compiler, linux)
                s += "      travis/install_linux.sh\n"
                s += "    script: export OS=linux; export COMPILER='{}'; export LINUX='{}'\n".format(compiler, linux)
                s += "      travis/postsubmit.sh {} {}".format(build, stage[1])
                s += "    before_cache:\n"
                s += "      docker cp storm:/storm/. .\n"

    # Mac OS X
    for config in configs_mac:
        osx = config[0]
        compiler = "{}{}".format(config[1], config[2])
        s += "\n"
        s += "  ###\n"
        s += "  # {}\n".format(osx)
        s += "  ###\n"
        s += "\n"
        for build in build_types:
            for stage in stages:
                s += "  - stage: {}\n".format(stage[0])
                s += "    os: osx\n"
                s += "    compiler: {}\n".format(config[1])
                s += "    env: BUILD={} COMPILER={} STL=libc++\n".format(build, compiler)
                s += "    install: export OS=osx; export COMPILER='{}'; export STL='libc++'\n".format(compiler)
                s += "      travis/install_osx.sh\n"
                s += "    script: export OS=osx; export COMPILER='{}'; export STL='libc++'\n".format(compiler)
                s += "      travis/postsubmit.sh {} {}".format(build, stage[1])

    print(s)
