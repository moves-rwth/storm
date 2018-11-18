#!/bin/bash -x

set -e

OS=$TRAVIS_OS_NAME

case $OS in
linux)
    # Only deploy for non pull requests
    if [ "${TRAVIS_PULL_REQUEST}" = "false" ]; then
        echo "$DOCKER_PASSWORD" | docker login -u "$DOCKER_USERNAME" --password-stdin
        # Deploy as debug/release
        case "$CONFIG" in
        *DebugTravis)
            docker commit storm movesrwth/storm:travis-debug
            docker push movesrwth/storm:travis-debug
            ;;
        *ReleaseTravis)
            docker commit storm movesrwth/storm:travis
            docker push movesrwth/storm:travis
            ;;
        *)
            echo "Unrecognized value of CONFIG: $CONFIG"; exit 1
            ;;
        esac
    fi
    ;;

osx)
    echo "Building Storm on Mac OSX not used."
    exit 1
    ;;

*)
    # Unknown OS
    echo "Unsupported OS: $OS"
    exit 1
esac

