#!/bin/bash -x

N_JOBS=2
TIMEOUT_MAC=1600
TIMEOUT_LINUX=2300

OS=$TRAVIS_OS_NAME

EXITCODE=42

# Skip this run?
if [ -f build/skip.txt ]
then
  # Remove flag s.t. tests will be executed
  if [[ "$1" == "BuildLast" ]]
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
    case "$CONFIG" in
    *DebugTravis)
        docker run -d -it --name storm --privileged movesrwth/carl:travis-debug
        ;;
    *ReleaseTravis)
        docker run -d -it --name storm --privileged movesrwth/carl:travis
        ;;
    *)
        docker run -d -it --name storm --privileged movesrwth/storm-basesystem:$LINUX
        ;;
    esac
    # Install doxygen if necessary
    if [[ "$TASK" == *Doxygen* ]]
    then
        docker exec storm apt-get install -qq -y doxygen graphviz
    fi
    # Copy local content into container
    docker exec storm mkdir /opt/storm
    docker cp . storm:/opt/storm
    set +e

    # Execute main process
    docker exec storm bash -c "
        export CONFIG=$CONFIG;
        export TASK=$TASK;
        export COMPILER=$COMPILER;
        export N_JOBS=$N_JOBS;
        export STLARG=;
        export OS=$OS;
        cd /opt/storm;
        timeout $TIMEOUT_LINUX ./travis/build_helper.sh $1"
    EXITCODE=$?
    ;;

osx)
    # Mac OSX
    STLARG="-stdlib=libc++"
    export CONFIG
    export TASK
    export COMPILER
    export N_JOBS
    export STLARG
    export OS
    gtimeout $TIMEOUT_MAC travis/build_helper.sh "$1"
    EXITCODE=$?
    ;;

*)
    # Unknown OS
    echo "Unsupported OS: $OS"
    exit 1
esac

if [[ $EXITCODE == 124 ]] && [[ "$1" == Build* ]] && [[ "$1" != "BuildLast" ]]
then
    exit 0
else
    exit $EXITCODE
fi
