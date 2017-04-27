#! /bin/bash

# v2017-04-21
# This is a hacky build script for building Storm on Debian Jessie.
# As both the GCC compiler and other dependencies are not current enough,
# we build our own versions inside a user directory.
#
# This script takes a conservative approach and builds current releases of
# several dependencies, even in cases where the package shipped with Debian
# Jessie would work. This way, we have libraries with known version numbers,
# which provides a more stable base for benchmarking.
#
# All the dependencies will be built and installed in a single
# directory (default $HOME/storm-root), the rest of your installation should
# hopefully not be affected. Note that the resulting directory will take
#  several GB of space.
#
#
# Configuration:
# You can set several environment variables on the command-line before running
#
#
# export STORM_ROOT=/directory/where/to/build/and/install/storm/and/dependencies
#  Default = $HOME/storm-root
#
# export PROCS=x
#  Perform compilation using x parallel processes, default = 1
#
# export STORM_RELEASE=1.0.0
#  Optional, build the given Storm release (git tag). Default is the HEAD
#  of the git repository
#
#
# export DONT_CLEAN_DEPENDENCIES=1
#  Optional. If this environment variable is set, don't clean the build
#  artefacts of the dependencies after they have been installed to the $STORM_ROOT
#  directory.
#  The Carl library and Storm will not be cleaned, as you might want to change things there.
#
#
#
# - IMPORTANT ---------------------------------------------------------------------------
# Before running the script, you should ensure that the folloing packages are installed,
# e.g., by running
#
#  sudo apt update
#  sudo apt install build-essential libmpfr-dev libgmp-dev libmpc-dev m4 pkg-config git libcurl4-openssl-dev zlib1g-dev libbz2-dev automake libhwloc-dev libpython-dev
# --------------------------------------------------------------------------------------
#
# The dependencies will be built in the src subdirectory of $STORM_ROOT.
#
# Run with
#   storm-build-debian-jessie.sh run
#
# Once a particular dependency has been built, a file of the form .have-foo will be created.
# If such a file exists, on subsequent runs this dependency will not be rebuilt. This might
# save time when some later-stage component fails.
#
# Remove the .have-foo file to rebuild the dependencies. If there are major problems, consider
# deleting the complete $STORM_ROOT directory...
#
# If you run into trouble or have some improvements, feel free to mail
# Joachim Klein <klein@tcs.inf.tu-dresden.de>
#
#
# Explanation for the Debian package dependencies:
# - build-essential: Compiler for building cmake and bootstrapping our version of GCC
# - libmpfr-dev, libgmp-dev, libmpc-dev: GCC dependencies
# - m4, automake: autotools, for various packages
# - pkg-config: Provides library path configuration support, needed for ginac
# - git: Needed for cloning various repositories
# - libcurl4-openssl-dev: Support for cmake to fetch https URLS
# - zlib1g-dev libbz2-dev: Compression libraries for boost
# - libpython-dev: Python for boost
# - libhwloc-dev: Dependency of storm


if [ -z "$1" ]; then
    cat <<HERE
-----------------------------------------------------------------------------------
Build script for Storm and dependencies on Debian Jessie.
-----------------------------------------------------------------------------------

First, ensure that the required dependencies available in the Debian package
system are installed, e.g., using

$ sudo apt update
$ sudo apt install build-essential libmpfr-dev libgmp-dev libmpc-dev m4 pkg-config git libcurl4-openssl-dev zlib1g-dev libbz2-dev automake libhwloc-dev libpython-dev

Then, run the build script with the 'run' parameter, i.e., with

$ ./storm-build-debian-jessie.sh run

By default, this will install Storm and various dependencies in the ${HOME}/storm-root directory.
You can change this by specifying the STORM_ROOT environment variable before running this script:

$ export STORM_ROOT=/path/to/storm-root

To use multiple CPUs for parallel building, you can specify the number of CPUs via the
PROCS environment variable before running the script, e.g.,

$ export PROCS=4

For further documentation and configuration options, have a look at the beginning of the script file.

-----------------------------------------------------------------------------------
HERE
    exit 1
elif [ "$1" != "run" ]; then
    echo "Unknown argument '$1', run script without arguments for a brief description."
    exit 1
fi


PROCS=${PROCS-1}
echo "Using $PROCS CPUs for parallel compilation"

set -x    # We want to see what commands are executed
set -e    # We want the script to abort, if any command fails

# regularize DONT_CLEAN_DEPENDENCIES
if [ ! -z "$DONT_CLEAN_DEPENDENCIES" ]; then
    DONT_CLEAN_DEPENDENCIES=1;
else
    DONT_CLEAN_DEPENDENCIES='';
fi

STORM_ROOT=${STORM_ROOT:-$HOME/storm-root}

####################################################################
# The following environment variables provide configuration
# for the build steps. If you want to manually perform
# the build steps, please set these variables beforehand
# as well.
####################################################################
export STORM_ROOT
export PATH=$STORM_ROOT/bin:"$PATH"
export PKG_CONFIG_PATH=$STORM_ROOT/lib/pkgconfig

