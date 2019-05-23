#!/bin/bash -x

set -e

OS=$TRAVIS_OS_NAME

# Do not deploy if credentials are not given
if [ "${TRAVIS_SECURE_ENV_VARS}" == "false" ]; then
    echo "WARNING: Not deploying as no credentials are given."
    exit 0;
fi

# Do not deploy for pull requests
if [ "${TRAVIS_PULL_REQUEST}" != "false" ]; then
    exit 0;
fi

echo "Deploying $1 to Dockerhub"

case $OS in
linux)
    echo "$DOCKER_PASSWORD" | docker login -u "$DOCKER_USERNAME" --password-stdin
    # Deploy as debug/release
    case "$CONFIG" in
    *DebugTravis)
        docker commit $1 movesrwth/$1:travis-debug
        docker push movesrwth/$1:travis-debug
        ;;
    *ReleaseTravis)
        docker commit $1 movesrwth/$1:travis
        docker push movesrwth/$1:travis
        ;;
    *)
        echo "Unrecognized value of CONFIG: $CONFIG"; exit 1
        ;;
    esac
    ;;

osx)
    echo "Docker deployment on Mac OSX not used."
    exit 1
    ;;

*)
    # Unknown OS
    echo "Unsupported OS: $OS"
    exit 1
esac
