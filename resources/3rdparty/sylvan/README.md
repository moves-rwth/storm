Sylvan [![Build Status](https://travis-ci.org/trolando/sylvan.svg?branch=master)](https://travis-ci.org/trolando/sylvan)
======
Sylvan is a parallel (multi-core) BDD library in C. Sylvan allows both sequential and parallel BDD-based algorithms to benefit from parallelism. Sylvan uses the work-stealing framework Lace and a scalable lockless hashtable to implement scalable multi-core BDD operations.

Sylvan is developed (&copy; 2011-2016) by the [Formal Methods and Tools](http://fmt.ewi.utwente.nl/) group at the University of Twente as part of the MaDriD project, which is funded by NWO. Sylvan is licensed with the Apache 2.0 license.

You can contact the main author of Sylvan at <t.vandijk@utwente.nl>. Please let us know if you use Sylvan in your projects.

Sylvan is available at: https://github.com/utwente-fmt/sylvan  
Java/JNI bindings: https://github.com/trolando/jsylvan  
Haskell bindings: https://github.com/adamwalker/sylvan-haskell

Publications
------------
T. van Dijk and J. van de Pol (2015) [Sylvan: Multi-core Decision Diagrams](http://dx.doi.org/10.1007/978-3-662-46681-0_60). In: TACAS 2015, LNCS 9035. Springer.

T. van Dijk and A.W. Laarman and J. van de Pol (2012) [Multi-Core BDD Operations for Symbolic Reachability](http://eprints.eemcs.utwente.nl/22166/). In: PDMC 2012, ENTCS. Elsevier.

Usage
-----
Simple examples can be found in the `examples` subdirectory. The file `simple.cpp` contains a toy program that 
uses the C++ objects to perform basic BDD manipulation.
The `mc.c` and `lddmc.c` programs are more advanced examples of symbolic model checking (with example models in the `models` subdirectory).

Sylvan depends on the [work-stealing framework Lace](http://fmt.ewi.utwente.nl/tools/lace) for its implementation. Lace is embedded in the Sylvan distribution.
To use Sylvan, Lace must be initialized first.
For more details, see the comments in `src/sylvan.h`.

### Basic functionality

To create new BDDs, you can use:
- `sylvan_true`: representation of constant `true`.
- `sylvan_false`: representation of constant `false`.
- `sylvan_ithvar(var)`: representation of literal &lt;var&gt; (negated: `sylvan_nithvar(var)`)

To follow the BDD edges and obtain the variable at the root of a BDD, you can use:
- `sylvan_var(bdd)`: obtain variable of the root node of &lt;bdd&gt; - requires that &lt;bdd&gt; is not constant `true` or `false`.
- `sylvan_high(bdd)`: follow high edge of &lt;bdd&gt;.
- `sylvan_low(bdd)`: follow low edge of &lt;bdd&gt;.

You need to manually reference BDDs that you want to keep during garbage collection:
- `sylvan_ref(bdd)`: add reference to &lt;bdd&gt;.
- `sylvan_deref(bdd)`: remove reference to &lt;bdd&gt;.
- `sylvan_protect(bddptr)`: add a pointer reference to the BDD variable &lt;bddptr&gt;
- `sylvan_unprotect(bddptr)`: remove a pointer reference to the BDD variable &lt;bddptr&gt;

It is recommended to use `sylvan_protect` and `sylvan_unprotect`.
The C++ objects handle this automatically.

The following 'primitives' are implemented:
- `sylvan_not(bdd)`: negation of &lt;bdd&gt;.
- `sylvan_ite(a,b,c)`: calculate 'if &lt;a&gt; then &lt;b&gt; else &lt;c&gt;'.
- `sylvan_and(a, b)`: calculate a and b
- `sylvan_or(a, b)`: calculate a or b
- `sylvan_nand(a, b)`: calculate not (a and b)
- `sylvan_nor(a, b)`: calculate not (a or b)
- `sylvan_imp(a, b)`: calculate a implies b
- `sylvan_invimp(a, b)`: calculate implies a
- `sylvan_xor(a, b)`: calculate a xor b
- `sylvan_equiv(a, b)`: calculate a = b
- `sylvan_diff(a, b)`: calculate a and not b
- `sylvan_less(a, b)`: calculate b and not a
- `sylvan_exists(bdd, vars)`: existential quantification of &lt;bdd&gt; with respect to variables &lt;vars&gt;. Here, &lt;vars&gt; is a conjunction of literals.
- `sylvan_forall(bdd, vars)`: universal quantification of &lt;bdd&gt; with respect to variables &lt;vars&gt;. Here, &lt;vars&gt; is a conjunction of literals.

### Other BDD operations

See `src/sylvan_bdd.h`, `src/sylvan_mtbdd.h` and `src/sylvan_ldd.h` for other implemented operations.
See `src/sylvan_obj.hpp` for the C++ interface.
 
### Garbage collection

Garbage collection is triggered when trying to insert a new node and no new bucket can be found within a reasonable upper bound. 
Garbage collection is stop-the-world and all workers must cooperate on garbage collection. (Beware of deadlocks if you use Sylvan operations in critical sections!)
- `sylvan_gc()`: manually trigger garbage collection.
- `sylvan_gc_enable()`: enable garbage collection.
- `sylvan_gc_disable()`: disable garbage collection.

### Table resizing

During garbage collection, it is possible to resize the nodes table and the cache.
Sylvan provides two default implementations: an agressive version that resizes every time garbage collection is performed,
and a less agressive version that only resizes when at least half the table is full.
This can be configured in `src/sylvan_config.h`
It is not possible to decrease the size of the nodes table and the cache.

### Dynamic reordering

Dynamic reordening is currently not supported.
For now, we suggest users find a good static variable ordering.

Troubleshooting
---------------
Sylvan may require a larger than normal program stack. You may need to increase the program stack size on your system using `ulimit -s`. Segmentation faults on large computations typically indicate a program stack overflow.

### I am getting the error "unable to allocate memory: ...!"
Sylvan allocates virtual memory using mmap. If you specify a combined size for the cache and node table larger than your actual available memory you may need to set `vm.overcommit_memory` to `1`. E.g. `echo 1 > /proc/sys/vm/overcommit_memory`. You can make this setting permanent with `echo "vm.overcommit_memory = 1" > /etc/sysctl.d/99-sylvan.conf`. You can verify the setting with `cat /proc/sys/vm/overcommit_memory`. It should report `1`.