echo "Using STORM_ROOT = $STORM_ROOT"



mkdir -p $STORM_ROOT
cd $STORM_ROOT

mkdir -p src

#
# Note: The sequence of building all the various dependencies is important.
#

#
# We bootstrap a current version of cmake first, using the normal Debian Jessie
# compiler (from build-essential)
#

test -f $STORM_ROOT/.have-cmake || (
cd src                       &&
rm -f cmake-3.7.2.tar.gz     &&
rm -rf cmake-3.7.2           &&
wget https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz    &&
tar xzf cmake-3.7.2.tar.gz   &&
cd cmake-3.7.2               &&
./configure --prefix=$STORM_ROOT --system-curl &&
make -j${PROCS} install      &&
cd $STORM_ROOT               &&
touch $STORM_ROOT/.have-cmake &&
([ -n "$DONT_CLEAN_DEPENDENCIES" ] || rm -rf src/cmake-3.7.2 src/cmake-3.7.2.tar.gz)
) || exit 1


#
# Then, we build our own GCC 6.3.0
#

test -f $STORM_ROOT/.have-gcc || (
cd src                       &&
rm -f gcc-6.3.0.tar.bz2      &&
rm -rf obj-gcc-6.3.0         &&
rm -rf gcc-6.3.0             &&
wget ftp://ftp.gwdg.de/pub/misc/gcc/releases/gcc-6.3.0/gcc-6.3.0.tar.bz2   &&
tar xjf gcc-6.3.0.tar.bz2    &&
mkdir obj-gcc-6.3.0          &&
cd obj-gcc-6.3.0             &&
`pwd`/../gcc-6.3.0/configure --prefix=$STORM_ROOT --enable-lto --enable-languages=c,c++ --disable-bootstrap --disable-multilib  &&
make -j${PROCS}              &&
make install                 &&
ln -s $STORM_ROOT/bin/gcc $STORM_ROOT/bin/cc &&     # GCC seems to not install cc, so we add a link...
cd $STORM_ROOT               &&
touch $STORM_ROOT/.have-gcc  &&
([ -n "$DONT_CLEAN_DEPENDENCIES" ] || rm -rf src/gcc-6.3.0.tar.bz2 src/obj-gcc-6.3.0 src/gcc-6.3.0)
) || exit 1


#
# Build up-to-date GMP library
#

test -f $STORM_ROOT/.have-gmp || (
cd src                       &&
rm -f gmp-6.1.2.tar.xz       &&
rm -rf gmp-6.1.2             &&
wget https://gmplib.org/download/gmp/gmp-6.1.2.tar.xz   &&
tar xf gmp-6.1.2.tar.xz      &&
cd gmp-6.1.2                 &&
./configure --prefix=$STORM_ROOT --enable-cxx  &&
make -j${PROCS} install      &&
cd $STORM_ROOT               &&
touch $STORM_ROOT/.have-gmp  &&
([ -n "$DONT_CLEAN_DEPENDENCIES" ] || rm -rf src/gmp-6.1.2.tar.xz src/gmp-6.1.2)
) || exit 1


#
# Build CLN library
#

test -f $STORM_ROOT/.have-cln || (
cd src                       &&
rm -f cln-1.3.4.tar.bz2      &&
rm -rf cln-1.3.4             &&
wget http://www.ginac.de/CLN/cln-1.3.4.tar.bz2  &&
tar xjf cln-1.3.4.tar.bz2    &&
cd cln-1.3.4                 &&
./configure --prefix=$STORM_ROOT  &&
make -j${PROCS} install      &&
cd $STORM_ROOT               &&
touch $STORM_ROOT/.have-cln  &&
([ -n "$DONT_CLEAN_DEPENDENCIES" ] || rm -rf src/cln-1.3.4.tar.bz2 src/cln-1.3.4)
) || exit 1


#
# Build ginac
#

test -f $STORM_ROOT/.have-ginac || (
cd src                      &&
rm -f ginac-1.7.2.tar.bz2   &&
rm -rf ginac-1.7.2          &&
wget http://www.ginac.de/ginac-1.7.2.tar.bz2   &&
tar xjf ginac-1.7.2.tar.bz2   &&
cd ginac-1.7.2                &&
./configure --prefix=$STORM_ROOT  &&
make -j${PROCS} install     &&
cd $STORM_ROOT               &&
touch $STORM_ROOT/.have-ginac &&
([ -n "$DONT_CLEAN_DEPENDENCIES" ] || rm -rf src/ginac-1.7.2.tar.bz2 src/ginac-1.7.2)
) || exit 1


#
# Build glpk
#

