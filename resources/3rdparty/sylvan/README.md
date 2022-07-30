Sylvan
======
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CI testing](https://github.com/trolando/sylvan/actions/workflows/ci-build.yml/badge.svg)](https://github.com/trolando/sylvan/actions/workflows/ci-build.yml)

Sylvan is a parallel (multi-core) multi-terminal binary decision diagram library written in C.
Sylvan implements typical binary decision diagram operations also found in libraries like CUDD,
but provides scalable parallel execution of these operations and is more versatile thanks to
supporting custom decision diagram terminal types.

The main author of Sylvan is Tom van Dijk who can be reached via <tom@tvandijk.nl>.

**Table of Contents**

- [Features](#features)
- [Dependencies](#dependencies)
- [Usage](#usage)
- [Documentation](#documentation)
- [License](#license)
- [Publications](#publications)

## Features

Sylvan implements operations on binary decision diagrams supporting any kind of terminal:
- standard Boolean True/False, that is, ordinary BDDs
- integers, such as a function from a Boolean domain to long integers
- floating points, thus representing functions from a Boolean domain to real numbers
- any user-defined types, by storing 64-bit pointers in the leaves
- as an example, Sylvan has prototype support of GMP rationals (see `sylvan_gmp.c`)

Apart from ordinary binary decision diagrams, where each node has a two children depending on the value of
some Boolean variable, Sylvan also implements operations on so-called *list decision diagrams*,
which are a kind of multi-way decision diagrams.

Support for zero-suppressed decision diagrams (ZDDs) and tagged BDDs (TBDDs) is available in the
branches `zdd` and `tbdd` and will eventually be merged with the master branch.

The key feature of Sylvan is its auto-parallelization. Sylvan operations are run on the Lace framework
which enables fine-grained scalable parallelism. With little extra effort from the user of Sylvan, the
Lace framework ensures that Sylvan operations are actually performed by multiple threads. Furthermore,
Sylvan is thread-safe, i.e., multiple operations can be offered to the framework simultaneously.

## Dependencies

The most important dependency of Sylvan is currently part of the library, which is **Lace**.
In order to use Sylvan, Lace must be running.

If the `GMP` library (e.g. `libgmp-dev`) is found, Sylvan will build with support for GMP terminals.

## Usage

Simply build the project with **CMake**. 

The _/examples_ folder contains a number of example programs that use Sylvan. 
| Example program     | Description                                                    |
|---------------------|----------------------------------------------------------------| 
| `bddmc`             | Compute reachable states of a labeled transition system as BDD |
| `lddmc`             | Compute reachable states of a labeled transition system as LDD |
| `ldd2bdd`           | Convert LDD file to BDD file (labeled transition systems)      |
| `ldd2meddly`        | Convert LDD file to Meddly file (for comparisons to Meddly)    |
| `nqueens`           | Count the solutions to the N Queens problem                    |

It is possible to use Sylvan from other languages. Sylvan contains a prototype C++ bridge.
Bindings for other languages than C/C++ also exist:

-  Java/JNI bindings: https://github.com/utwente-fmt/jsylvan (outdated prototype)
-  Haskell bindings: https://github.com/adamwalker/sylvan-haskell
-  Python bindings: https://github.com/johnyf/dd
-  Rust bindings: https://github.com/daemontus/sylvan-sys

## Documentation

Documentation is available [on GitHub Pages](https://trolando.github.io/sylvan).

## License

Sylvan was initially developed at the [Formal Methods and Tools](http://fmt.ewi.utwente.nl/)
group at the University of Twente as part of the MaDriD project, which
was funded by NWO, and further by the [Formal Methods and Verification](http://fmv.jku.at/)
group at the Johannes Kepler University Linz as part of the RiSE project.

Sylvan is licensed with the Apache 2.0 license.

Please let us know if you use Sylvan in your projects and if you need
decision diagram operations that are currently not implemented in Sylvan.

The main repository of Sylvan is https://github.com/trolando/sylvan.  

## Publications

T. van Dijk (2016) [Sylvan: Multi-core Decision Diagrams](http://dx.doi.org/10.3990/1.9789036541602). PhD Thesis.

T. van Dijk and J.C. van de Pol (2016) [Sylvan: Multi-core Framework for Decision Diagrams](http://dx.doi.org/10.1007/s10009-016-0433-2>).  In: STTT (Special Issue), Springer.

T. van Dijk and J. van de Pol (2015) [Sylvan: Multi-core Decision Diagrams](http://dx.doi.org/10.1007/978-3-662-46681-0_60). In: TACAS 2015, LNCS 9035. Springer.

T. van Dijk and A.W. Laarman and J. van de Pol (2012) [Multi-Core BDD Operations for Symbolic Reachability](http://eprints.eemcs.utwente.nl/22166/). In: PDMC 2012, ENTCS. Elsevier.
