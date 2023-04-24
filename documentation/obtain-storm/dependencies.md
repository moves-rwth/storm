---
title: Dependencies
layout: default
---

<h1>Dependencies</h1>

{% include includes/toc.html %}


Storm depends on several other tools. Partly, they are packed with Storm. This page describes dependencies which are assumed to be present on the target system.
We both give a general list, as well as operating system specific hints how to install them.


## Compiler

For the compilation step, a C++17-compliant compiler is required. Storm is known to work with GCC, Clang, and AppleClang.

## General Dependencies

The following two lists provide an overview over the *required* and *recommended* dependencies of Storm. *Required* dependencies are absolutely essential for Storm to be compiled and must be installed. *Recommended* dependencies are optional, but not installing them may severely limit the offered functionality.

Required:
- git
- cmake
- boost
- cln
- gmp
- ginac
- autoreconf
- glpk
- hwloc

Recommended:
- [Z3](https://github.com/Z3Prover/z3){:target="_blank"} (not strictly required, but already needed for standard tasks like PRISM/JANI model building)
- [xerces-c](https://xerces.apache.org/xerces-c/){:target="_blank"} (needed for the parsing and export of XML files, in particular for GSPNs)
- [MathSAT](http://mathsat.fbk.eu/){:target="_blank"} (needed by the abstraction refinement engine, needs to be configured manually during the [configuration](manual-configuration.html#mathsat))


## OS specific preparations

We collected some platform specific hints to ease the installation of Storm on the supported operating systems.
Since Storm has some optional dependencies that enhance it's functionality, and some dependencies that are strictly required, we show how to install both the *required* and *recommended* dependencies.
The installation instructions of the *recommended* dependencies already include the *required* dependencies.

### <i class="fa fa-apple" aria-hidden="true"></i> macOS

Make sure that you use a recent macOS version.
You need to download and install Xcode or its command line tools (CLT) to have the suitable tools needed for compilation. For more details, we refer to [this tutorial](https://www.moncefbelyamani.com/how-to-install-xcode-homebrew-git-rvm-ruby-on-mac/){:target="_blank"}.

Furthermore, we recommend the usage of [Homebrew](https://brew.sh){:target="_blank"} to install the missing packages.

#### Apple Silicon Systems

{:.alert .alert-danger}
For troubleshooting building on ARM-based <i class="fa fa-apple" aria-hidden="true"></i>  Apple Silicon CPUs and for building using x86 emulations, please refer to [this page](apple-silicon.html){:.alert-link}.

- Required:
``` console
$ brew install automake cmake boost gmp glpk hwloc
```

- Recommended:
``` console
$ brew install automake cmake boost gmp glpk hwloc z3 xerces-c
```

#### Intel-based Systems

- Required:
``` console
$ brew install cln ginac automake cmake boost gmp glpk hwloc
```

- Recommended:
``` console
$ brew install cln ginac automake cmake boost gmp glpk hwloc z3 xerces-c
```


### <i class="icon-debian"></i> Debian and <i class="icon-ubuntu"></i> Ubuntu
<!-- If these are changed, also change them in `vm.md` -->

We currently support Debian from version 10 and Ubuntu from version 20.04.

- Required:
``` console
$ sudo apt-get install build-essential git cmake libboost-all-dev libcln-dev libgmp-dev libginac-dev automake libglpk-dev libhwloc-dev
```

- Recommended
``` console
$ sudo apt-get install build-essential git cmake libboost-all-dev libcln-dev libgmp-dev libginac-dev automake libglpk-dev libhwloc-dev libz3-dev libxerces-c-dev libeigen3-dev
```


## Manually installing dependencies <span class="label label-info">optional</span>

### CArL

Storm makes use of [CArL](https://github.com/moves-rwth/carl-storm){:target="_blank"} for the representation of rationals and rational functions. If you don't have it installed on your system, our build script will download and configure it automatically for you. However, under certain circumstances, you might want to install CArL yourself. This may for example be advantageous if you need to repeatedly build Storm from scratch or you want to change its source code. Installing CArL is as easy as

```console
$ git clone https://github.com/moves-rwth/carl-storm
$ cd carl-storm
$ mkdir build
$ cd build
$ cmake ..
$ make lib_carl
```

Once it is build, it will register itself to cmake so Storm can find your build automatically.

{:.alert .alert-warning}
There may be problems with this auto-detection mechanism if you have multiple versions of CArL installed. We strongly recommend to have CArL just once and completely remove the build folder of Storm if one is already present. Rerunning `cmake` should then pick up on the "new" version of CArL.

## Boost

Storm requires [Boost](http://www.boost.org/){:target="_blank"} to be available in a recent version. On the [supported operating systems](requirements.html) this can be easily achieved with readily available package managers.
If Boost is not present in a standard system locations (for example because it was manually built), it might not automatically be found by Storm. You can make Storm aware of the Boost location by passing `-DBOOST_ROOT=/path/to/boost` to the `cmake` invocation in the [configuration step](installation.html#configuration-step).
