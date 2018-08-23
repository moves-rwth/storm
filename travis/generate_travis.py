# Generate .travis.yml automatically

# Configuration for Linux
configs_linux = [
    # OS, compiler, build type
    ("debian-9", "gcc", "DefaultDebug"),
    ("debian-9", "gcc", "DefaultRelease"),
    ("ubuntu-18.04", "gcc", "DefaultDebugTravis"),
    ("ubuntu-18.04", "gcc", "DefaultReleaseTravis"),
    ("ubuntu-18.04", "gcc", "DefaultDebug"),
    ("ubuntu-18.04", "gcc", "DefaultRelease"),
]

# Configurations for Mac
configs_mac = [
    # OS, compiler, build type
#    ("osx", "clang", "DefaultDebug"),
#    ("osx", "clang", "DefaultRelease"),
]

# Stages in travis
stages = [
    ("Build (1st run)", "Build1"),
    ("Build (2nd run)", "Build2"),
    ("Build (3rd run)", "Build3"),
    ("Build (4th run)", "BuildLast"),
    ("Test all", "TestAll"),
]


if __name__ == "__main__":
    allow_failures = []

    s = ""
    # Initial config
    s += "#\n"
    s += "# General config\n"
    s += "#\n"
    s += "branches:\n"
    s += "  only:\n"
    s += "  - master\n"
    s += "  - stable\n"
    s += "sudo: required\n"
    s += "dist: trusty\n"
    s += "language: cpp\n"
    s += "\n"
    s += "git:\n"
    s += "  depth: false\n"
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
    s += "\n"

    s += "notifications:\n"
    s += "  email:\n"
    s += "    on_failure: always\n"
    s += "    on_success: change\n"
    s += "    recipients:\n"
    s += '    - secure: "VWnsiQkt1xjgRo1hfNiNQqvLSr0fshFmLV7jJlUixhCr094mgD0U2bNKdUfebm28Byg9UyDYPbOFDC0sx7KydKiL1q7FKKXkyZH0k04wUu8XiNw+fYkDpmPnQs7G2n8oJ/GFJnr1Wp/1KI3qX5LX3xot4cJfx1I5iFC2O+p+ng6v/oSX+pewlMv4i7KL16ftHHHMo80N694v3g4B2NByn4GU2/bjVQcqlBp/TiVaUa5Nqu9DxZi/n9CJqGEaRHOblWyMO3EyTZsn45BNSWeQ3DtnMwZ73rlIr9CaEgCeuArc6RGghUAVqRI5ao+N5apekIaILwTgL6AJn+Lw/+NRPa8xclgd0rKqUQJMJCDZKjKz2lmIs3bxfELOizxJ3FJQ5R95FAxeAZ6rb/j40YqVVTw2IMBDnEE0J5ZmpUYNUtPti/Adf6GD9Fb2y8sLo0XDJzkI8OxYhfgjSy5KYmRj8O5MXcP2MAE8LQauNO3MaFnL9VMVOTZePJrPozQUgM021uyahf960+QNI06Uqlmg+PwWkSdllQlxHHplOgW7zClFhtSUpnJxcsUBzgg4kVg80gXUwAQkaDi7A9Wh2bs+TvMlmHzBwg+2SaAfWDgjeJIeOaipDkF1uSGzC+EHAiiKYMLd4Aahoi8SuelJUucoyJyLAq00WdUFQIh/izVhM4Y="\n'
    s += "\n"
    s += "#\n"
    s += "# Configurations\n"
    s += "#\n"
    s += "jobs:\n"
    s += "  include:\n"

    # Start with prebuilding carl for docker
    s += "\n"
    s += "    ###\n"
    s += "    # Stage: Build Carl\n"
    s += "    ###\n"
    s += "\n"
    for config in configs_linux:
        linux = config[0]
        compiler = config[1]
        build_type = config[2]
        if "Travis" in build_type:
            s += "    # {} - {}\n".format(linux, build_type)
            buildConfig = ""
            buildConfig += "    - stage: Build Carl\n"
            buildConfig += "      os: linux\n"
            buildConfig += "      compiler: {}\n".format(compiler)
            buildConfig += "      env: CONFIG={} LINUX={} COMPILER={}\n".format(build_type, linux, compiler)
            buildConfig += "      install:\n"
            buildConfig += "        - travis/install_linux.sh\n"
            buildConfig += "      script:\n"
            buildConfig += "        - travis/build_carl.sh\n"
            # Upload to DockerHub
            buildConfig += "      after_success:\n"
            buildConfig += '        - docker login -u "$DOCKER_USERNAME" -p "$DOCKER_PASSWORD";\n'
            if "Debug" in build_type:
                buildConfig += "        - docker commit carl movesrwth/carl:travis-debug;\n"
                buildConfig += "        - docker push movesrwth/carl:travis-debug;\n"
            elif "Release" in build_type:
                buildConfig += "        - docker commit carl movesrwth/carl:travis;\n"
                buildConfig += "        - docker push movesrwth/carl:travis;\n"
            else:
                assert False
            s += buildConfig

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
            compiler = config[1]
            build_type = config[2]
            s += "    # {} - {}\n".format(osx, build_type)
            buildConfig = ""
            buildConfig += "    - stage: {}\n".format(stage[0])
            buildConfig += "      os: osx\n"
            buildConfig += "      osx_image: xcode9.1\n"
            buildConfig += "      compiler: {}\n".format(compiler)
            buildConfig += "      env: CONFIG={} COMPILER={} STL=libc++\n".format(build_type, compiler)
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
            allow_fail = ""
            linux = config[0]
            compiler = config[1]
            build_type = config[2]
            s += "    # {} - {}\n".format(linux, build_type)
            buildConfig = ""
            buildConfig += "    - stage: {}\n".format(stage[0])
            allow_fail += "    - stage: {}\n".format(stage[0])
            buildConfig += "      os: linux\n"
            allow_fail += "      os: linux\n"
            buildConfig += "      compiler: {}\n".format(compiler)
            buildConfig += "      env: CONFIG={} LINUX={} COMPILER={}\n".format(build_type, linux, compiler)
            allow_fail += "      env: CONFIG={} LINUX={} COMPILER={}\n".format(build_type, linux, compiler)
            buildConfig += "      install:\n"
            if stage[1] == "Build1":
                buildConfig += "        - rm -rf build\n"
            buildConfig += "        - travis/install_linux.sh\n"
            buildConfig += "      script:\n"
            buildConfig += "        - travis/build.sh {}\n".format(stage[1])
            buildConfig += "      before_cache:\n"
            buildConfig += "        - docker cp storm:/opt/storm/. .\n"
            buildConfig += "      after_failure:\n"
            buildConfig += "        - find build -iname '*err*.log' -type f -print -exec cat {} \;\n"
            # Upload to DockerHub
            if stage[1] == "TestAll" and "Travis" in build_type:
                buildConfig += "      after_success:\n"
                buildConfig += '        - docker login -u "$DOCKER_USERNAME" -p "$DOCKER_PASSWORD";\n'
                if "Debug" in build_type:
                    buildConfig += "        - docker commit storm movesrwth/storm:travis-debug;\n"
                    buildConfig += "        - docker push movesrwth/storm:travis-debug;\n"
                elif "Release" in build_type:
                    buildConfig += "        - docker commit storm movesrwth/storm:travis;\n"
                    buildConfig += "        - docker push movesrwth/storm:travis;\n"
                else:
                    assert False
            s += buildConfig
            if "Travis" in build_type and "Release" in build_type:
                allow_failures.append(allow_fail)

    if len(allow_failures) > 0:
        s += "  allow_failures:\n"
        for fail in allow_failures:
            s += fail
    print(s)
