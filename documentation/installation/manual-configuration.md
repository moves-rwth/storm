---
title: Manual Configuration
layout: default
documentation: true
category_weight: 3
categories: [Installation]
---

{% include includes/toc.html %}

Designed for users that need particular features and people developing under Storm, this guide will detail how to perform a manual configuration of the build process.

## Manually installing dependencies

### CArL

Storm makes use of [CArL](https://github.com/smtrat/carl){:target="_blank"} for the representation of rationals and rational functions. If you don't have it installed on your system, our build script will download and configure it automatically for you. However, under certain circumstances, you might want to install CArL yourself. This may for example be advantageous if you need to repeatedly build Storm from scratch or you want to change its source code. Installing CArL is as easy as

```console
$ git clone https://github.com/smtrat/carl
$ cd carl
$ mkdir build
$ cd build
$ cmake -DUSE_CLN_NUMBERS=ON -DUSE_GINAC=ON -DTHREAD_SAFE=ON ..
$ make lib_carl
```

Once it is build, it will register itself to cmake so Storm can find your build automatically.

{:.alert .alert-warning}
There may be problems with this auto-detection mechanism if you have multiple versions of CArL installed. We strongly recommend to have CArL just once and completely remove the build folder of Storm if one is already present. Rerunning `cmake` should then pick up on the "new" version of CArL.

### Boost

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

## CMake Options

There are a number of cmake options that modify how Storm is built. All of them can be set in the [configuration step](installation.html#configuration-step) by providing them to `cmake`. We don't detail all options here, but only selected ones.

### Developer

For developers, we offer the option `STORM_DEVELOPER=ON`. This enables

- more cmake output
- more warnings
- debug and trace log levels (they are not always printed and may need to be enabled; in the Storm main binary, this can be done by providing `--debug` or `--trace`)

### Link-time optimization

By default, Storm uses link-time optimization (LTO) to enable even more optimizations that are done at link time. This, however, comes at a penalty for building in terms of both time and memory, as the linking step becomes very resource-intensive. Disabling LTO via `-DSTORM_USE_LTO=OFF` avoids this at the price of producing slower binaries.

### Portability

By default, the binaries will be built specifically for your machine, because this enables a wider range of optimizations to be enabled. If you need your binaries to be portable (in the sense that they could run on another machine that has all required dependencies in their right version), you can set `-DSTORM_PORTABLE=ON` at the cost of producing slower binaries.

### Enabling libraries

Some libraries will not be enabled by default as they are not critical (or only for some tasks) and may not be easily available.

#### Intel Threading Building Blocks (TBB)

[Intel's Threading Building Blocks](https://www.threadingbuildingblocks.org/){:target="_blank"} is a framework for parallelism. If available, Storm can use it to parallelize some operations like (some but not all) matrix-vector multiplications. If TBB is installed, you can enable it via `-DSTORM_USE_INTELTBB=ON`.

#### Gurobi

[Gurobi](http://www.gurobi.com/){:target="_blank"} is a commercial high-performance solver for (mixed-integer) linear programs (and similar problems). A free license can be obtained for academic purposes. Storm provides an implementation of its (MI)LP solver interface that uses Gurobi (provided Storm has been compiled with support for it). To enable Gurobi, specify the option `-DSTORM_USE_GUROBI=ON` and set a hint at where to find Gurobi via `-DGUROBI_ROOT=/path/to/gurobi`.

#### MathSAT

[MathSAT](http://mathsat.fbk.eu/){:target="_blank"} is a high-performance SMT solver that can be used as an alternative to [Z3](https://github.com/Z3Prover/z3){:target="_blank"}. In contrast to Z3, it provides native support for AllSat (enumeration of all satisfying models of a formula) and may generate Craig interpolants. This makes MathSAT particularly useful in the [abstraction-refinement engine]({{ site.github.url }}/documentation/usage/engines.html#abstraction-refinement). To enable MathSAT, you need to set `-DMSAT_ROOT=/path/to/mathsat/root` where MathSAT's root directory is required to contain the `include` and `lib` folders with the appropriate files (if you download and unpack it, its the directory into which you unpacked it).
