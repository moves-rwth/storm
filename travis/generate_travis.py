# Generate .travis.yml automatically

# Configuration for Linux
configs = [
    # OS, OS version, compiler, build type, task
    ("ubuntu", "18.04", "gcc", "DefaultDebug", "Test"),
    ("ubuntu", "18.04", "gcc", "DefaultRelease", "Test"),
    ("debian", "9", "gcc", "DefaultDebug", "Test"),
    ("debian", "9", "gcc", "DefaultRelease", "Test"),
    ("debian", "10", "gcc", "DefaultDebug", "Test"),
    ("debian", "10", "gcc", "DefaultRelease", "Test"),
    ("ubuntu", "20.04", "gcc", "DefaultDebugTravis", "TestDocker"),
    ("ubuntu", "20.04", "gcc", "DefaultReleaseTravis", "TestDockerDoxygen"),
#    ("osx", "xcode9.3", "clang", "DefaultDebug", "Test"),
#    ("osx", "xcode9.3", "clang", "DefaultRelease", "Test"),
]

# Stages in travis
build_stages = [
    ("Build (1st run)", "Build1"),
    ("Build (2nd run)", "Build2"),
    ("Build (3rd run)", "BuildLast"),
    ("Tasks", "Tasks"),
]

def get_env_string(os, os_version, compiler, build_type, task):
    if os == "osx":
        return "CONFIG={} TASK={} COMPILER={} STL=libc++\n".format(build_type, task, compiler)
    else:
        return "CONFIG={} TASK={} LINUX={} COMPILER={}\n".format(build_type, task, "{}-{}".format(os, os_version), compiler)


if __name__ == "__main__":
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
    for config in configs:
        os, os_version, compiler, build_type, task = config
        os_type = "osx" if os == "osx" else "linux"
        if "Travis" in build_type:
            s += "    # {}-{} - {}\n".format(os, os_version, build_type)
            buildConfig = ""
            buildConfig += "    - stage: Build Carl\n"
            buildConfig += "      os: {}\n".format(os_type)
            buildConfig += "      compiler: {}\n".format(compiler)
            buildConfig += "      env: {}".format(get_env_string(os, os_version, compiler, build_type, task))
            buildConfig += "      before_script:\n"
            buildConfig += '        - python -c "import fcntl; fcntl.fcntl(1, fcntl.F_SETFL, 0)" # Workaround for nonblocking mode\n'
            buildConfig += "      script:\n"
            buildConfig += "        - travis/build_carl.sh\n"
            buildConfig += "      before_cache:\n"
            buildConfig += "        - docker cp carl:/opt/carl/. .\n"
            # Upload to DockerHub
            buildConfig += "      deploy:\n"
            buildConfig += "        - provider: script\n"
            buildConfig += "          skip_cleanup: true\n"
            buildConfig += "          script: bash travis/deploy_docker.sh carl\n"
            s += buildConfig

    # Generate all build configurations
    for stage in build_stages:
        s += "\n"
        s += "    ###\n"
        s += "    # Stage: {}\n".format(stage[0])
        s += "    ###\n"
        for config in configs:
            os, os_version, compiler, build_type, task = config
            os_type = "osx" if os == "osx" else "linux"
            s += "    # {}-{} - {}\n".format(os, os_version, build_type)
            buildConfig = ""
            buildConfig += "    - stage: {}\n".format(stage[0])
            buildConfig += "      os: {}\n".format(os_type)
            if os_type == "osx":
                buildConfig += "      osx_image: {}\n".format(os_version)
            buildConfig += "      compiler: {}\n".format(compiler)
            buildConfig += "      env: {}".format(get_env_string(os, os_version, compiler, build_type, task))
            buildConfig += "      install:\n"
            if stage[1] == "Build1":
                buildConfig += "        - rm -rf build\n"
            buildConfig += "        - travis/skip_test.sh\n"
            if os_type == "osx":
                buildConfig += "        - travis/install_osx.sh\n"
            buildConfig += "      before_script:\n"
            buildConfig += '        - python -c "import fcntl; fcntl.fcntl(1, fcntl.F_SETFL, 0)" # Workaround for nonblocking mode\n'
            buildConfig += "      script:\n"
            buildConfig += "        - travis/build.sh {}\n".format(stage[1])
            if os_type == "linux":
                buildConfig += "      before_cache:\n"
                buildConfig += "        - docker cp storm:/opt/storm/. .\n"
            buildConfig += "      after_failure:\n"
            buildConfig += "        - find build -iname '*err*.log' -type f -print -exec cat {} \;\n"

            # Deployment
            if stage[1] == "Tasks":
                if "Docker" in task or "Doxygen" in task:
                    buildConfig += "      deploy:\n"
                if "Docker" in task:
                    buildConfig += "        - provider: script\n"
                    buildConfig += "          skip_cleanup: true\n"
                    buildConfig += "          script: bash travis/deploy_docker.sh storm\n"
                if "Doxygen" in task:
                    buildConfig += "        - provider: pages\n"
                    buildConfig += "          skip_cleanup: true\n"
                    buildConfig += "          github_token: $GITHUB_TOKEN\n"
                    buildConfig += "          local_dir: build/doc/html/\n"
                    buildConfig += "          repo: moves-rwth/storm-doc\n"
                    buildConfig += "          target_branch: master\n"
                    buildConfig += "          on:\n"
                    buildConfig += "            branch: master\n"

            s += buildConfig

    print(s)
