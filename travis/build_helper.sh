#!/bin/bash

set -e

# Helper for travis folding
travis_fold() {
  local action=$1
  local name=$2
  echo -en "travis_fold:${action}:${name}\r"
}

# Helper to write output every minute
function bell() {
  while true; do
    echo "travis_wait for it..."
    sleep 60
  done
}

# Helper for distinguishing between different runs
run() {
  case "$1" in
  Build*)
    if [[ "$1" == "Build1" ]]
    then
        # CMake
        travis_fold start cmake
        mkdir build
        cd build
        cmake .. "${CMAKE_ARGS[@]}"
        echo
        if [ -f "CMakeFiles/CMakeError.log" ]
        then
          echo "Content of CMakeFiles/CMakeError.log:"
          cat CMakeFiles/CMakeError.log
        fi
        echo
        cd ..
        travis_fold end cmake
    fi

    # Make
    travis_fold start make
    cd build
    make -j$N_JOBS
    travis_fold end make
    # Set skip-file
    if [[ "$1" != "BuildLast" ]]
    then
        touch skip.txt
    else
        rm -rf skip.txt
    fi
    ;;

  Tasks)
    # Perform tasks
    if [[ "$TASK" == *Test* ]]
    then
        # Test all
        travis_fold start test_all
        cd build
        ctest test --output-on-failure
        travis_fold end test_all

        # Check correctness of build types
        echo "Checking correctness of build types"
        case "$CONFIG" in
        DefaultDebug*)
            ./bin/storm --version | grep "with flags .* -g" || (echo "Error: Missing flag '-g' for debug build." && return 1)
            ;;
        DefaultRelease*)
            ./bin/storm --version | grep "with flags .* -O3" || (echo "Error: Missing flag '-O3' for release build." && return 1)
            ./bin/storm --version | grep "with flags .* -DNDEBUG" || (echo "Error: Missing flag '-DNDEBUG' for release build." && return 1)
            ;;
        *)
            echo "Unrecognized value of CONFIG: $CONFIG"
            exit 1
        esac
        cd ..
    fi

    if [[ "$TASK" == *Doxygen* ]]
    then
        # Generate doxygen doc
        travis_fold start make_doc
        cd build
        make -j$N_JOBS doc
        # Disable jekyll as otherwise files with starting underscore are not published
        echo "" > doc/html/.nojekyll
        cd ..
        travis_fold end make_doc
    fi
    ;;

  *)
    echo "Unrecognized value of run: $1"
    exit 1
  esac
}


# This only exists in OS X, but it doesn't cause issues in Linux (the dir doesn't exist, so it's
# ignored).
export PATH="/usr/local/opt/coreutils/libexec/gnubin:$PATH"

case $COMPILER in
gcc-6)
    export CC=gcc-6
    export CXX=g++-6
    ;;

gcc)
    export CC=gcc
    export CXX=g++
    ;;

clang-4)
    case "$OS" in
    linux)
        export CC=clang-4.0
        export CXX=clang++-4.0
        ;;
    osx)
        export CC=/usr/local/opt/llvm/bin/clang-4.0
        export CXX=/usr/local/opt/llvm/bin/clang++
        ;;
    *) echo "Error: unexpected OS: $OS"; exit 1 ;;
    esac
    ;;

clang)
    export CC=clang
    export CXX=clang++
    ;;

*)
    echo "Unrecognized value of COMPILER: $COMPILER"
    exit 1
esac

# Build
echo CXX version: $($CXX --version)
echo C++ Standard library location: $(echo '#include <vector>' | $CXX -x c++ -E - | grep 'vector\"' | awk '{print $3}' | sed 's@/vector@@;s@\"@@g' | head -n 1)
echo Normalized C++ Standard library location: $(readlink -f $(echo '#include <vector>' | $CXX -x c++ -E - | grep 'vector\"' | awk '{print $3}' | sed 's@/vector@@;s@\"@@g' | head -n 1))

case "$CONFIG" in
DefaultDebug*)
    CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Debug -DSTORM_DEVELOPER=ON -DSTORM_PORTABLE=ON -DCMAKE_CXX_FLAGS="$STLARG")
    ;;
DefaultRelease*)
    CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Release -DSTORM_DEVELOPER=OFF -DSTORM_PORTABLE=ON -DCMAKE_CXX_FLAGS="$STLARG")
    ;;
*)
    echo "Unrecognized value of CONFIG: $CONFIG"; exit 1
    ;;
esac

# Restore timestamps of files
travis_fold start mtime
if [[ "$1" == "Build1" ]]
then
    # Remove old mtime cache
    rm -rf travis/mtime_cache/cache.json
fi
ruby travis/mtime_cache/mtime_cache.rb -g travis/mtime_cache/globs.txt -c travis/mtime_cache/cache.json
travis_fold end mtime

# Run and print output to avoid travis timeout
bell &
bellPID=$!
trap 'rc=$?; kill $bellPID; exit $rc' EXIT
run "$1"