test -f $STORM_ROOT/.have-glpk || (
cd src                     &&
rm -f glpk-4.61.tar.gz     &&
rm -rf glpk-4.61           &&
wget https://ftpmirror.gnu.org/glpk/glpk-4.61.tar.gz  &&
tar xzf glpk-4.61.tar.gz   &&
cd glpk-4.61               &&
./configure --prefix=$STORM_ROOT --with-gmp  &&
make -j${PROCS} install    &&
cd $STORM_ROOT               &&
touch $STORM_ROOT/.have-glpk &&
([ -n "$DONT_CLEAN_DEPENDENCIES" ] || rm -rf src/glpk-4.61.tar.gz src/glpk-4.61)
) || exit 1


#
# Build xerces
#

test -f $STORM_ROOT/.have-xerces || (
cd src                       &&
rm -f xerces-c-3.1.4.tar.gz  &&
rm -rf xerces-c-3.1.4        &&
wget http://mirror.netcologne.de/apache.org//xerces/c/3/sources/xerces-c-3.1.4.tar.gz   &&
tar xzf xerces-c-3.1.4.tar.gz  &&
cd xerces-c-3.1.4              &&
./configure --prefix=$STORM_ROOT  &&
make -j${PROCS} install     &&
cd $STORM_ROOT               &&
touch $STORM_ROOT/.have-xerces &&
([ -n "$DONT_CLEAN_DEPENDENCIES" ] || rm -rf src/xerces-c-3.1.4.tar.gz src/xerces-c-3.1.4)
) || exit 1

#
# Build z3
#

test -f $STORM_ROOT/.have-z3 || (
cd src                      &&
rm -f z3-4.5.0.tar.gz       &&
rm -rf z3-z3-4.5.0          &&
wget https://github.com/Z3Prover/z3/archive/z3-4.5.0.tar.gz &&
tar xzf z3-4.5.0.tar.gz     &&
cd z3-z3-4.5.0              &&
# released version of z3 has problems with reading localized GCC version
# message, so we set LANG=C. Fixed on trunk
LANG=C ./configure --prefix=$STORM_ROOT  &&
cd build                   &&
make -j${PROCS} install    &&
cd $STORM_ROOT               &&
touch $STORM_ROOT/.have-z3 &&
([ -n "$DONT_CLEAN_DEPENDENCIES" ] || rm -rf src/z3-4.5.0.tar.gz src/z3-z3-4.5.0)
) || exit 1


#
# Fetch and build boost
#

test -f $STORM_ROOT/.have-boost || (
cd src                    &&
rm -f boost_1_61_0.tar.bz2   &&
rm -rf boost_1_61_0       &&
wget https://sourceforge.net/projects/boost/files/boost/1.61.0/boost_1_61_0.tar.bz2/download -O boost_1_61_0.tar.bz2  &&
tar xjf boost_1_61_0.tar.bz2  &&
cd boost_1_61_0               &&
./bootstrap.sh --prefix=$STORM_ROOT  &&
./b2 install -j${PROCS} --layout=tagged  &&
cd $STORM_ROOT               &&
touch $STORM_ROOT/.have-boost &&
([ -n "$DONT_CLEAN_DEPENDENCIES" ] || rm -rf src/boost_1_61_0.tar.bz2 src/boost_1_61_0)
) || exit 1


#
# Fetch and build carl
#

test -f $STORM_ROOT/.have-carl || (
cd src                   &&
rm -rf carl.git          &&
rm -rf carl-build        &&
git clone https://github.com/smtrat/carl.git carl.git   &&
cd carl.git              &&
git checkout 8391970879463e46d581291c3bbfbceee6d013c5  &&
cd ..                    &&
mkdir carl-build         &&
cd carl-build            &&
#
# carl currently does not search for ginac in CMAKE_PREFIX_PATH, so we have to provide the paths...
#
cmake ../carl.git -DCMAKE_INSTALL_PREFIX=$STORM_ROOT -DCMAKE_PREFIX_PATH=$STORM_ROOT -DUSE_GINAC=ON -DUSE_CLN_NUMBERS=ON -DTHREAD_SAFE=ON -DGINAC_INCLUDE_DIR=$STORM_ROOT/include/ginac -DGINAC_LIBRARY=$STORM_ROOT/lib/libginac.so &&
make -j${PROCS} install lib_carl  &&
touch $STORM_ROOT/.have-carl
) || exit 1


#
# Finally, fetch and build storm
#

test -f $STORM_ROOT/.have-storm || (
cd src                 &&
rm -rf storm.git       &&
rm -rf storm-build     &&
git clone https://github.com/moves-rwth/storm.git storm.git   &&
if [[ -z "$STORM_RELEASE" ]]; then
    echo "Building HEAD of storm"
else
    echo "Building release tag $STORM_RELEASE of storm"
    (cd storm.git; git checkout "tags/$STORM_RELEASE")
fi
mkdir storm-build     &&
cd storm-build        &&
cmake ../storm.git -DCMAKE_INSTALL_PREFIX=$STORM_ROOT -DCMAKE_PREFIX_PATH=$STORM_ROOT  &&
make -j${PROCS} install  &&
touch $STORM_ROOT/.have-storm
) || exit 1


set +x
echo
echo "Looks like we made it! Storm binaries should be installed in " $STORM_ROOT/bin
echo "Enjoy!"
