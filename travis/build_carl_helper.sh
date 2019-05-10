#!/bin/bash

set -e

# Helper for travis folding
travis_fold() {
  local action=$1
  local name=$2
  echo -en "travis_fold:${action}:${name}\r"
}

# Helper for building and testing
run() {
  travis_fold start install_dependencies
  apt-get update
  #apt-get install -qq -y openjdk-8-jdk maven uuid-dev pkg-config
  apt-get install -qq -y uuid-dev pkg-config
  travis_fold end install_dependencies

  travis_fold start install_carl
  git clone https://github.com/smtrat/carl.git
  cd carl
  git checkout master14
  mkdir build
  cd build
  cmake .. "${CMAKE_ARGS[@]}"
  #make lib_carl addons -j$N_JOBS
  make lib_carl -j$N_JOBS
  travis_fold end install_carl
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
    CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="$STLARG" -DUSE_CLN_NUMBERS=ON -DUSE_GINAC=ON -DTHREAD_SAFE=ON -DBUILD_ADDONS=ON -DBUILD_ADDON_PARSER=ON)
    ;;
DefaultRelease*)
    CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$STLARG" -DUSE_CLN_NUMBERS=ON -DUSE_GINAC=ON -DTHREAD_SAFE=ON -DBUILD_ADDONS=ON -DBUILD_ADDON_PARSER=ON)
    ;;
*)
    echo "Unrecognized value of CONFIG: $CONFIG"; exit 1
    ;;
esac

run
