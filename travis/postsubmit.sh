#!/bin/bash -x
# Inspired by https://github.com/google/fruit

set -e

: ${N_JOBS:=2}
: ${TIMEOUT:=2000}

if [ "$STL" != "" ]
then
  STLARG="-stdlib=$STL"
fi

if [ "$2" = "BuildLib1" ]
then
    TIMEARG="--preserve-status"
else
    TIMEARG=""
fi

case $OS in
linux)
    # Execute docker image on linux
    # Stop previous session
    docker rm -f storm &>/dev/null || true
    # Run container
    docker run -d -it --name storm --privileged mvolk/storm-basesystem:$LINUX
    # Copy local content into container
    docker exec storm mkdir storm
    docker cp . storm:/storm

    # Execute main process
    timeout $TIMEOUT $TIMEARG docker exec storm bash -c "
        export COMPILER=$COMPILER;
        export N_JOBS=$N_JOBS;
        export STLARG=$STLARG;
        export OS=$OS;
        cd storm;
        travis/postsubmit-helper.sh $1 $2"
    exit $?
    ;;

osx)
    # Mac OSX
    export COMPILER
    export N_JOBS
    export STLARG
    export OS
    gtimeout $TIMEOUT $TIMEARG travis/postsubmit-helper.sh "$1" "$2"
    exit $?
    ;;

*)
    # Other OS
    echo "Unsupported OS: $OS"
    exit 1
esac
