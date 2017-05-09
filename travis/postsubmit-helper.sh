#!/bin/bash -x
# Inspired by https://github.com/google/fruit

set -e

# This only exists in OS X, but it doesn't cause issues in Linux (the dir doesn't exist, so it's
# ignored).
export PATH="/usr/local/opt/coreutils/libexec/gnubin:$PATH"

case $COMPILER in
gcc-4.8)
    export CC=gcc-4.8
    export CXX=g++-4.8
    ;;

gcc-4.9)
    export CC=gcc-4.9
    export CXX=g++-4.9
    ;;

gcc-5)
    export CC=gcc-5
    export CXX=g++-5
    ;;

gcc-6)
    export CC=gcc-6
    export CXX=g++-6
    ;;

clang-3.5)
    export CC=clang-3.5
    export CXX=clang++-3.5
    ;;

clang-3.6)
    export CC=clang-3.6
    export CXX=clang++-3.6
    ;;

clang-3.7)
    export CC=clang-3.7
    export CXX=clang++-3.7
    ;;

clang-3.8)
    export CC=clang-3.8
    export CXX=clang++-3.8
    ;;

clang-3.9)
    export CC=clang-3.9
    export CXX=clang++-3.9
    ;;

clang-4.0)
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

clang-default)
    export CC=clang
    export CXX=clang++
    ;;

*)
    echo "Unrecognized value of COMPILER: $COMPILER"
    exit 1
esac

travis_fold() {
  local action=$1
  local name=$2
  echo -en "travis_fold:${action}:${name}\r"
}

run_make() {
  travis_fold start make
  #make storm -j$N_JOBS
  #make resources -j$N_JOBS
  make -j$N_JOBS
  travis_fold end make
}

# Build
echo CXX version: $($CXX --version)
echo C++ Standard library location: $(echo '#include <vector>' | $CXX -x c++ -E - | grep 'vector\"' | awk '{print $3}' | sed 's@/vector@@;s@\"@@g' | head -n 1)
echo Normalized C++ Standard library location: $(readlink -f $(echo '#include <vector>' | $CXX -x c++ -E - | grep 'vector\"' | awk '{print $3}' | sed 's@/vector@@;s@\"@@g' | head -n 1))

case "$1" in
DebugPlain)           CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Debug   -DCMAKE_CXX_FLAGS="$STLARG") ;;
ReleasePlain)         CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$STLARG") ;;
*) echo "Error: you need to specify one of the supported postsubmit modes (see postsubmit.sh)."; exit 1 ;;
esac

# Restore timestamps of files
travis_fold start mtime
ruby travis/mtime_cache/mtime_cache.rb -g travis/mtime_cache/globs.txt -c travis/mtime_cache/cache.json
travis_fold end mtime

travis_fold start cmake
#rm -rf build
mkdir -p build
cd build
cmake .. "${CMAKE_ARGS[@]}"
echo
echo "Content of CMakeFiles/CMakeError.log:"
if [ -f "CMakeFiles/CMakeError.log" ]
then
  cat CMakeFiles/CMakeError.log
fi
echo
travis_fold end cmake
run_make
