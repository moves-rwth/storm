#!/bin/bash -x
# Inspired by https://github.com/google/fruit

N_JOBS=2
TIMEOUT_MAC=1600
TIMEOUT_LINUX=2300

OS=$TRAVIS_OS_NAME

EXITCODE=42

# Skip this run?
if [ -f build/skip.txt ]
then
  # Remove flag s.t. tests will be executed
  if [[ "$1" == "Build4" ]]
  then
    rm build/skip.txt
  fi
  exit 0
fi

case $OS in
linux)
    # Execute docker image on Linux
    # Stop previous session
    docker rm -f storm &>/dev/null
    # Run container
    set -e
    docker run -d -it --name storm --privileged mvolk/storm-basesystem:$LINUX
    # Copy local content into container
    docker exec storm mkdir storm
    docker cp . storm:/storm
    set +e

    # Execute main process
    timeout $TIMEOUT_LINUX docker exec storm bash -c "
        export CONFIG=$CONFIG;
        export COMPILER=$COMPILER;
        export N_JOBS=$N_JOBS;
        export STLARG=;
        export OS=$OS;
        cd storm;
        travis/build-helper.sh $1"
    EXITCODE=$?
    ;;

osx)
    # Mac OSX
    STLARG="-stdlib=libc++"
    export CONFIG=$CONFIG
    export COMPILER
    export N_JOBS
    export STLARG
    export OS
    gtimeout $TIMEOUT_MAC travis/build-helper.sh "$1"
    EXITCODE=$?
    ;;

*)
    # Unknown OS
    echo "Unsupported OS: $OS"
    exit 1
esac

if [[ $EXITCODE == 124 ]] && [[ "$1" == Build* ]] && [[ "$1" != "Build4" ]]
then
    exit 0
else
    exit $EXITCODE
fi
