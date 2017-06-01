#!/bin/bash
# Script installing dependencies
# Inspired by https://github.com/google/fruit

set -e

# Helper for travis folding
travis_fold() {
  local action=$1
  local name=$2
  echo -en "travis_fold:${action}:${name}\r"
}

# Helper for installing packages via homebrew
install_brew_package() {
  if brew list -1 | grep -q "^$1\$"; then
    # Package is installed, upgrade if needed
    brew outdated "$1" || brew upgrade "$@"
  else
    # Package not installed yet, install.
    # If there are conflicts, try overwriting the files (these are in /usr/local anyway so it should be ok).
    brew install "$@" || brew link --overwrite gcc49
  fi
}

# Update packages
travis_fold start brew_update
brew update
travis_fold end brew_update

travis_fold start brew_install_util
# For md5sum
install_brew_package md5sha1sum
# For `timeout'
install_brew_package coreutils

which cmake &>/dev/null || install_brew_package cmake

# Install compiler
case "${COMPILER}" in
gcc-4.8)       install_brew_package gcc@4.8 ;;
gcc-4.9)       install_brew_package gcc@4.9 ;;
gcc-5)         install_brew_package gcc@5 ;;
gcc-6)         install_brew_package gcc@6 ;;
clang-default) ;;
clang-3.7)     install_brew_package llvm@3.7 --with-clang --with-libcxx;;
clang-3.8)     install_brew_package llvm@3.8 --with-clang --with-libcxx;;
clang-3.9)     install_brew_package llvm@3.9 --with-clang --with-libcxx;;
clang-4.0)     install_brew_package llvm     --with-clang --with-libcxx;;
*) echo "Compiler not supported: ${COMPILER}. See travis_ci_install_osx.sh"; exit 1 ;;
esac
travis_fold end brew_install_util


# Install dependencies
travis_fold start brew_install_dependencies
install_brew_package gmp --c++11
install_brew_package cln
install_brew_package ginac
install_brew_package doxygen
install_brew_package boost --c++11
install_brew_package z3 # optional
brew tap homebrew/science
install_brew_package homebrew/science/glpk
install_brew_package homebrew/science/hwloc
travis_fold end brew_install_dependencies
