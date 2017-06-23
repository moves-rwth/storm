#!/bin/bash -x
# Inspired by https://github.com/google/fruit

: ${N_JOBS:=2}
: ${TIMEOUT_MAC:=1800}
: ${TIMEOUT_LINUX:=2000}

if [ "$STL" != "" ]
then
  STLARG="-stdlib=$STL"
fi

EXITCODE=42

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
        export COMPILER=$COMPILER;
        export N_JOBS=$N_JOBS;
        export STLARG=$STLARG;
        export OS=$OS;
        cd storm;
        travis/build-helper.sh $1 $2"
    EXITCODE=$?
    ;;

osx)
    # Mac OSX
    export COMPILER
    export N_JOBS
    export STLARG
    export OS
    gtimeout $TIMEOUT_MAC travis/build-helper.sh "$1" "$2"
    EXITCODE=$?
    ;;

*)
    # Unknown OS
    echo "Unsupported OS: $OS"
    exit 1
esac

if [[ $EXITCODE == 124 ]] && [[ "$2" == Build* ]] && [[ "$2" != "Build4" ]]
then
    exit 0
else
    exit $EXITCODE
fi
