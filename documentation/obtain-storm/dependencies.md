---
title: Dependencies
layout: default
---

<h1>Dependencies</h1>

{% include includes/toc.html %}


Storm depends on several other tools. Partly, they are packed with Storm. This page describes dependencies which are assumed to be present on the target system.
We both give a general list, as well as operating system specific hints how to install them.


## Compiler

For the compilation step, a C++14-compliant compiler is required. Storm is known to work with

- GCC 5.3, GCC 6
- clang 3.5.0
- AppleClang 8.0.0

Newer versions of these compilers will probably work, but are not necessarily tested. In particular, the following list of compilers is known to *not* work.

- GCC versions 4.9.1 and older
- clang 3.4 and older

## General Dependencies

The following two lists provide an overview over the *required* and *recommended* dependencies of Storm. *Required* dependencies are absolutely essential for Storm to be compiled and must be installed. *Recommended* dependencies are optional, but not installing them may severely limit the offered functionality.

Required:
- git
- cmake
- boost (>= 1.61)
- cln
- gmp
- ginac
- autoreconf
- glpk
- hwloc

Recommended:
- [Z3](https://github.com/Z3Prover/z3) (not strictly required, but already needed for standard tasks like PRISM/JANI model building)
- xercesc (installation prevents an expensive part of the build step)
- [Eigen3](http://eigen.tuxfamily.org/index.php) (installation prevents an expensive part of the build step)
- [MathSAT](http://mathsat.fbk.eu/){:target="_blank"} (needed by the abstraction refinement engine, needs to be configured manually during the [configuration](manual-configuration.html#mathsat))


## OS specific preparations

We collected some platform specific hints to ease the installation of Storm on the supported operating systems. Since Storm has some optional dependencies that enhance it's functionality, and some dependencies that are strictly required, we show how to install both the *required* and *recommended* dependencies. The installation instructions of the *recommended* dependencies are to be understood incrementally, i.e. in addition to the required dependencies.

### <i class="fa fa-apple" aria-hidden="true"></i> macOS 10.12 "Sierra" and higher

First of all, you need to download and install Xcode and its command line utilities to have the suitable command line tools. For more details, we refer to [this tutorial](https://www.moncefbelyamani.com/how-to-install-xcode-homebrew-git-rvm-ruby-on-mac/){:target="_blank"}.

Furthermore, we recommend the usage of [Homebrew](https://brew.sh){:target="_blank"} to install the missing packages, but MacPorts might (at some point) have the desired dependencies as well.

- Required:
``` console
$ brew install cln ginac automake cmake
$ brew install boost --c++11
$ brew install gmp --c++11
$ brew install glpk
$ brew install hwloc
```

- Recommended:
``` console
$ brew install z3 xerces-c
```

### <i class="icon-debian"></i> Debian 9 "Stretch"

- Required:
``` console
$ sudo apt-get install git cmake libboost-all-dev libcln-dev libgmp-dev libginac-dev automake libglpk-dev
```

- Recommended
``` console
$ sudo apt-get install libz3-dev libxerces-c-dev libeigen3-dev
```

### <i class="icon-ubuntu"></i> Ubuntu 16.10 "Yakkety Yak" and higher

- Required:
``` console
$ sudo apt-get install git cmake libboost-all-dev libcln-dev libgmp-dev libginac-dev automake libglpk-dev libhwloc-dev
```

- Recommended:
``` console
$ sudo apt-get install libz3-dev libxerces-c-dev libeigen3-dev
```

## Manually installing dependencies <span class="label label-info">optional</span>

### CArL

Storm makes use of [CArL](https://github.com/smtrat/carl){:target="_blank"} for the representation of rationals and rational functions. If you don't have it installed on your system, our build script will download and configure it automatically for you. However, under certain circumstances, you might want to install CArL yourself. This may for example be advantageous if you need to repeatedly build Storm from scratch or you want to change its source code. Installing CArL is as easy as

```console
$ git clone -b master14 https://github.com/smtrat/carl
$ cd carl
$ mkdir build
$ cd build
$ cmake -DUSE_CLN_NUMBERS=ON -DUSE_GINAC=ON -DTHREAD_SAFE=ON ..
$ make lib_carl
```

{:.alert .alert-warning}
Make sure you are using the `master14` branch of CArL as the current master branch uses a newer C++ standard that Storm does not require and therefore does not enable.

Once it is build, it will register itself to cmake so Storm can find your build automatically.

{:.alert .alert-warning}
There may be problems with this auto-detection mechanism if you have multiple versions of CArL installed. We strongly recommend to have CArL just once and completely remove the build folder of Storm if one is already present. Rerunning `cmake` should then pick up on the "new" version of CArL.

## Boost

Storm requires [Boost](http://www.boost.org/){:target="_blank"} to be available in a version that is at least 1.61. On the [supported operating systems](requirements.html) this can be easily achieved with readily available package managers. If your system does not allow for an easy installation of this Boost version, you might need to build it yourself.

```console
$ wget https://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.gz
$ tar -xzf boost_1_63_0.tar.gz
$ cd boost_1_63_0
$ ./bootstrap.sh
$ ./bjam
```

If you want to install Boost in some other location, you can provide a `--prefix=path/to/installation/prefix` to bootstrap.

If you installed your self-built Boost version into standard system locations, it should automatically be found by Storm. However, if it resides in a non-standard location, you need to make Storm aware of it, by passing `-DBOOST_ROOT=/path/to/boost` to the `cmake` invocation in the [configuration step](installation.html#configuration-step).
