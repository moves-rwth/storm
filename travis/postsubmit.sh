#!/bin/bash
# Inspired by https://github.com/google/fruit

set -e

: ${N_JOBS:=2}

if [ "$STL" != "" ]
then
  STLARG="-stdlib=$STL"
fi

case $OS in
linux)
    # Execute docker image on linux
    docker rm -f storm &>/dev/null || true
    docker run -d -it --name storm --privileged mvolk/storm-basesystem:$LINUX
    docker exec storm mkdir storm
    docker cp . storm:/storm

    docker exec storm bash -c "
        export COMPILER=$COMPILER;
        export N_JOBS=$N_JOBS;
        export STLARG=$STLARG;
        export OS=$OS;
        cd storm;
        travis/postsubmit-helper.sh $1"
    exit $?
    ;;

osx)
    # Mac OSX
    export COMPILER
    export N_JOBS
    export STLARG
    export OS
    travis/postsubmit-helper.sh "$1"
    exit $?
    ;;

*)
    # Other OS
    echo "Unsupported OS: $OS"
    exit 1
esac
