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

        if [[ "$CONFIG" == "*Travis" ]]
        then
            # Build Carl separately
            travis_fold start install_carl
            cd ..
            git clone https://github.com/smtrat/carl.git
            cd carl
            mkdir build
            cd build
            cmake .. "${CARL_CMAKE_ARGS[@]}"
            make lib_carl -j$N_JOBS
            cd ../../storm
            travis_fold end install_carl
        fi


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

  TestAll)
    # Test all
    travis_fold start test_all
    cd build
    ctest test --output-on-failure
    travis_fold end test_all
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
    CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Debug -DSTORM_DEVELOPER=ON -DCMAKE_CXX_FLAGS="$STLARG")
    CARL_CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="$STLARG" -DUSE_CLN_NUMBERS=ON -DUSE_GINAC=ON -DTHREAD_SAFE=ON -DBUILD_ADDONS=ON -DBUILD_ADDON_PARSER=ON)
    ;;
DefaultRelease*)
    CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Release -DSTORM_DEVELOPER=OFF -DCMAKE_CXX_FLAGS="$STLARG")
    CARL_CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$STLARG" -DUSE_CLN_NUMBERS=ON -DUSE_GINAC=ON -DTHREAD_SAFE=ON -DBUILD_ADDONS=ON -DBUILD_ADDON_PARSER=ON)
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
run "$1"
kill $bellPID

