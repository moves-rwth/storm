#! /bin/bash

PROCS=${PROCS-1}
echo "Using $PROCS CPUs for parallel compilation"

set -x
set -e

export STORM_ROOT=$HOME/storm-root
export PATH=$STORM_ROOT/bin:"$PATH"
# needed for ginac
export PKG_CONFIG_PATH=$STORM_ROOT/lib/pkgconfig

# sudo apt install libmpfr-dev libgmp-dev libmpc-dev m4 pkg-config git libcurl4-openssl-dev zlib1g-dev libbz2-dev automake libhwloc-dev

mkdir -p src

(
cd src
wget https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz
tar xzf cmake-3.7.2.tar.gz
cd cmake-3.7.2
./configure --prefix=$STORM_ROOT --system-curl
make -j${PROCS} install
)

(
cd src
wget ftp://ftp.gwdg.de/pub/misc/gcc/releases/gcc-6.3.0/gcc-6.3.0.tar.bz2
tar xjf gcc-6.3.0.tar.bz2
mkdir obj-gcc-6.3.0
cd obj-gcc-6.3.0
`pwd`/../gcc-6.3.0/configure --prefix=$STORM_ROOT --enable-lto --enable-languages=c,c++ --disable-bootstrap --disable-multilib
make -j${PROCS}
make install
)

(
cd src
wget https://gmplib.org/download/gmp/gmp-6.1.2.tar.xz
tar xf gmp-6.1.2.tar.xz
cd gmp-6.1.2
./configure --prefix=$STORM_ROOT --enable-cxx
make -j${PROCS} install
)

(
cd src
wget http://www.ginac.de/CLN/cln-1.3.4.tar.bz2
tar xjf cln-1.3.4.tar.bz2
cd cln-1.3.4
./configure --prefix=$STORM_ROOT
make -j${PROCS} install
)

(
cd src
wget http://www.ginac.de/ginac-1.7.2.tar.bz2
tar xjf ginac-1.7.2.tar.bz2
cd ginac-1.7.2
./configure --prefix=$STORM_ROOT
make -j${PROCS} install
)

(
cd src
wget https://ftpmirror.gnu.org/glpk/glpk-4.61.tar.gz
tar xzf glpk-4.61.tar.gz
cd glpk-4.61
./configure --prefix=$STORM_ROOT --with-gmp
make -j${PROCS} install
)

(
cd src
wget http://mirror.netcologne.de/apache.org//xerces/c/3/sources/xerces-c-3.1.4.tar.gz
tar xzf xerces-c-3.1.4.tar.gz
cd xerces-c-3.1.4
./configure --prefix=$STORM_ROOT
make -j${PROCS} install
)


(
cd src
wget https://github.com/Z3Prover/z3/archive/z3-4.5.0.tar.gz
tar xzf z3-4.5.0.tar.gz
cd z3-z3-4.5.0
# released version of z3 has problems with reading localized GCC version
# message, so we set LANG=C. Fixed on trunk
LANG=C ./configure --prefix=$STORM_ROOT
cd build
make -j${PROCS} install
)

(
cd src
wget https://sourceforge.net/projects/boost/files/boost/1.61.0/boost_1_61_0.tar.bz2/download -O boost_1_61_0.tar.bz2
tar xjf boost_1_61_0.tar.bz2
cd boost_1_61_0
./bootstrap.sh --prefix=$HOME/storm-root
./b2 install -j${PROCS} --layout=tagged
)

(
cd src
git clone https://github.com/smtrat/carl.git carl.git
mkdir carl-build
cd carl-build
# requires cmake tweak
cmake ../carl.git -DCMAKE_INSTALL_PREFIX=$STORM_ROOT -DCMAKE_PREFIX_PATH=$STORM_ROOT -DUSE_GINAC=ON -DUSE_CLN=ON
make -j${PROCS} install
)

(cd src
git clone https://github.com/moves-rwth/storm.git storm.git
mkdir storm-build
cd storm-build
cmake ../storm.git -DCMAKE_INSTALL_PREFIX=$STORM_ROOT -DCMAKE_PREFIX_PATH=$STORM_ROOT
make -j${PROCS} install
)

