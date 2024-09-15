Storm - A Modern Probabilistic Model Checker
============================================

[![Build Status](https://github.com/moves-rwth/storm/workflows/Build%20Test/badge.svg)](https://github.com/moves-rwth/storm/actions)
[![GitHub release](https://img.shields.io/github/release/moves-rwth/storm.svg)](https://github.com/moves-rwth/storm/releases/)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1181896.svg)](https://doi.org/10.5281/zenodo.1181896)

This is a custom fork of Storm for my Bachelor and Master Thesis at IIT Delhi, titled, "`INTERLEAVE` : An Empirically Faster Symbolic Algorithm for Maximal End Component Decomposition of MDPs" and advised by Suguman Bansal (Georgia Tech) and Subodh Sharma (IIT Delhi). Most of the code (except the new algorithm added -- `INTERLEAVE` and minor bug fixes) has been adapted from [the code for Felix Faber's Bachelor Thesis at RWTH Aachen](https://doi.org/10.5281/zenodo.8311805). I am grateful to Felix for writing an excellent thesis, and for writing code that was easy to understand and extend. Below is a brief description of the relevant files.

- `src/storm/storage/SymbolicMEC.h` and `src/storm/storage/SymbolicMEC_stats.h` contain the algorithm implementations (the `_stats` files additionally count the number of transition BDD operations)
- `src/storm/storage/SymbolicOperations.h` and `src/storm/storage/SymbolicOperations_stats.h` contain the implementations of symbolic operations (including a new implementation of `pick` which I tried out and added support for to the `src/storm/storage/dd`, `src/storm/storage/dd/cudd` and `src/storm/storage/dd/sylvan` folders)
- `src/storm/storage/SymbolicSCCDecomposition.h` and `src/storm/storage/SymbolicSCCDecomposition_stats.h` contain symbolic SCC decomposition algorithm implementations
- `src/storm/storage/THESIS_DEBUG.h` contains miscellaneous definitions of the algorithms we benchmarked.

To run any of the symbolic MEC decomposition algorithms, call the `storm` binary as usual, adding the argument `--benchmarkForceMECDecompositionAlgorithm <n>` with `n = 1,2,3` for `NAIVE, LOCKSTEP, INTERLEAVE`. Using `n = 4,5,6` also counts and outputs the number of symbolic operations performed by the `NAIVE, LOCKSTEP, INTERLEAVE` algorithms. Usual Storm GitHub README is below.

Usage
-----------------------------
The Storm website [www.stormchecker.org](https://www.stormchecker.org/) provides documentation and background information.
- For installation and usage instructions, check out the documentation found in [Getting Started](http://www.stormchecker.org/getting-started.html).
- Video tutorials and interactive code examples are available in [Tutorials](https://www.stormchecker.org/tutorials.html).
- The [Storm starter project](https://github.com/moves-rwth/storm-project-starter-cpp/) provides a starting point for incorporating Storm into C++ projects.
- Storm provides a Python interface called [stormpy](https://moves-rwth.github.io/stormpy/) for easy prototyping and interaction with Storm.
- In case of any issues installing or using Storm, [let us know](https://www.stormchecker.org/documentation/obtain-storm/troubleshooting.html).


Examples
-----------------------------
Various benchmarks together with example invocations of Storm can be found at the [Quantitative Verification Benchmark Set (QVBS)](http://qcomp.org/benchmarks).
Additional input files for Storm can be obtained from the [storm-examples](https://github.com/moves-rwth/storm-examples) repository.


Developers
-----------------------------
We welcome contributions to Storm.
Our [information for developers](doc/developers.md) contains general information to get started with the development on Storm.
Feel free to contact us in case you need any pointers or help.


Authors
-----------------------------
Storm has been developed at RWTH Aachen University.

###### Principal developers
* Christian Hensel
* Sebastian Junges
* Joost-Pieter Katoen
* Tim Quatmann
* Matthias Volk

###### Developers (lexicographical order)
* Jana Berger
* Alexander Bork
* David Korzeniewski
* Jip Spel

###### Contributors (lexicographical order)
* Daniel Basgöze
* Dimitri Bohlender
* Harold Bruintjes
* Michael Deutschen
* Linus Heck
* Thomas Heinemann
* Thomas Henn
* Tom Janson
* Jan Karuc
* Joachim Klein
* Gereon Kremer
* Sascha Vincent Kurowski
* Hannah Mertens
* Stefanie Mohr
* Stefan Pranger
* Svenja Stein
* Manuel Sascha Weiand
* Lukas Westhofen

For an exhaustive list of contributors and more details, see the [Github page](https://github.com/moves-rwth/storm/graphs/contributors).


Citing Storm
-----------------------------
If you want to cite Storm, please use the most recent paper in [this category](https://www.stormchecker.org/publications.html).
