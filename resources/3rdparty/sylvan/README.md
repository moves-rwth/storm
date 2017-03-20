Sylvan [![Build Status](https://travis-ci.org/trolando/sylvan.svg?branch=master)](https://travis-ci.org/trolando/sylvan)
======
Sylvan is a parallel (multi-core) MTBDD library written in C. Sylvan
implements parallelized operations on BDDs, MTBDDs and LDDs. Both
sequential and parallel BDD-based algorithms can benefit from
parallelism. Sylvan uses the work-stealing framework Lace and parallel
datastructures to implement scalable multi-core operations on decision
diagrams.

Sylvan is developed (&copy; 2011-2016) by the [Formal Methods and Tools](http://fmt.ewi.utwente.nl/)
group at the University of Twente as part of the MaDriD project, which
was funded by NWO, and (&copy; 2016-2017) by the [Formal Methods and Verification](http://fmv.jku.at/)
group at the Johannes Kepler University Linz as part of the RiSE project.
Sylvan is licensed with the Apache 2.0 license.

The main author of Sylvan is Tom van Dijk who can be reached via <tom@tvandijk.nl>.
Please let us know if you use Sylvan in your projects and if you need
decision diagram operations that are currently not implemented in Sylvan.

The main repository of Sylvan is https://github.com/trolando/sylvan. A
mirror is available at https://github.com/utwente-fmt/sylvan.

Bindings for other languages than C/C++ also exist:

-  Java/JNI bindings: https://github.com/utwente-fmt/jsylvan
-  Haskell bindings: https://github.com/adamwalker/sylvan-haskell
-  Python bindings: https://github.com/johnyf/dd

**Documentation** is available [at GitHub Pages](https://trolando.github.com/sylvan).

Publications
------------
T. van Dijk (2016) [Sylvan: Multi-core Decision Diagrams](http://dx.doi.org/10.3990/1.9789036541602). PhD Thesis.

T. van Dijk and J.C. van de Pol (2016) [Sylvan: Multi-core Framework for Decision Diagrams](http://dx.doi.org/10.1007/s10009-016-0433-2>).  In: STTT (Special Issue), Springer.

T. van Dijk and J. van de Pol (2015) [Sylvan: Multi-core Decision Diagrams](http://dx.doi.org/10.1007/978-3-662-46681-0_60). In: TACAS 2015, LNCS 9035. Springer.

T. van Dijk and A.W. Laarman and J. van de Pol (2012) [Multi-Core BDD Operations for Symbolic Reachability](http://eprints.eemcs.utwente.nl/22166/). In: PDMC 2012, ENTCS. Elsevier.

