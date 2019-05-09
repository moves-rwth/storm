#!/bin/bash -x

N_JOBS=2
TIMEOUT_LINUX=2300

OS=$TRAVIS_OS_NAME

case $OS in
linux)
    # Execute docker image on Linux
    # Stop previous session
    docker rm -f carl &>/dev/null
    # Run container
    set -e
    docker run -d -it --name carl --privileged movesrwth/storm-basesystem:$LINUX
    # Copy local content into container
    docker cp travis/build_carl_helper.sh carl:/opt/
    set +e

    # Execute main process
    docker exec carl bash -c "
        export CONFIG=$CONFIG;
        export COMPILER=$COMPILER;
        export N_JOBS=$N_JOBS;
        export STLARG=;
        export OS=$OS;
        cd /opt/;
        timeout $TIMEOUT_LINUX ./build_carl_helper.sh"
    ;;

osx)
    echo "Building carl on Mac OSX not used."
    exit 1
    ;;

*)
    # Unknown OS
    echo "Unsupported OS: $OS"
    exit 1
esac

