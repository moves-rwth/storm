---
title: Manual Configuration
layout: default
---

<h1>Manual Configuration</h1>

{% include includes/toc.html %}


Designed for users that need particular features and people developing under Storm, this guide will detail how to perform a manual configuration of the build process.

There are a number of **cmake options** that modify how Storm is built.
All of them can be set in the [configuration step](compile.html#configuration-step) by providing them to `cmake`.
To modify the options after the configuration step, you may run `ccmake ..` (assuming that you currently are in `STORM_DIR/build`). After changing these options you need to rebuild Storm using `make`.

We don't detail all options here, but only selected ones.

## Developer

For developers, we offer the option `-DSTORM_DEVELOPER=ON`. This enables

- more cmake output
- more warnings
- debug and trace log levels (they are not always printed and may need to be enabled; in the Storm main binary, this can be done by providing `--debug` or `--trace`)

## Link-time optimization

By default, Storm uses link-time optimization (LTO) to enable even more optimizations that are done at link time. This, however, comes at a penalty for building in terms of both time and memory, as the linking step becomes very resource-intensive. Disabling LTO via `-DSTORM_USE_LTO=OFF` avoids this at the price of producing slower binaries.

## Portability

By default, the binaries will be built specifically for your machine, because this enables a wider range of optimizations to be enabled. If you need your binaries to be portable (in the sense that they could run on another machine that has all required dependencies in their right version), you can set `-DSTORM_PORTABLE=ON` at the cost of producing slower binaries.


## Intel Threading Building Blocks (TBB)

[Intel's Threading Building Blocks](https://www.threadingbuildingblocks.org/){:target="_blank"} is a framework for parallelism. If available, Storm can use it to parallelize some operations like (some but not all) matrix-vector multiplications. If TBB is installed, you can enable it by setting the cmake option `-DSTORM_USE_INTELTBB=ON` and adding the command line argument `--enable-tbb` to Storm.

## Gurobi

[Gurobi](http://www.gurobi.com/){:target="_blank"} is a commercial high-performance solver for (mixed-integer) linear programs (and similar problems). A free license can be obtained for academic purposes. Storm provides an implementation of its (MI)LP solver interface that uses Gurobi (provided Storm has been compiled with support for it). To enable Gurobi, specify the option `-DSTORM_USE_GUROBI=ON` and set a hint at where to find Gurobi via `-DGUROBI_ROOT=/path/to/gurobi`.

## MathSAT

[MathSAT](http://mathsat.fbk.eu/){:target="_blank"} is a high-performance SMT solver that can be used as an alternative to [Z3](https://github.com/Z3Prover/z3){:target="_blank"}. In contrast to Z3, it provides native support for AllSat (enumeration of all satisfying models of a formula) and may generate Craig interpolants. This makes MathSAT particularly useful in the [abstraction-refinement engine]({{ site.github.url }}/documentation/background/engines.html#abstraction-refinement). To enable MathSAT, you need to set `-DMSAT_ROOT=/path/to/mathsat/root` where MathSAT's root directory is required to contain the `include` and `lib` folders with the appropriate files (if you download and unpack it, its the directory into which you unpacked it).
